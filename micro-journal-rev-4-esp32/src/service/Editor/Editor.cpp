#include "Editor.h"
#include "app/app.h"
#include "display/display.h"

//
#include "service/WordCounter/WordCounter.h"

//
// EDITOR CLASS IMPLEMENTATION
//

// defined below - moves a file offset back to a UTF-8 character start
static size_t alignToCharStart(File &file, size_t pos);

//
// Editor Initialization with column and row setup
void Editor::init(int cols, int rows, const EditorFont *font)
{
    // Define Screen Size
    this->cols = cols;
    this->rows = rows;

    // Active font (may be NULL for fixed-layout backends)
    this->font = font;

    //
    resetBuffer();
    composer.reset();
    updateScreen();

    //
    savingInProgress = false;

    // You can add any additional setup logic here
    _log("Editor initialized with columns: %d, rows: %d, font: %s\n",
         cols, rows, font ? font->id : "(none)");
}

// Given the fileName, go through the loading process
// initialize FileBuffer and ScreenBuffer
void Editor::loadFile(String fileName)
{
    if (savingInProgress)
    {
        _log("Save is is progress. Load file skipped\n");
        return;
    }

    // app status
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't load.\n";
        app["screen"] = ERRORSCREEN;

        //
        _debug(app["error"]);

        return;
    }

    //
    _log("Editor loading file [%s]\n", fileName.c_str());

    // Step 1. Create file if necessary
    if (!gfs()->exists(fileName.c_str()))
    {
        _debug("Creating an empty file since it's new\n");
        File file = gfs()->open(fileName.c_str(), "w");
        if (!file)
        {
            //
            app["error"] = format("Failed to create a file. %s\n", fileName.c_str());
            app["screen"] = ERRORSCREEN;

            //
            _log(app["error"].as<const char *>());

            return;
        }

        //
        file.close();
        delay(100);

        //
        _debug("File created. %s\n", fileName.c_str());
    }

    // Save filen name
    this->fileName = fileName;

    // Open File
    File file = gfs()->open(fileName.c_str(), "r");
    if (!file)
    {
        //
        app["error"] = format("file open failed %s\n", fileName.c_str());
        app["screen"] = ERRORSCREEN;

        //
        _debug(app["error"]);

        return;
    }

    // Determine file size and set buffer accordingly
    fileSize = file.size();
    _debug("File: %s of size: %d\n", fileName.c_str(), fileSize);

    // calcualte the file offset
    seekPos = 0;
    int stepSize = BUFFER_SIZE / 2; // use half of the buffer
    if (fileSize > 0)
    {
        // this offset will offer last portion of the buffer
        seekPos = (fileSize / stepSize) * stepSize;
    }

    // when it is exactly the buffer end
    // go one buffer behind so that screen will show something
    if (fileSize == seekPos && seekPos > 0)
    {
        if (seekPos > stepSize)
            seekPos -= stepSize;
        else
            // defensive code in order for the offset not to go negative (MAX in unsigned in)
            seekPos = 0;
    }

    // never start the window in the middle of a UTF-8 character
    seekPos = alignToCharStart(file, seekPos);

    _log("File seekPos: %d\n", seekPos);

    // move the file position to offset
    if (!file.seek(seekPos))
    {
        //
        file.close();
        delay(100);

        //
        app["error"] = format("Failed to seek file pointer. fileSize: %d seekPos: %d\n", fileSize, seekPos);
        app["screen"] = ERRORSCREEN;
        _debug(app["error"].as<const char *>());

        return;
    }

    // reset the buffer
    resetBuffer();
    composer.reset();

    // Read file content into text buffer
    int bufferSize = 0;
    while (file.available())
    {
        buffer[bufferSize++] = file.read();
    }
    cursorPos = bufferSize;

    // this window mirrors [seekPos, seekPos+bufferSize) on disk exactly
    loadedLength = bufferSize;
    pageChanged = true;

    //
    file.close();
    delay(100);

    // log
    _debug("Editor::loadFile size: %d, seek: %d, buffer: %d, cursor: %d\n",
           fileSize, seekPos, bufferSize, cursorPos);

    // Update the Screen Buffer
    updateScreen();

    // Update the word count
    wordCountFile = wordcounter_file(fileName.c_str());
    wordCountBuffer = wordcounter_buffer(buffer);

    // update the word count in config
    int file_index = app["config"]["file_index"].as<int>();
    app["config"][format("wordcount_file_%d", file_index)] = wordCountFile;
    app["config"][format("wordcount_buffer_%d", file_index)] = wordCountBuffer;

    //
    config_save();
}

