#include "locale.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

#include <unordered_map>
#include <functional>
#include <utility>
#include <string>

// locale
#include "be/be.h"
#include "ca/ca.h"
#include "dv/dv.h"
#include "finnish/finnish.h"
#include "norweigian/norweigian.h"
#include "fr/fr.h"
#include "ge/ge.h"
#include "it/it.h"
#include "latin/latin.h"
#include "swedish/swedish.h"
#include "uk/uk.h"
#include "us/us.h"

// Custom hash function for Arduino String
struct StringHash
{
  std::size_t operator()(const String &s) const
  {
    return std::hash<std::string>()(s.c_str()); // Use std::string's hash
  }
};

// Define a type for the function pointers
using KeycodeFunction = std::function<uint8_t(uint8_t, bool, bool)>;

uint8_t keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt)
{
  // Use the custom hash function for String
  static const std::unordered_map<String, KeycodeFunction, StringHash> locale_map = {
      {"INT", [](uint8_t keycode, bool shift, bool alt)
       { return keyboard_precursor_filter(keyboard_keycode_ascii_us(keycode, shift)); }},
      {"BE", keyboard_keycode_ascii_be},
      {"CA", keyboard_keycode_ascii_ca},
      {"DV", keyboard_keycode_ascii_dv},
      {"FN", keyboard_keycode_ascii_finnish},
      {"SWE", keyboard_keycode_ascii_swedish},
      {"NO", keyboard_keycode_ascii_norweigian},
      {"FR", keyboard_keycode_ascii_fr},
      {"GE", keyboard_keycode_ascii_ge},
      {"IT", keyboard_keycode_ascii_it},
      {"LAT", keyboard_keycode_ascii_latin},
      {"UK", keyboard_keycode_ascii_uk},
  };

  auto it = locale_map.find(locale);
  if (it != locale_map.end())
  {
    return it->second(keycode, shift, alt);
  }

  // Default to US layout
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
      display_keyboard(pre_cursor_ascii, true);
      display_keyboard(pre_cursor_ascii, false);

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
      display_keyboard(pre_cursor_ascii, true);
      display_keyboard(pre_cursor_ascii, false);

      // then clear the precursor and send out the key stroke
      pre_cursor_ascii = 0;

      // if space is pressed then don't print
      if (ascii == ' ')
        return 0;
    }
  }

  return ascii;
}

// Custom hash function for std::pair to use it as a key in std::unordered_map
struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const
  {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

uint8_t keyboard_international(uint8_t precursor, uint8_t ascii)
{
  static const std::unordered_map<std::pair<uint8_t, uint8_t>, uint8_t, pair_hash> ascii_map = {
      // ~ a n o
      {{'~', 'a'}, 227},
      {{'~', 'A'}, 195},
      {{'~', 'n'}, 241},
      {{'~', 'N'}, 209},
      {{'~', 'o'}, 245},
      {{'~', 'O'}, 213},

      // ` a e i o u
      {{'`', 'a'}, 224},
      {{'`', 'A'}, 192},
      {{'`', 'e'}, 232},
      {{'`', 'E'}, 200},
      {{'`', 'i'}, 236},
      {{'`', 'I'}, 204},
      {{'`', 'o'}, 242},
      {{'`', 'O'}, 210},
      {{'`', 'u'}, 249},
      {{'`', 'U'}, 217},

      // ' a e i o u
      {{'\'', 'a'}, 225},
      {{'\'', 'A'}, 193},
      {{'\'', 'e'}, 233},
      {{'\'', 'E'}, 201},
      {{'\'', 'i'}, 237},
      {{'\'', 'I'}, 205},
      {{'\'', 'o'}, 243},
      {{'\'', 'O'}, 211},
      {{'\'', 'u'}, 250},
      {{'\'', 'U'}, 218},

      // " a e i o u y
      {{'\"', 'a'}, 228},
      {{'\"', 'A'}, 196},
      {{'\"', 'e'}, 235},
      {{'\"', 'E'}, 203},
      {{'\"', 'i'}, 239},
      {{'\"', 'I'}, 207},
      {{'\"', 'o'}, 246},
      {{'\"', 'O'}, 214},
      {{'\"', 'u'}, 252},
      {{'\"', 'U'}, 220},
      {{'\"', 'y'}, 255},

      // ¨ (168) a e i o u y
      {{168, 'a'}, 228},
      {{168, 'A'}, 196},
      {{168, 'e'}, 235},
      {{168, 'E'}, 203},
      {{168, 'i'}, 239},
      {{168, 'I'}, 207},
      {{168, 'o'}, 246},
      {{168, 'O'}, 214},
      {{168, 'u'}, 252},
      {{168, 'U'}, 220},
      {{168, 'y'}, 255},

      // ^ a e i o u
      {{'^', 'a'}, 226},
      {{'^', 'A'}, 194},
      {{'^', 'e'}, 234},
      {{'^', 'E'}, 202},
      {{'^', 'i'}, 238},
      {{'^', 'I'}, 206},
      {{'^', 'o'}, 244},
      {{'^', 'O'}, 212},
      {{'^', 'u'}, 251},
      {{'^', 'U'}, 219}};

  auto it = ascii_map.find({precursor, ascii});
  if (it != ascii_map.end())
  {
    return it->second;
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

