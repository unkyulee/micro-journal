#include "Keypad.h"
#include "app/app.h"
#include "display/display.h"

void keypad_load_config(String filename, int *layers, int size, const char **keys, int num_layers)
{
    //
    JsonDocument &app = status();

    // check if file exists in SD card
    if (gfs()->exists(filename.c_str()))
    {
        _log("Loading %s from file system\n", filename.c_str());
        // load image
        File file = gfs()->open(filename.c_str(), "r");
        if (!file)
        {
            _log("Failed to open %s for reading\n", filename.c_str());
            return;
        }

        //
        String fileString = file.readString();
        file.close();

        //
        _debug(fileString.c_str());
        _debug("\n");

        //
        // Prepare a JsonDocument for the keyboard configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument keyboardConfig;
        // convert to JsonObject
        DeserializationError error = deserializeJson(keyboardConfig, fileString);
        if (error)
        {
            //
            app["error"] = format("%s not in a correct json form: %s\n", filename.c_str(), error.c_str());
            // app["screen"] = ERRORSCREEN;

            //
            _log(app["error"]);

            return;
        }

        // overwrite to layers
        // int layers[LAYERS][ROWS * COLS]

        // save the current key pos
        int pos = 0;
        for (int i = 0; i < num_layers; i++)
        {
            const char *key = keys[i];
            JsonArray keyArray = keyboardConfig[key].as<JsonArray>();

            int pos = 0;
            for (JsonVariant obj : keyArray)
            {
                // check if it has extended ascii
                if (obj.is<int>())
                {
                    *(layers + i * size + pos++) = obj.as<int>();
                }
                else
                {
                    //
                    String key = obj.as<String>();
                    *(layers + i * size + pos++) = keypad_convert_wp_keys(key);
                }
            }
        }

        // Close the file
        file.close();
    }
}

//
int keypad_convert_wp_keys(String key)
{
    if (key.length() != 1)
    {
        // If the input is not a single character, return 0 as an error value
        if (key == "MENU")
            return MENU;

        else if (key == "ESC")
            return 27;

        else if (key == "BACKSPACE")
            return 8;

        else if (key == "DEL")
            return 127;

        else if (key == "SHIFT")
            return 14;

        else if (key == "ALT")
            return 17;

        else if (key == "LOWER")
            return 17;

        else if (key == "RAISE")
            return 24;

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

        else if (key == "FILE0")
            return 1000;

        else if (key == "FILE1")
            return 1001;

        else if (key == "FILE2")
            return 1002;

        else if (key == "FILE3")
            return 1003;

        else if (key == "FILE4")
            return 1004;

        else if (key == "FILE5")
            return 1005;

        else if (key == "FILE6")
            return 1006;

        else if (key == "FILE7")
            return 1007;

        else if (key == "FILE8")
            return 1008;

        else if (key == "FILE9")
            return 1009;

        return 0;
    }

    //
    return key[0];
}