//
// UTF-8 CHARACTER NAVIGATION
//

// back up to the lead byte of the character at/before pos
int Editor::snapToCharStart(int pos)
{
    while (pos > 0 && utf8_is_continuation((uint8_t)buffer[pos]))
        pos--;
    return pos;
}

// start of the character before pos
int Editor::prevCharStart(int pos)
{
    if (pos <= 0)
        return 0;
    return snapToCharStart(pos - 1);
}

// start of the character after pos
int Editor::nextCharStart(int pos)
{
    int size = getBufferSize();
    if (pos >= size)
        return size;

    pos += utf8_length((uint8_t)buffer[pos]);
    if (pos > size)
        pos = size;
    return pos;
}

// Move a file offset back to the start of a UTF-8 character so a window
// never begins in the middle of a multi-byte sequence.
static size_t alignToCharStart(File &file, size_t pos)
{
    while (pos > 0)
    {
        if (!file.seek(pos))
            break;

        int b = file.read();
        if (b < 0 || !utf8_is_continuation((uint8_t)b))
            break;

        pos--;
    }
    return pos;
}

// Copy `count` bytes from the current position of `src` to `dst`.
// Returns false if fewer than `count` bytes could be read.
static bool copyFileChunk(File &src, File &dst, size_t count)
{
    const size_t chunkSize = 512;
    uint8_t chunk[chunkSize];
    size_t remaining = count;
    while (remaining > 0)
    {
        size_t toRead = remaining < chunkSize ? remaining : chunkSize;
        size_t readSize = src.read(chunk, toRead);
        if (readSize == 0)
            return false;
        dst.write(chunk, readSize);
        remaining -= readSize;
    }
    return true;
}

bool Editor::saveFile()
{
    // TEMPORARY instrumentation for save-blocking-typing diagnosis
    unsigned long saveTimingStart = millis();

    if (savingInProgress)
    {
        _log("Save already in progress, skipping.\n");
        return false;
    }
    savingInProgress = true;

    //
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't save.\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        savingInProgress = false;
        return false;
    }

    // if already saved nothing to do
    if (this->saved)
    {
        _log("File already saved. No operation required.\n");
        savingInProgress = false;
        return true;
    }

    //
    _log("Saving file %s\n", fileName.c_str());

    // The buffer is a window onto [seekPos, seekPos+loadedLength) on disk.
    // Anything outside that window (before seekPos, after windowEnd) must
    // survive the save untouched.
    size_t newLength = getBufferSize();
    size_t windowEnd = seekPos + loadedLength;
    bool hasTrailingData = windowEnd < fileSize;

    // TEMPORARY instrumentation for save-blocking-typing diagnosis
    bool tookSplicePath = hasTrailingData || newLength < loadedLength;

    // FAST PATH: window is the tail of the file and isn't shrinking -
    // just overwrite/extend in place, no rewrite of the rest of the file needed.
    if (!hasTrailingData && newLength >= loadedLength)
    {
        File file = gfs()->open(fileName.c_str(), "r+"); // read/write, no truncate
        if (!file)
        {
            // If file doesn't exist, create it
            file = gfs()->open(fileName.c_str(), "w+"); // create + read/write
        }

        if (!file)
        {
            //
            app["error"] = "Failed to open file for writing\n";
            app["screen"] = ERRORSCREEN;

            //
            _log(app["error"]);
            savingInProgress = false;
            return false;
        }

        // Seek to the last loaded offset
        if (!file.seek(seekPos))
        {
            _log("Failed to seek file pointer\n");
            file.close();
            delay(100);

            savingInProgress = false;
            return false;
        }
        _log("Writing file at: %d\n", seekPos);

        // writing the file content
        size_t length = file.print(buffer);
        _log("File written: %d bytes\n", length);

        file.close();
        delay(100);

        fileSize = seekPos + length;
        loadedLength = length;
    }

    // SPLICE PATH: either the tail is shrinking, or this window sits in the
    // middle of the file and there is trailing data after it that must be
    // preserved. Rewrite the file as [prefix][new window content][suffix].
    else
    {
        String tempFileName = format("%s.tmp", fileName.c_str());
        if (gfs()->exists(tempFileName.c_str()))
        {
            gfs()->remove(tempFileName.c_str());
        }

        if (!gfs()->rename(fileName.c_str(), tempFileName.c_str()))
        {
            app["error"] = "Save failed (rename for splice)\n";
            app["screen"] = ERRORSCREEN;
            _log(app["error"].as<const char *>());
            savingInProgress = false;
            return false;
        }

        File src = gfs()->open(tempFileName.c_str(), "r");
        File dst = gfs()->open(fileName.c_str(), "w");
        bool ok = src && dst;

        if (ok)
            ok = copyFileChunk(src, dst, seekPos); // prefix, unchanged

        if (ok)
            dst.print(buffer); // this window's edited content

        if (ok && fileSize > windowEnd)
        {
            // suffix, unchanged
            ok = src.seek(windowEnd) && copyFileChunk(src, dst, fileSize - windowEnd);
        }

        if (src)
            src.close();
        if (dst)
            dst.close();
        delay(100);

        if (!ok)
        {
            // Restore the original file so a transient I/O error never
            // leaves the journal missing or half-written.
            app["error"] = "Save failed (splice)\n";
            app["screen"] = ERRORSCREEN;
            _log(app["error"].as<const char *>());

            gfs()->remove(fileName.c_str());
            gfs()->rename(tempFileName.c_str(), fileName.c_str());

            savingInProgress = false;
            return false;
        }

        gfs()->remove(tempFileName.c_str());

        fileSize = fileSize + (newLength - loadedLength);
        loadedLength = newLength;
    }

    // flag to save
    this->saved = true;

    // Persist the word count alongside the file itself, instead of on a
    // fixed timer (see wordcounter_service()) - that way writing config.json
    // only ever happens at a moment a flash write was going to happen anyway,
    // and never interrupts active typing.
    wordCountBuffer = wordcounter_buffer(buffer);
    int file_index = app["config"]["file_index"].as<int>();
    app["config"][format("wordcount_buffer_%d", file_index)] = wordCountBuffer;
    config_save();

    //
    savingInProgress = false;

    // TEMPORARY instrumentation for save-blocking-typing diagnosis
    printf("[save] %s path, %d bytes buffer: %lu ms\n",
           tookSplicePath ? "SPLICE" : "fast",
           (int)newLength,
           millis() - saveTimingStart);

