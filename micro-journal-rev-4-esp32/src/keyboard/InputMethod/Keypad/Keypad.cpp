#include "Keypad.h"
#include "app/app.h"
#include "display/display.h"

//
bool load_keymap(JsonArray keyArray, int l, int *layers, int num_cols)
{
    if (keyArray.size() == 48)
    {
        // only when 48 elements are presented
        int pos = 0;
        for (JsonVariant obj : keyArray)
        {
            // check if it has extended ascii
            if (obj.is<int>())
            {
                *(layers + l * num_cols + pos++) = obj.as<int>();
            }
            else
            {
                //
                String key = obj.as<String>();
                *(layers + l * num_cols + pos++) = keypad_convert_wp_keys(key);
            }
        }
        return true;
    }

    return false;
}

void keypad_load_config(String filename, int *layers, int num_cols)
{
    //
    JsonDocument &app = status();

    // check if file exists in SD card
    if (gfs()->exists(filename.c_str()))
    {
        _log("Loading keyboard.json from file system\n");
        // load image
        File file = gfs()->open(filename.c_str(), "r");
        if (!file)
        {
            _log("Failed to open file for reading\n");
            return;
        }

        //
        String fileString = file.readString();        
        file.close();
        _debug(fileString.c_str());

        //
        // Prepare a JsonDocument for the keyboard configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument keyboardConfig;
        // convert to JsonObject
        DeserializationError error = deserializeJson(keyboardConfig, fileString);
        if (error)
        {
            //
            app["error"] = format("keyboard.json not in a correct json form: %s\n", error.c_str());
            app["screen"] = ERRORSCREEN;

            //
            _log(app["error"]);

            return;
        }

        // overwrite to layers
        // int layers[LAYERS][ROWS * COLS]
        const char *keys[] = {"main", "main-shift", "alt", "alt-shift"};
        int keymapIndices[] = {0, 1, 2, 3};

        for (int i = 0; i < 4; i++)
        {
            const char *key = keys[i];
            int index = keymapIndices[i];

            if (keyboardConfig[key].is<JsonArray>())
            {
                if (load_keymap(keyboardConfig[key].as<JsonArray>(), index, layers, num_cols))
                {
                    _log("%s loaded\n", key);
                }
                else
                {
                    //
                    app["error"] = format("%s keyboard layout load failed\n", key);
                    app["screen"] = ERRORSCREEN;

                    //
                    _log(app["error"]);

                    return;
                }
            }
        }

        // Close the file
        file.close();

        return;
    }
}

//
uint8_t keypad_convert_wp_keys(String key)
{
    if (key.length() != 1)
    {
        // If the input is not a single character, return 0 as an error value
        if (key == "ESC")
            return 27;

        else if (key == "BACKSPACE")
            return 8;

        else if (key == "SHIFT")
            return 14;

        else if (key == "ALT")
            return 17;

        else if (key == "LEFT")
            return 18;

        else if (key == "RIGHT")
            return 19;

        else if (key == "UP")
            return 20;

        else if (key == "DOWN")
            return 21;

        else if (key == "PGUP")
            return 22;

        else if (key == "PGDN")
            return 23;

        else if (key == "HOME")
            return 2;

        else if (key == "END")
            return 3;

        return 0;
    }

    //
    return key[0];
}
