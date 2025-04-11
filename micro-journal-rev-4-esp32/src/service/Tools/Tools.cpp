#include "Tools.h"
#include "app/app.h"

//
//
#include <SD.h>
#include <SPIFFS.h>

String formatNumber(int num)
{
    String formattedNumber = "";
    int digitCount = 0;
    if (num < 0)
    {
        formattedNumber += "-";
        num = -num;
    }
    do
    {
        if (digitCount > 0 && digitCount % 3 == 0)
        {
            formattedNumber = "," + formattedNumber;
        }
        formattedNumber = String(num % 10) + formattedNumber;
        num /= 10;
        digitCount++;
    } while (num > 0);

    return formattedNumber;
}


// Get the size of a file in bytes
size_t fileSize(String fileName)
{
    size_t file_size = 0;
    if (SD.exists(fileName))
    {
        File file = SD.open(fileName, FILE_READ);
        if (!file)
        {   //something bad happened
            char buffer [32];
            sprintf(buffer, "Failed to open a file. %s\n", fileName);
            app_log(buffer);
            file_size = -1;
        }
        else 
        {   //file exists
            file_size = file.size();
        }
        //
        file.close();
        delay(10);
    }
    return file_size;
}