#if defined(DEBUG) && defined(BOARD_PICO)
    printMemoryUsage();
#endif

    return true;
}

// Read `length` bytes starting at `offset` from disk into the buffer,
// replacing whatever window was loaded before. Used by paging so the
// in-memory buffer can slide to any part of the file, not just the tail.
bool Editor::loadWindow(size_t offset, size_t length)
{
    JsonDocument &app = status();

    File file = gfs()->open(fileName.c_str(), "r");
    if (!file)
    {
        app["error"] = format("file open failed %s\n", fileName.c_str());
        app["screen"] = ERRORSCREEN;
        _debug(app["error"]);
        return false;
    }

    // never start the window in the middle of a UTF-8 character - widen
    // the window backward so the split character is wholly included
    size_t alignedOffset = alignToCharStart(file, offset);
    length += offset - alignedOffset;
    offset = alignedOffset;

    if (!file.seek(offset))
    {
        file.close();
        delay(100);

        app["error"] = format("Failed to seek file pointer. offset: %d\n", offset);
        app["screen"] = ERRORSCREEN;
        _debug(app["error"].as<const char *>());
        return false;
    }

    resetBuffer();
    composer.reset();
    size_t bytesRead = 0;
    while (bytesRead < length && file.available())
    {
        buffer[bytesRead++] = file.read();
    }

    file.close();
    delay(100);

    seekPos = offset;
    loadedLength = bytesRead;
    pageChanged = true;

    _debug("Editor::loadWindow offset: %d length: %d read: %d\n", offset, length, bytesRead);

    return true;
}

// Load the chunk of the file that ends exactly where the current window
// starts, so the writer can keep scrolling back through earlier text.
void Editor::pageBackward()
{
    if (seekPos == 0)
    {
        _log("pageBackward: already at the start of the file\n");
        return;
    }

    if (!saved && !saveFile())
    {
        _log("pageBackward: flush failed, staying on current page\n");
        return;
    }

    size_t windowEnd = seekPos;
    size_t stepSize = BUFFER_SIZE / 2;
    size_t newSeekPos = (windowEnd > stepSize) ? windowEnd - stepSize : 0;

    if (!loadWindow(newSeekPos, windowEnd - newSeekPos))
        return;

    // land at the end, continuing the upward motion seamlessly
    cursorPos = getBufferSize();
    updateScreen();
}

