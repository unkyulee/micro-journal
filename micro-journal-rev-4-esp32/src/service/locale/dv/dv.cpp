#include "dv.h"
#include "../locale.h"

/*
Translate a hardware QWERTY keyboard keycode to a Dvorak keycode. This allows
for a standard keyboard to be used as a Dvorak keyboard. Do not use a custom QMK
Dvorak keyboard (or other such constructs) with this setting because you will
not get the expected results. Layout positions follow as closely as posible the
Mac OS Dvorak layout. Keycodes come from WordProcessor.cpp
*/

// Fields: {normal, shift, alt, alt-shift}
// Comment: show first the letter in QWERTY, then notes on the alt codes
uint8_t hid_keycode_to_ascii_dv[][4] = {
    {0, 0, 0, 0},               /* 0x00 */
    {0, 0, 0, 0},               /* 0x01 */
    {0, 0, 0, 0},               /* 0x02 */
    {0, 0, 0, 0},               /* 0x03 */
    {'a', 'A', 229, 197},       /* 0x04 a; funny a */
    {'x', 'X', 170, 0},         /* 0x05 b; little a */
    {'j', 'J', 0, 0},           /* 0x06 c */
    {'e', 'E', 0, 0},           /* 0x07 d */
    {'.', '>', 168, 0},         /* 0x08 e; 2 dots */
    {'u', 'U', 0, 0},           /* 0x09 f */
    {'i', 'I', 0, 0},           /* 0x0a g */
    {'d', 'D', 208, 0},         /* 0x0b h; funny d */
    {'c', 'C', 231, 199},       /* 0x0c i; funny c */
    {'h', 'H', 0, 0},           /* 0x0d j */
    {'t', 'T', 0, 0},           /* 0x0e k */
    {'n', 'N', 241, 209},       /* 0x0f l; n like things */
    {'m', 'M', 181, 0},         /* 0x10 m; micro */
    {'b', 'B', 0, 0},           /* 0x11 n */
    {'r', 'R', 0, 0},           /* 0x12 o */
    {'l', 'L', 172, 210},       /* 0x13 p; break, funny o */
    {'"', '\'', 230, 198},      /* 0x14 q; AE */
    {'p', 'P', 222, 254},       /* 0x15 r; a p like things */
    {'o', 'O', 248, 216},       /* 0x16 s; O with slash in it */
    {'y', 'Y', 165, 0},         /* 0x17 t, yen */
    {'g', 'G', 169, 174},       /* 0x18 u; copyright, reserved */
    {'k', 'K', 0, 0},           /* 0x19 v */
    {',', '<', 0, 0},           /* 0x1a w */
    {'q', 'Q', 0, 0},           /* 0x1b x */
    {'f', 'F', 0, 0},           /* 0x1c y */
    {';', ':', 166, 0},         /* 0x1d z; a pipe */
    {'1', '!', 185, 161},       /* 0x1e 1; small 1, upside down exclimation */
    {'2', '@', 178, 189},       /* 0x1f 2; small 2, half frac */
    {'3', '#', 179, 163},       /* 0x20 3; small 3, pound */
    {'4', '$', 162, 188},       /* 0x21 4; cent, quarter frac */
    {'5', '%', 0, 0},           /* 0x22 5 */
    {'6', '^', 167, 0},         /* 0x23 6; generic currency */
    {'7', '&', 182, 190},       /* 0x24 7; paragraph mark, 3/4 */
    {'8', '*', 176, 183},       /* 0x25 8; degree, dot */
    {'9', '(', 0, 0},           /* 0x26 9 */
    {'0', ')', 186, 0},         /* 0x27 0; small 0 */
    {'\n', '\n', '\n', '\n'},   /* 0x28 new line */
    {27, 27, 27, 27},           /* 0x29 ESC */
    {'\b', '\b', '\b', '\b'},   /* 0x2a backspace */
    {'\t', '\t', '\t', '\t'},   /* 0x2b tab */
    {' ', ' ', ' ', ' '},       /* 0x2c space */
    {'[', '{', 171, 0},         /* 0x2d dash; opening chevrons */
    {']', '}', 187, 0},         /* 0x2e equals; closing cheverons */
    {'/', '?', 247, 191},       /* 0x2f [; divide, upside down question */
    {'=', '+', 177, 0},         /* 0x30 ]; plus minus */
    {'\\', '|', 171, 187},      /* 0x31 slash; left right double cheverons */
    {'#', '~', 165, 0},         /* 0x32 int value; yen, only some keyboards do this */
    {'s', 'S', 223, 0},         /* 0x33 simicolon; ß */
    {'-', '_', 173, 175},       /* 0x34 quote; short dash, high dash*/
    {'`', '~', 0, 0},           /* 0x35 tick/tildi  */
    {'w', 'W', 0, 0},           /* 0x36 comma */
    {'v', 'V', 164, 0},         /* 0x37 period; x like thing, */
    {'z', 'Z', 184, 0},         /* 0x38 slash; comma like */
    {0, 0, 0, 0},               /* 0x39 */
    {0, 0, 0, 0},               /* 0x3a */
    {0, 0, 0, 0},               /* 0x3b */
    {0, 0, 0, 0},               /* 0x3c */
    {0, 0, 0, 0},               /* 0x3d */
    {5, 5, 5, 5},               /* 0x3e F5 */
    {0, 0, 0, 0},               /* 0x3f */
    {0, 0, 0, 0},               /* 0x40 */
    {0, 0, 0, 0},               /* 0x41 */
    {0, 0, 0, 0},               /* 0x42 */
    {0, 0, 0, 0},               /* 0x43 */
    {0, 0, 0, 0},               /* 0x44 */
    {0, 0, 0, 0},               /* 0x45 */
    {0, 0, 0, 0},               /* 0x46 */
    {0, 0, 0, 0},               /* 0x47 */
    {24, 24, 24, 24},           /* 0x48 PAUSE */
    {0, 0, 0, 0},               /* 0x49 */
    {2, 2, 2, 2},               /* 0x4a Home */
    {22, 22, 22, 22},           /* 0x4b PAGE UP */
    {127, 127, 127, 127},       /* 0x4c */
    {3, 3, 3, 3},               /* 0x4d End */
    {23, 23, 23, 23},           /* 0x4e PAGE DOWN */
    {19, 19, 19, 19},           /* 0x4f Right */
    {18, 18, 18, 18},           /* 0x50 Left */
    {21, 21, 21, 21},           /* 0x51 Down */
    {20, 20, 20, 20},           /* 0x52 Up */
    {0, 0, 0, 0},               /* 0x53 */
    {'z', 'Z', 0, 0},           /* 0x54 */
    {'*', '*', 0, 0},           /* 0x55 */
    {'[', '[', 0, 0},           /* 0x56 */
    {'+', '+', 0, 0},           /* 0x57 */
    {'n', 'n', 0, 0},           /* 0x58 */
    {'1', 0, '1', 0},           /* 0x59 number pad 1 */
    {'2', 0, '2', 0},           /* 0x5a */
    {'3', 0, '3', 0},           /* 0x5b */
    {'4', 0, '4', 0},           /* 0x5c */
    {'5', 0, '5', 0},           /* 0x5d */
    {'6', 0, '6', 0},           /* 0x5e */
    {'7', 0, '7', 0},           /* 0x5f */
    {'8', 0, '8', 0},           /* 0x60 */
    {'9', 0, '9', 0},           /* 0x61 */
    {'0', 0, '0', 0},           /* 0x62 */
    {'.', 0, '.', 0},           /* 0x63 */
    {0, 0, 0, 0},               /* 0x64 */
    {0, 0, 0, 0},               /* 0x65 */
    {0, 0, 0, 0},               /* 0x66 */
    {']', '=', 0, 0},           /* 0x67 */
};

// Function to get the ASCII value from HID keycode and modifier
uint8_t keyboard_keycode_ascii_dv(int keycode, bool shift, bool alt)
{
    if (keycode >= sizeof(hid_keycode_to_ascii_dv) / sizeof(hid_keycode_to_ascii_dv[0]))
    {
        return 0; // Invalid keycode
    }

    //{ normal, shift, alt0, alt1 }
    int offset_alt = alt ? 2 : 0;
    int offset_shift = shift ? 1 : 0;

    int ascii = hid_keycode_to_ascii_dv[keycode][offset_alt  + offset_shift];
    // go through capslock filter
    ascii = keyboard_caplock_filter(ascii);

    return ascii;
}
