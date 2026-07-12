#include "ko.h"
#include "../us/us.h"
#include "app/app.h"
#include "display/display.h"

//
// Korean 두벌식 (dubeolsik) layout
//

// jamo for the letter keys a-z (0 = no mapping)
// q=ㅂ w=ㅈ e=ㄷ r=ㄱ t=ㅅ y=ㅛ u=ㅕ i=ㅑ o=ㅐ p=ㅔ
// a=ㅁ s=ㄴ d=ㅇ f=ㄹ g=ㅎ h=ㅗ j=ㅓ k=ㅏ l=ㅣ
// z=ㅋ x=ㅌ c=ㅊ v=ㅍ b=ㅠ n=ㅜ m=ㅡ
static const uint16_t dubeolsik[26] = {
    0x3141, // a ㅁ
    0x3160, // b ㅠ
    0x314A, // c ㅊ
    0x3147, // d ㅇ
    0x3137, // e ㄷ
    0x3139, // f ㄹ
    0x314E, // g ㅎ
    0x3157, // h ㅗ
    0x3151, // i ㅑ
    0x3153, // j ㅓ
    0x314F, // k ㅏ
    0x3163, // l ㅣ
    0x3161, // m ㅡ
    0x315C, // n ㅜ
    0x3150, // o ㅐ
    0x3154, // p ㅔ
    0x3142, // q ㅂ
    0x3131, // r ㄱ
    0x3134, // s ㄴ
    0x3145, // t ㅅ
    0x3155, // u ㅕ
    0x314D, // v ㅍ
    0x3148, // w ㅈ
    0x314C, // x ㅌ
    0x315B, // y ㅛ
    0x314B, // z ㅋ
};

// shifted variants: Q=ㅃ W=ㅉ E=ㄸ R=ㄲ T=ㅆ O=ㅒ P=ㅖ
// (all other letters keep their unshifted jamo)
static uint16_t dubeolsik_shifted(char letter, uint16_t jamo)
{
    switch (letter)
    {
    case 'q':
        return 0x3143; // ㅃ
    case 'w':
        return 0x3149; // ㅉ
    case 'e':
        return 0x3138; // ㄸ
    case 'r':
        return 0x3132; // ㄲ
    case 't':
        return 0x3146; // ㅆ
    case 'o':
        return 0x3152; // ㅒ
    case 'p':
        return 0x3156; // ㅖ
    default:
        return jamo;
    }
}

// Returned instead of 0 when a key is handled but must type nothing (the
// Hangul/English toggle). 0 means "no mapping" and would make the keypad
// driver fall back to its hardcoded US table, typing the key anyway;
// value 1 travels the whole pipeline and is discarded by Editor::keyboard.
#define KEY_CONSUMED 1

// current input mode, shared by both entry points below
static bool hangul = true;

// Map one already-resolved US-layout character to its dubeolsik jamo:
// 'r' -> ㄱ, 'R' -> ㄲ, 'c' -> ㅊ ... The case of the letter selects the
// shifted variant, so this works no matter which physical key produced
// the character - keypads with custom keyboard.json layouts included.
// Non-letter characters (and English mode) pass through unchanged.
int keyboard_ascii_ko(int ascii, bool pressed)
{
    // outside the word processor (menus, dialogs) keys must stay Latin
    // so the navigation shortcuts keep working
    JsonDocument &app = status();
    if (app["screen"].as<int>() != WORDPROCESSOR)
        return ascii;

    if (!hangul)
        return ascii;

    // letter keys become jamo
    char letter = 0;
    if (ascii >= 'a' && ascii <= 'z')
        letter = ascii;
    else if (ascii >= 'A' && ascii <= 'Z')
        letter = ascii - 'A' + 'a';

    if (letter == 0)
        return ascii;

    uint16_t jamo = dubeolsik[letter - 'a'];
    if (ascii >= 'A' && ascii <= 'Z')
        jamo = dubeolsik_shifted(letter, jamo);

    return jamo;
}

int keyboard_keycode_ascii_ko(uint8_t keycode, bool shift, bool alt, bool pressed)
{
    // start from the US mapping; everything that is not a letter key
    // (digits, punctuation, space, enter, backspace, arrows ...) passes
    // through unchanged
    int ascii = keyboard_keycode_ascii_us(keycode, shift);

    // outside the word processor (menus, dialogs) keys must stay Latin
    // so the navigation shortcuts keep working
    JsonDocument &app = status();
    if (app["screen"].as<int>() != WORDPROCESSOR)
        return ascii;

    // Hangul <-> English toggle: the dedicated toggle event, which the
    // keyboard drivers send as alt with keycode 0 (see Keypad_68/48 and
    // USBHost).
    if (alt)
    {
        if (keycode == 0)
        {
            if (pressed)
                hangul = !hangul;
            return KEY_CONSUMED; // the toggle itself types nothing
        }

        // alt/Fn held with a real key: not ours - return "no mapping" so
        // the driver's own layer value (file switching etc.) is used
        return 0;
    }

    // the US mapping already resolved shift into the letter's case
    return keyboard_ascii_ko(ascii, pressed);
}