// Load the chunk of the file that starts exactly where the current window
// ends, so the writer can scroll forward again after paging backward.
void Editor::pageForward()
{
    if (!saved && !saveFile())
    {
        _log("pageForward: flush failed, staying on current page\n");
        return;
    }

    size_t windowEnd = seekPos + loadedLength;
    if (windowEnd >= fileSize)
    {
        // already at the live tail - nothing further on disk
        return;
    }

    size_t stepSize = BUFFER_SIZE / 2;
    size_t remaining = fileSize - windowEnd;
    size_t toLoad = (remaining <= stepSize) ? remaining : stepSize;

    if (!loadWindow(windowEnd, toLoad))
        return;

    // land at the start, continuing the downward motion seamlessly
    cursorPos = 0;
    updateScreen();
}

// The buffer filled up while typing. Flush it, then keep going from
// wherever this window ended - either a fresh empty tail window, or the
// next chunk of on-disk content if this wasn't the tail. Replaces the old
// behaviour of unconditionally jumping back to the tail, which would have
// discarded the writer's place (and unsaved on-disk content past it) when
// triggered on a non-tail window.
void Editor::advanceWindow()
{
    // TEMPORARY instrumentation for save-blocking-typing diagnosis: this
    // save is NOT gated by the idle timer in WP_check_saved - it runs
    // synchronously on the keystroke path itself whenever the buffer fills.
    printf("[save] advanceWindow: buffer full, forced save starting\n");

    if (!saveFile())
    {
        _log("advanceWindow: flush failed, buffer is full and can't advance\n");
        return;
    }

    size_t windowEnd = seekPos + loadedLength;
    if (windowEnd >= fileSize)
    {
        // was at the tail - open a fresh empty window to keep typing into

        // The buffer saveFile() just flushed is now permanently on disk,
        // outside the window we're about to load. Fold its count into the
        // running file total now, before resetBuffer() below wipes it --
        // otherwise it's silently dropped the moment wordcounter_service()
        // next recomputes wordCountBuffer against the new, much smaller
        // buffer, undercounting every page after the first.
        wordCountFile += wordCountBuffer;
        wordCountBuffer = 0;

        JsonDocument &app = status();
        int file_index = app["config"]["file_index"].as<int>();
        app["config"][format("wordcount_file_%d", file_index)] = wordCountFile;
        app["config"][format("wordcount_buffer_%d", file_index)] = wordCountBuffer;

        seekPos = windowEnd;
        loadedLength = 0;
        resetBuffer();
        cursorPos = 0;
        pageChanged = true;
    }
    else
    {
        size_t stepSize = BUFFER_SIZE / 2;
        size_t remaining = fileSize - windowEnd;
        size_t toLoad = (remaining <= stepSize) ? remaining : stepSize;

        if (!loadWindow(windowEnd, toLoad))
            return;

        cursorPos = 0;
    }

    updateScreen();
}

