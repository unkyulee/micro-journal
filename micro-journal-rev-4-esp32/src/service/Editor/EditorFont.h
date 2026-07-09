#pragma once

#include <Arduino.h>

//
// Describes one selectable word processor font.
//
// The Editor service and the display backends share this descriptor so that
// nothing about a font (its pixel metrics, or later, how many display
// columns a character occupies) is hard coded in either place.
//
// fontData is opaque on purpose: each display backend uses a different font
// system (u8g2 fonts on RLCD/ILI9341/ST7735, M5GFX fonts on CARDPUTER,
// GFXfont on EPD) and only the backend that registered the font ever
// interprets the pointer.
//
struct EditorFont
{
    // stable name, used to select the font from config.json
    const char *id;

    // backend-specific font pointer (u8g2 font, GFXfont, M5GFX font, ...)
    const void *fontData;

    // pixel advance of one single-width glyph (monospace).
    // Filled with the declared value from the font table, then corrected at
    // setup by querying the display library when it can measure the font.
    int16_t glyphWidth;

    // pixel vertical advance between lines
    int16_t lineHeight;

    // How many display columns one codepoint occupies. Latin fonts leave
    // this NULL (every character is 1 column). A Korean font supplies a
    // function returning 2 for Hangul codepoints so that line wrapping and
    // cursor placement stay correct for double-width glyphs.
    uint8_t (*charColumns)(uint32_t codepoint);

    // Integer render scale for backends that support pixel doubling
    // (0 or 1 = native size). glyphWidth/lineHeight above are declared for
    // the SCALED glyphs. Used to thicken thin unicode fonts (unifont has
    // 1px strokes that look faint and broken on the reflective panel).
    uint8_t scale;
};

// charColumns implementation for Korean fonts: Hangul glyphs are rendered
// full-width (two columns), everything else half-width. Shared here so
// every display backend's font table can reference the same rule.
inline uint8_t editorfont_hangul_columns(uint32_t codepoint)
{
    if ((codepoint >= 0xAC00 && codepoint <= 0xD7A3) || // Hangul syllables
        (codepoint >= 0x1100 && codepoint <= 0x11FF) || // Hangul jamo
        (codepoint >= 0x3130 && codepoint <= 0x318F) || // compatibility jamo
        (codepoint >= 0xA960 && codepoint <= 0xA97F) || // jamo extended-A
        (codepoint >= 0xD7B0 && codepoint <= 0xD7FB))   // jamo extended-B
        return 2;

    return 1;
}
