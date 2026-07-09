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

#endif
