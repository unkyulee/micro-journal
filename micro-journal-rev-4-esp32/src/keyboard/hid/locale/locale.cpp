#include "locale.h"
#include "app/app.h"

// locale
#include "us/us.h"

uint8_t keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt, bool capslock)
{
    return keyboard_keycode_ascii_us(keycode, shift);
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

      //
      // when precursor keys are typed
      if (released == false)
      {
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
      }

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

    //
    // US Layout
    //
    else
    {
      // US
      return keyboard_conv_table_us[keycode][shift];
    }

    // do nothing
    return 0;
  }


  */