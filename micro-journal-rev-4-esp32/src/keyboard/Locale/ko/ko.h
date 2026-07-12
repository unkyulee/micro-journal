#ifndef keyboard_locale_KO_h
#define keyboard_locale_KO_h

#include <Arduino.h>

// Korean 두벌식 (dubeolsik) layout.
//
// Letter keys return Hangul compatibility jamo codepoints (0x3131-0x3163);
// the Editor's HangulComposer assembles them into syllables. All other
// keys pass through as US. Alt toggles between Hangul and English input.
//
// int return: jamo codepoints don't fit in a byte.
int keyboard_keycode_ascii_ko(uint8_t keycode, bool shift, bool alt, bool pressed);

// Character-based variant for keypads whose layer tables (possibly
// customized via keyboard.json) already resolved the physical key to a
// US character: maps 'r' -> ㄱ, 'R' -> ㄲ, 'c' -> ㅊ ... by the character
// itself, independent of which physical key produced it. Respects the
// same Hangul/English mode as the keycode-based entry point.
int keyboard_ascii_ko(int ascii, bool pressed);

#endif