// Make the current file empty
void Editor::clearFile()
{
    if (savingInProgress)
    {
        _log("Save is is progress. Clear file skipped\n");
        return;
    }

    //
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't clear.\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // Step 1. Check if the backup file exists
    // remove it if already exists
    String backupFileName = format("/%s_backup.txt", fileName.c_str());
    _debug("backupFilename: %s\n", backupFileName);
    if (gfs()->exists(backupFileName.c_str()))
    {
        // remove the backup file
        gfs()->remove(backupFileName.c_str());
    }

    // Step 2. Rename the current file to the backup.txt
    if (gfs()->rename(fileName.c_str(), backupFileName.c_str()))
    {
        _debug("File renamed successfully: %s -> %s.\n", fileName.c_str(), backupFileName.c_str());
    }
    else
    {
        //
        app["error"] = format("Error making a backup file. %s\n", backupFileName);
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // Step 3. Empty the current file by opening it with FILE_WRITE
    File file = gfs()->open(fileName.c_str(), "w");
    if (!file)
    {
        //
        app["error"] = format("Failed to create an empty file %s\n", fileName.c_str());
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // clean up file
    file.close();
    delay(100);

    // Go through the loading process of the empty file
    loadFile(fileName);
}

// House Keeping Tasks
void Editor::loop()
{
    unsigned long now = millis();
    // Auto Repeat is activated
    if (lastKey != 0)
    {
        // check if past point of repeatDelay
        if (now - lastPressTime > repeatDelay)
        {
            // Check if past repeatInterval
            if (now - lastPressTime - repeatDelay >= repeatInterval)
            {
                keyboard(lastKey, true);
                lastPressTime = now - repeatDelay;
            }
        }
    }
}

// Handle Keyboard Input
void Editor::keyboard(int key, bool pressed)
{
    // ignore non printable character
    // (1 is the "consumed" sentinel from the keyboard locale layer,
    // e.g. the Hangul/English toggle key which must type nothing)
    if (key == 0 || key == 1 || key == 27 || key == MENU)
        return;

    //
    _debug("Editor::keyboard:: %c [%d] pressed: %d cursorPos: %d\n", key, key, pressed, cursorPos);

#ifdef DEBUG
    _debug("Buffer: %d\n", getBufferSize());
#endif

    // when any key is pressed track the last key pressed and if they don't release
    // keep issueing press events so that it keeps on typing on the screen
    if (pressed)
    {
        if (key != lastKey)
        {
            // New key or new press: process immediately
            lastKey = key;
            lastPressTime = millis();
            _debug("Auto Repeat %d %d\n", lastKey, lastPressTime);
        }
    }
    else
    {
        _debug("Auto Repeat Release %d\n", lastKey);
        // Key released: reset state
        lastKey = 0;
        lastPressTime = 0;
    }

    // below is only for when the key is pressed
    if (pressed)
    {
        //////////////////////////
        // BACKWARD EDITING
        //////////////////////////
        if (key == '\b')
        {
            // during Hangul composition backspace removes one jamo,
            // not the whole syllable: 간 -> 가 -> ㄱ -> (gone)
            if (composer.isComposing())
            {
                uint32_t display = composer.backspace();
                if (display)
                    replaceCharBeforeCursor(display);
                else
                    removeLastChar();

                // set saved flag to false
                this->saved = false;

                // set flag
                this->backSpacePressed = true;
            }

            // buffer has more than 1 character
            else if (getBufferSize() > 0)
            {
                //
                removeLastChar();

                // set saved flag to false
                this->saved = false;

                // set flag
                this->backSpacePressed = true;
            }
            // buffer emptied
            else
            {
                // Load previous contents from the file if at the beginning of the buffer
                _log("Backspace reached the beginning of the buffer\n");
            }
        }

        // DEL key deletes the word
        else if (key == 127)
        {
            // deleting forward ends any composition
            composer.reset();

            if (getBufferSize() > 0)
            {
                // if editing at the end of the line then remove word
                if (cursorPos == getBufferSize())
                {
                    removeLastWord();
                }

                else
                {
                    // remove word in front
                    removeCharAtCursor();
                }

                // set saved flag to false
                this->saved = false;

                // set flag
                this->backSpacePressed = true;
            }
            // buffer emptied
            else
            {
                // Load previous contents from the file if at the beginning of the buffer
                _log("Delete word reached the beginning of the buffer\n");
            }
        }

        //////////////////////////
        // CURSORS
        //////////////////////////
        else if (key >= 18 && key <= 23 || key == 2 || key == 3)
        {
            // moving the cursor ends any composition - the composed
            // syllable simply stays in the buffer as normal text
            composer.reset();

            // arrow keys
            // 18 - Left, 19 - Right, 20 - Up, 21 - Down
            // 22 - Page Up, 23 - Page Down
            // 2 - Home 3 - End
            if (key == 18)
            {
                if (cursorPos == 0)
                {
                    // already at the start of the buffer - load the previous page
                    pageBackward();
                }
                else
                {
                    // left - move over one whole character
                    cursorPos = prevCharStart(cursorPos);
                }
            }
            else if (key == 19)
            {
                // cursor can't move outside the last text
                if (cursorPos < getBufferSize())
                    // right - move over one whole character
                    cursorPos = nextCharStart(cursorPos);
                else
                    // already at the end of the buffer - load the next page
                    pageForward();
            }

            // UP
            else if (key == 20)
            {
                // move the cursorPos to the start of the previous line
                if (cursorLine == 0)
                {
                    // already at the top line - load the previous page
                    pageBackward();
                }
                else if (cursorLine > 0)
                {
                    // look at the previous line and move to the start of the cursor
                    int newCursorPos =
                        linePositions[cursorLine - 1] - linePositions[0];

                    // if previous line length is shorter than cursorLinePos
                    int previousLineLength = lineLengths[cursorLine - 1];
                    if (previousLineLength < cursorLinePos)
                    {
                        // then move to the end of the previous line
                        newCursorPos += previousLineLength - 1;
                    }
                    else
                    {
                        // if previous line length is long enough
                        // then move as much as the currentLineCursorPos
                        newCursorPos += cursorLinePos;
                    }

                    // edge case
                    if (newCursorPos < 0)
                        newCursorPos = 0;

                    // land on a character boundary - byte-offset math can
                    // fall inside a multi-byte character
                    cursorPos = snapToCharStart(newCursorPos);
                }
            }

            // DOWN
            else if (key == 21)
            {
                // move the cursorPos to the start of the next line
                if (cursorLine < totalLine)
                {
                    //
                    int newCursorPos =
                        linePositions[cursorLine + 1] - linePositions[0];

                    // next line length
                    int nextLineLength = max(lineLengths[cursorLine + 1], 1);

                    // if next line has shorter length of the current cursor pos
                    // then move to the end of the next line
                    if (cursorLinePos > nextLineLength)
                    {
                        newCursorPos += nextLineLength - 1;
                    }
                    else
                    {
                        //
                        newCursorPos += cursorLinePos;
                    }

                    // handle edge case
                    if (newCursorPos > getBufferSize())
                        newCursorPos = getBufferSize();

                    // land on a character boundary
                    cursorPos = snapToCharStart(newCursorPos);
                    cursorLine += 1;
                }

                // when trying to go down at the last line, move the cursor to the end
                else if (cursorLine == totalLine)
                {
                    // already at the end of the buffer - load the next page
                    if (cursorPos == getBufferSize())
                    {
                        pageForward();
                    }
                    else
                    {
                        // if last line then move to the end of the buffer
                        cursorPos = getBufferSize();

                        _debug("Editor::keyboard::DOWN last line condition met cursorPos %d\n",
                               cursorPos);
                    }
                }
            }

            // HOME
            else if (key == 2)
            {
                // home - move to the start of the line
                int newCursorPos =
                    linePositions[cursorLine] - linePositions[0];
                //
                cursorPos = newCursorPos;
            }

            // END
            else if (key == 3)
            {
                // end - move to the end of the line
                int lineLength = max(lineLengths[cursorLine], 1);
                int newCursorPos =
                    linePositions[cursorLine] - linePositions[0] + lineLength - 1;

                // if last line then move to the end of the buffer
                if (cursorLine == totalLine)
                    newCursorPos = getBufferSize();

                // land on a character boundary
                cursorPos = snapToCharStart(newCursorPos);
            }

            // PAGE UP
            else if (key == 22)
            {
                if (cursorLine == 0)
                {
                    // already at the top line - load the previous page
                    pageBackward();
                }
                else
                {
                    int newCursorLine = max(cursorLine - rows, 0);
                    int newCursorPos =
                        linePositions[newCursorLine] - linePositions[0];

                    //
                    cursorPos = newCursorPos;
                }
            }

            // PAGE DOWN
            else if (key == 23)
            {
                if (cursorLine == totalLine && cursorPos == getBufferSize())
                {
                    // already at the end of the buffer - load the next page
                    pageForward();
                }
                else
                {
                    int newCursorLine = min(cursorLine + rows, totalLine);
                    int lineLength = max(lineLengths[newCursorLine], 1);
                    int newCursorPos =
                        linePositions[newCursorLine] - linePositions[0] + lineLength - 1;

                    // if last line then move to the end of the buffer
                    if (cursorLine == totalLine)
                        newCursorPos = getBufferSize();

                    // land on a character boundary
                    cursorPos = snapToCharStart(newCursorPos);
                }
            }
        }

        //////////////////////////
        // FORWARD EDITING
        //////////////////////////
        else
        {
            // add to the edit buffer new character
            // (a character can be up to 4 UTF-8 bytes, so flush a little
            // before the boundary so an insert never overruns it)
            if (getBufferSize() + 4 >= BUFFER_SIZE)
            {
                _log("Text buffer full\n");

                // the window swap invalidates the composing character's
                // position - commit the composition as-is
                composer.reset();

                //
                advanceWindow();
            }

            // Hangul jamo go through the composer, which assembles them
            // into syllables and updates the character before the cursor
            // in place as the syllable evolves (ㄱ -> 가 -> 간)
            if (HangulComposer::isJamo((uint32_t)key))
            {
                HangulComposer::Action action = composer.feed((uint32_t)key);

                if (action.replaceComposing)
                    replaceCharBeforeCursor(action.replaceComposing);

                if (action.insert)
                    addChar((int)action.insert);
            }
            else
            {
                // any other character ends the composition
                composer.reset();

                //
                addChar(key);
            }

            // set saved flag to false
            this->saved = false;
        }

        // update the screen buffer
        updateScreen();
    }
}

//
void Editor::updateScreen()
{
    // Loop through the text buffer
    // and product the data structure that is splitted in each line
    _debug("Editor::updateScreen called\n");

    // Handle empty buffer
    if (buffer[0] == '\0')
    {
        totalLine = 0;
        linePositions[0] = buffer;
        lineLengths[0] = 0;
        cursorLine = 0;
        cursorLinePos = 0;
        cursorLineCols = 0;
        return;
    }

    // first line is the first of the buffer
    linePositions[0] = &buffer[0];
    lineLengths[0] = 0;

    //
    this->totalLine = 0;
    int line_count = 0; // bytes in the current line
    int line_width = 0; // display columns in the current line

    // remember the last space position to use it for the word wrap
    int last_space_index = -1;
    int last_space_position = -1;
    int last_space_width = -1;

    //
    // BUFFER -> SPLIT IN LINES
    //
    // The buffer holds UTF-8. linePositions/lineLengths stay byte-based
    // (renderers print bytes), but the loop advances one whole character
    // at a time and makes wrapping decisions in display columns via
    // charColumns(), so multi-byte and double-width glyphs wrap correctly.
    int i = 0;
    while (i < BUFFER_SIZE)
    {
        // When reaching the end of text, break
        if (buffer[i] == '\0')
        {
            // Update the length of the last line
            lineLengths[totalLine] = line_count;

            //
            break;
        }

        // Advance exactly one character: its bytes and its display width
        int charLen;
        uint32_t codepoint = utf8_decode(&buffer[i], &charLen);
        line_count += charLen;
        line_width += charColumns(codepoint);

        // Track the position of the last space
        if (codepoint == ' ')
        {
            last_space_index = i;
            last_space_position = line_count;
            last_space_width = line_width;
        }

        // Handle words longer than `cols`
        if (line_width >= cols && last_space_index == -1)
        {
            // Register the line count
            lineLengths[totalLine] = line_count;

            // Start a new line
            linePositions[++totalLine] = &buffer[i + charLen];

            // Reset counters
            line_count = 0;
            line_width = 0;
            last_space_index = -1;
            last_space_position = -1;
            last_space_width = -1;

            i += charLen;
            continue;
        }
        // When receiving a newline or max characters reached, start a new line
        if (codepoint == '\n' || line_width >= cols)
        {
            // when ENTER key is found
            if (codepoint == '\n')
            {
                // register the line count
                lineLengths[totalLine] = line_count;

                // start of the new line
                linePositions[++totalLine] = &buffer[i + charLen];

                // reset counters
                line_count = 0;
                line_width = 0;
            }

            // This line requires word-wrap
            else if (last_space_index != -1)
            {
                // register the line position as the last space position
                lineLengths[totalLine] = last_space_position;

                // new line starts from just after the space
                linePositions[++totalLine] = &buffer[last_space_index + 1];

                // new line count starts from the wrapped word
                line_count -= last_space_position;
                line_width -= last_space_width;
            }

            // This line doesn't requrie word wrap
            else
            {
                // register the line count
                lineLengths[totalLine] = line_count;

                //
                linePositions[++totalLine] = &buffer[i + charLen];

                //
                line_count = 0;
                line_width = 0;
            }

            // reset the word wrap flags
            last_space_index = -1;
            last_space_position = -1;
            last_space_width = -1;
        }

        i += charLen;
    }

    // Handle cursor position beyond buffer
    if (cursorPos >= BUFFER_SIZE)
    {
        cursorPos = snapToCharStart(BUFFER_SIZE - 1);
    }

    //
    // CALCULATE CURSOR INFORMATION
    //
    char *pCursorPos = &buffer[cursorPos];

    //
    cursorLine = 0;
    cursorLinePos = lineLengths[0];

    // caculate the which line cursor is located and the line position
    for (int i = totalLine; i >= 0; i--)
    {
        //
        if (pCursorPos >= linePositions[i])
        {
            // found the line index
            cursorLine = i;
            // calculate the cursor position within the line
            cursorLinePos = pCursorPos - linePositions[i];
            break;
        }
    }

    // cursor offset within the line in display columns - what the display
    // multiplies by the glyph width to place the cursor. Counted per
    // character, not per byte, so multi-byte glyphs occupy their true width.
    cursorLineCols = 0;
    for (char *p = linePositions[cursorLine]; p < pCursorPos;)
    {
        int charLen;
        uint32_t codepoint = utf8_decode(p, &charLen);
        cursorLineCols += charColumns(codepoint);
        p += charLen;
    }

    //
    _debug("Editor::updateScreen cursorPos: %d\n", cursorPos);
}

void Editor::addChar(int c)
{
    // encode the codepoint as UTF-8 (1 byte for ASCII, 2 for Latin-1
    // accents, 3 for a Korean syllable)
    char encoded[4];
    int len = utf8_encode((uint32_t)c, encoded);

    int bufferSize = getBufferSize();

    // the buffer has slack beyond BUFFER_SIZE so a multi-byte character
    // arriving right at the boundary is never dropped - keyboard() flushes
    // the window before the slack could ever be exceeded
    if (bufferSize + len < (int)sizeof(buffer) - 1)
    {
        // shift the trailing texts
        if (bufferSize > cursorPos)
            memmove(buffer + cursorPos + len, buffer + cursorPos, bufferSize - cursorPos);

        //
        memcpy(buffer + cursorPos, encoded, len);
        cursorPos += len;
        buffer[bufferSize + len] = '\0';

        _debug("FileBuffer::addChar::cursorPos %d %d (%d bytes)\n", cursorPos, c, len);
    }
}

void Editor::removeLastChar()
{
    int bufferSize = getBufferSize();

    //
    _debug("FileBuffer::removeLastChar cusorPos: %d bufferSize: %d\n", cursorPos, bufferSize);

    if (bufferSize > 0 && cursorPos > 0)
    {
        // delete the whole character before the cursor
        int start = prevCharStart(cursorPos);
        int len = cursorPos - start;

        // Shift the trailing texts left
        if (bufferSize > cursorPos)
        {
            memmove(buffer + start, buffer + cursorPos, bufferSize - cursorPos);
        }

        // Null terminate the buffer
        buffer[bufferSize - len] = 0;
        cursorPos = start;

        bufferSize = getBufferSize();
        _debug("FileBuffer::removeLastChar After cusorPos: %d bufferSize: %d\n", cursorPos, bufferSize);
    }
}

void Editor::removeCharAtCursor()
{
    int bufferSize = getBufferSize();
    if (bufferSize > 0 && cursorPos < bufferSize)
    {
        // delete the whole character at the cursor
        int end = nextCharStart(cursorPos);
        int len = end - cursorPos;

        // Shift the trailing text left
        if (bufferSize > end)
        {
            memmove(buffer + cursorPos, buffer + end, bufferSize - end);
        }

        // Null terminate the buffer
        buffer[bufferSize - len] = '\0';
    }
}

// Replace the character before the cursor (the composing Hangul syllable)
// with another one, e.g. 가 -> 간. Sets charReplaced so displays repaint
// the line - the new glyph doesn't necessarily cover the old one's pixels.
void Editor::replaceCharBeforeCursor(uint32_t codepoint)
{
    removeLastChar();
    addChar((int)codepoint);

    //
    charReplaced = true;
}

void Editor::removeLastWord()
{
    int length = getBufferSize();
    if (length == 0)
        return;

    int end = length - 1;
    while (end >= 0 && buffer[end] == ' ')
        end--;

    if (end < 0)
        return;

    int start = end;
    while (start >= 0 && buffer[start] != ' ' && buffer[start] != '\n')
        start--;

    if (start <= 0)
    {
        start = 0;
        buffer[0] = '\0';
    }
    else
    {
        buffer[start] = ' ';
        buffer[start + 1] = '\0';
    }

    cursorPos = getBufferSize();

    //
    _debug("FileBuffer::removeLastWord %d\n", cursorPos);
}
