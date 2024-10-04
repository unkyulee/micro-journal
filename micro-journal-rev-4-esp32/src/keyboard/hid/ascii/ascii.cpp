#include "ascii.h"

typedef struct
{
  uint8_t precursor;
  uint8_t ascii;
  uint8_t code;
} AsciiMapping;

static const AsciiMapping ascii_map[] = {
    // ~  a n o
    {'~', 'a', 227},
    {'~', 'A', 195},
    {'~', 'n', 241},
    {'~', 'N', 209},
    {'~', 'o', 245},
    {'~', 'O', 213},
    // ` a e i o u
    {'`', 'a', 224},
    {'`', 'A', 192},
    {'`', 'e', 232},
    {'`', 'E', 200},
    {'`', 'i', 236},
    {'`', 'I', 204},
    {'`', 'o', 242},
    {'`', 'O', 210},
    {'`', 'u', 249},
    {'`', 'U', 217},
    // ' a e i o u
    {'\'', 'a', 225},
    {'\'', 'A', 193},
    {'\'', 'e', 233},
    {'\'', 'E', 201},
    {'\'', 'i', 237},
    {'\'', 'I', 205},
    {'\'', 'o', 243},
    {'\'', 'O', 211},
    {'\'', 'u', 250},
    {'\'', 'U', 218},
    // " a e i o u y
    {'\"', 'a', 228},
    {'\"', 'A', 196},
    {'\"', 'e', 235},
    {'\"', 'E', 203},
    {'\"', 'i', 239},
    {'\"', 'I', 207},
    {'\"', 'o', 246},
    {'\"', 'O', 214},
    {'\"', 'u', 252},
    {'\"', 'U', 220},
    {'\"', 'y', 255},
    // ^ a e i o u
    {'^', 'a', 226},
    {'^', 'A', 194},
    {'^', 'e', 234},
    {'^', 'E', 202},
    {'^', 'i', 238},
    {'^', 'I', 206},
    {'^', 'o', 244},
    {'^', 'O', 212},
    {'^', 'u', 251},
    {'^', 'U', 219}};

uint8_t ascii_international(uint8_t precursor, uint8_t ascii)
{
  for (size_t i = 0; i < sizeof(ascii_map) / sizeof(AsciiMapping); ++i)
  {
    if (ascii_map[i].precursor == precursor && ascii_map[i].ascii == ascii)
    {
      return ascii_map[i].code;
    }
  }
  return 0; // Default return value if no match is found
}

uint8_t unicode_convert_ascii(String key)
{
  if (key.length() != 1)
  {
    // If the input is not a single character, return 0 as an error value
    if (key == "ESC")
    {
      return 27;
    }
    else if (key == "BACKSPACE")
    {
      return 8;
    }
    else if (key == "SHIFT")
    {
      return 14;
    }
    else if (key == "ALT")
    {
      return 17;
    }

    else if (key == "LEFT")
    {
      return 18;
    }
    else if (key == "RIGHT")
    {
      return 19;
    }
    else if (key == "UP")
    {
      return 20;
    }
    else if (key == "DOWN")
    {
      return 21;
    }
    else if (key == "PGUP")
    {
      return 22;
    }
    else if (key == "PGDN")
    {
      return 23;
    }
    else if (key == "HOME")
    {
      return 2;
    }
    else if (key == "END")
    {
      return 3;
    }
    return 0;
  }

  //
  return key[0];
}

