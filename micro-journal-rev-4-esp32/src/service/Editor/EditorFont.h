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
};
