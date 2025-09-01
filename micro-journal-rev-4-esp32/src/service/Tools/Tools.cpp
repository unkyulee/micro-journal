#include "Tools.h"
#include "app/app.h"

//
//
#include <SD.h>

#ifdef BOARD_ESP32_S3
#include <SPIFFS.h>
#endif

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
            _log(buffer);
            file_size = -1;
        }
        else 
        {   //file exists
            file_size = file.size();
        }
        //
        file.close();
    }
    return file_size;
}


// Create an array of String objects
// Many of these ascii codes can be tracked back to:
// https://en.wikipedia.org/wiki/ISO/IEC_8859 column 1 of the table
static const String extended_ascii[128] = {
    "E", // 128 euro sign doesn't work on Micro Journal
    "",
    "", // 130
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 140
    "",
    "Ž", // 142
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 150
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    " ", // 160
    "¡",
    "¢",
    "£",
    "¤",
    "¥",
    "¦",
    "§",
    "¨",
    "©",
    "ª", // 170 -
    "«",
    "¬",
    "­",
    "®",
    "¯",
    "°",
    "±",
    "²",
    "³",
    "", // 180
    "µ",
    "¶",
    "·",
    "¸",
    "¹",
    "º",
    "»",
    "¼",
    "½",
    "¾", // 190
    "¿",
    "À",
    "Á",
    "Â",
    "Ã",
    "Ä",
    "Å",
    "Æ",
    "Ç",
    "È", // 200
    "É",
    "Ê",
    "Ë",
    "Ì",
    "Í",
    "Î",
    "Ï",
    "Ð",
    "Ñ",
    "Ò", // 210
    "Ó",
    "Ô",
    "Õ",
    "Ö",
    "×",
    "Ø",
    "Ù",
    "Ú",
    "Û",
    "Ü", // 220
    "Ý",
    "Þ",
    "ß",
    "à",
    "á",
    "â",
    "ã",
    "ä",
    "å",
    "æ", // 230
    "ç",
    "è",
    "é",
    "ê",
    "ë",
    "ì",
    "í",
    "î",
    "ï",
    "ð", // 240
    "ñ",
    "ò",
    "ó",
    "ô",
    "õ",
    "ö",
    "÷",
    "ø",
    "ù",
    "ú", // 250
    "û",
    "ü",
    "ý",
    "þ",
    "ÿ", // 255
};

String asciiToUnicode(uint8_t value)
{
  if (value < 128)
    return "";

  uint8_t code = value - 128;
  return extended_ascii[code];
}



String format(const char *format, ...)
{
    char buffer[256]; // Adjust the size according to your needs
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return String(buffer);
}