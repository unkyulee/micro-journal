#include "locale.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

// locale
#include "us/us.h"
#include "be/be.h"
#include "ca/ca.h"
#include "dv/dv.h"
#include "it/it.h"
#include "uk/uk.h"
#include "ge/ge.h"

uint8_t keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt)
{
  if (locale == "BE")
  {
    // Belgium Layout
    return keyboard_keycode_ascii_be(keycode, shift, alt);
  }
  else if (locale == "GE")
  {
    // Canadian Layout
    return keyboard_keycode_ascii_ge(keycode, shift, alt);
  }
  else if (locale == "CA")
  {
    // Canadian Layout
    return keyboard_keycode_ascii_ca(keycode, shift, alt);
  }
  else if (locale == "INT")
  {
    // US International
    return keyboard_precursor_filter(keyboard_keycode_ascii_us(keycode, shift));
  }
  else if (locale == "IT")
  {
    // Italian
    return keyboard_keycode_ascii_it(keycode, shift, alt);
  }
  else if (locale == "UK")
  {
    // Italian
    return keyboard_keycode_ascii_uk(keycode, shift, alt);
  }
  else if (locale == "DV")
  {
    return keyboard_keycode_ascii_dv(keycode, shift, alt);
  }
  // by default return US keyboard layout
  return keyboard_keycode_ascii_us(keycode, shift);
}

uint8_t keyboard_precursor_filter(uint8_t ascii)
{
  static uint8_t pre_cursor_ascii = 0;

  // when precursor keys are typed
  if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == 168 || ascii == '\'' || ascii == '^')
  {
    // check if previous precursor is set
    if (pre_cursor_ascii != 0)
    {
      // when precursor keys is registered twice
      // send out the pre registered precursor key
      display_keyboard(pre_cursor_ascii);

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

      //
      return found;
    }
    else
    {
      //
      // precursor invalid just send out the key press
      //
      display_keyboard(pre_cursor_ascii);

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
    // 
    {168, 'a', 228},
    {168, 'A', 196},
    {168, 'e', 235},
    {168, 'E', 203},
    {168, 'i', 239},
    {168, 'I', 207},
    {168, 'o', 246},
    {168, 'O', 214},
    {168, 'u', 252},
    {168, 'U', 220},
    {168, 'y', 255},
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

uint8_t keyboard_caplock_filter(uint8_t ascii)
{
  // upgrade to large character when capslock is present
  // only enlarge when it is alphabet
  // check capslock
  if (keyboard_capslock())
  {
    // check if the ascii is between 97 ~ 122
    // 224 ~
    if (
        (ascii >= 97 && ascii <= 122) || (ascii >= 224 && ascii <= 254))
    {
      ascii = ascii - 32;      
    }
    else if (ascii == 255)
    {
      ascii = 159;
    }
  }

  return ascii;
}