#include "locale.h"
#include "app/app.h"
#include "keyboard/keyboard.h"

// locale
#include "us/us.h"
#include "be/be.h"

uint8_t keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt)
{
    if (locale == "BE")
    {
        // Belgium Layout
        return keyboard_keycode_ascii_be(keycode, shift, alt);
    }
    // by default return US keyboard layout
    return keyboard_keycode_ascii_us(keycode, shift);
}

uint8_t keyboard_precursor_filter(uint8_t ascii)
{
    static uint8_t pre_cursor_ascii = 0;

    // when precursor keys are typed
    if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == '\'' || ascii == '^')
    {
        // check if previous precursor is set
        if (pre_cursor_ascii != 0)
        {
            // when precursor keys is registered twice
            // send out the pre registered precursor key
            keyboard_key(pre_cursor_ascii);

            // then clear the precursor and send out the key stroke
            pre_cursor_ascii = 0;

            // then return the current key
            return ascii;
        }
        else
        {
            // save to the precursor
            pre_cursor_ascii = ascii;
            // do not process this key press
            return 0;
        }
    }

    // when precursor exists
    if (pre_cursor_ascii != 0 && ascii != 0)
    {
        uint8_t found = keyboard_international(pre_cursor_ascii, ascii);

        // type latin character
        if (found > 0)
        {
            // reset precursor
            pre_cursor_ascii = 0;

            // send out the combined key
            keyboard_key(found);

            //
            return 0;
        }
        else
        {
            //
            // precursor invalid just send out the key press
            //
            keyboard_key(pre_cursor_ascii);

            // then clear the precursor and send out the key stroke
            pre_cursor_ascii = 0;

            // if space is pressed then don't print
            if (ascii == ' ')
                return 0;          
        }
    }

    return ascii;
}

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

uint8_t keyboard_international(uint8_t precursor, uint8_t ascii)
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

/*
//
  uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr, bool released)
  {
    // left - 80 right - 79 up - 82 down - 81 home - 74 end - 77

    //
    static uint8_t const keyboard_conv_table_us[128][2] = {HID_KEYCODE_TO_ASCII_US};
    static uint8_t const keyboard_conv_table_it[128][3] = {HID_KEYCODE_TO_ASCII_IT};
    static uint8_t const keyboard_conv_table_ca_bi[128][5] = {HID_KEYCODE_TO_ASCII_CA_BI};
    static uint8_t const keyboard_conv_table_ge[128][3] = {HID_KEYCODE_TO_ASCII_GE};
    static uint8_t const keyboard_conv_table_be[128][3] = {HID_KEYCODE_TO_ASCII_BE};
    static uint8_t const keyboard_conv_table_uk[128][2] = {HID_KEYCODE_TO_ASCII_UK};

    //
    static uint8_t pre_cursor_ascii = 0;

    // debug
    // app_log("%d %d %d\n", keycode, shift, altgr);

    //
    if (shift > 1)
      shift = 1;

    //
    // Italian Keyboard
    //
    if (hidLocal == HID_LOCAL_Italian)
    {
      if (altgr > 0)
        shift += 2;

      // Italian
      return keyboard_conv_table_it[keycode][shift];
    }

    //
    // Belgium
    //
    else if (hidLocal == HID_LOCAL_Belgian)
    {
      if (altgr > 0)
        shift += 2;

      // international layout
      uint8_t ascii = keyboard_conv_table_be[keycode][shift];



      return keyboard_conv_table_be[keycode][shift];
    }

    //
    // UK
    //
    else if (hidLocal == HID_LOCAL_UK)
    {
      return keyboard_conv_table_uk[keycode][shift];
    }

    //
    // Canadian Bilingual
    //
    else if (hidLocal == HID_LOCAL_Canadian_Bilingual)
    {
      if (altgr > 0)
        shift += 2;

      // Canadian Bilingual
      return keyboard_conv_table_ca_bi[keycode][shift];
    }

    //
    // HID_LOCAL_German
    //
    else if (hidLocal == HID_LOCAL_German)
    {
      // Serial.println(keycode, HEX);

      if (altgr > 0)
        shift += 2;

      // German
      return keyboard_conv_table_ge[keycode][shift];
    }

    //
    // International
    //
    else if (hidLocal == HID_LOCAL_International)
    {
      // international layout
      uint8_t ascii = keyboard_conv_table_us[keycode][shift];

      //
      // when precursor keys are typed
      if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == '\'' || ascii == '^')
      {
        // check if previous precursor is set
        if (pre_cursor_ascii != 0)
        {
          //
          onKeyboardKey(pre_cursor_ascii, 0, 0);

          // then clear the precursor and send out the key stroke
          pre_cursor_ascii = 0;
        }
        else
        {
          // save to the precursor
          pre_cursor_ascii = ascii;
          // do not process this key press
          return 0;
        }
      }

      // when precursor exists
      if (pre_cursor_ascii != 0 && ascii != 0)
      {
        uint8_t found = ascii_international(pre_cursor_ascii, ascii);

        // type latin character
        if (found > 0)
        {
          // reset precursor
          pre_cursor_ascii = 0;

          // á
          onKeyboardKey(found, 0, 0);

          //
          return 0;
        }
        else
        {
          //
          // precursor invalid just send out the key press
          //
          onKeyboardKey(pre_cursor_ascii, 0, 0);

          // then clear the precursor and send out the key stroke
          pre_cursor_ascii = 0;

          // if space is pressed then don't print
          if (ascii == ' ')
            return 0;
        }
      }

      // rest of the keys uses US keyboard
      return keyboard_conv_table_us[keycode][shift];
    }



  */