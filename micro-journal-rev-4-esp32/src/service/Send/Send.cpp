#include "Send.h"
#include "app/app.h"

//
#include "service/Editor/Editor.h"

//
SendFunc send_func = nullptr;;
void send_register(SendFunc f) {
    send_func = f;
}

// Monitor if there are any send request
void send_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        last = millis();

        //
        JsonDocument &app = status();
        String task = app["task"].as<String>();

        if (task == "send_start")
        {
            //
            _log("[send_loop] Send Start Request\n");

            //
            app["task"] = "";
            app["send_finished"] = false;
            app["send_stop"] = false;

            //
            send_start();
        }
    }
}

//
void send_start()
{
    //
    JsonDocument &app = status();

    // Loading File
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    // Get File reference
    File file = gfs()->open(Editor::getInstance().fileName.c_str(), "r");
    if (!file || !file.available())
    {
        _log("Send_render: Failed to open file: %s\n", Editor::getInstance().fileName.c_str());
        return;
    }

    //
    _log("Sending file content over keyboard...\n");

    // Read file content in small chunks
    while (file.available() && app["send_stop"].as<bool>() == false)
    {
        //
        char c = file.read();

        // use the send function pointer to send
        if (send_func != nullptr)
            send_func(c);

        // Small delay to prevent buffer overflow
        delay(5); // You can tweak this as needed
    }
    //
    file.close();
    delay(100);

    //
    app["send_finished"] = true;
}