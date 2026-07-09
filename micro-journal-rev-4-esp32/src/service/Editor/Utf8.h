#pragma once

#include <stdint.h>

//
// Minimal UTF-8 helpers shared by the Editor service and display renderers.
//
// The text buffer holds UTF-8 bytes. These helpers keep every operation on
// it (editing, line wrapping, paging, rendering) on character boundaries so
// a multi-byte character - a Korean syllable is 3 bytes - is never split.
//
// Malformed input degrades gracefully: an invalid or truncated sequence is
// treated as a run of single-byte characters, so at worst a garbage glyph
// is shown; cursor movement and wrapping never get stuck or corrupt text.
//

// true when the byte is a UTF-8 continuation byte (10xxxxxx)
static inline bool utf8_is_continuation(uint8_t b)
{
    return (b & 0xC0) == 0x80;
}

// Number of bytes in the character announced by this lead byte (1-4).
// Invalid lead bytes count as single-byte characters.
static inline int utf8_length(uint8_t lead)
{
    if (lead < 0x80)
        return 1;
    if ((lead & 0xE0) == 0xC0)
        return 2;
    if ((lead & 0xF0) == 0xE0)
        return 3;
    if ((lead & 0xF8) == 0xF0)
        return 4;
    return 1;
}

// Decode the character starting at p. Returns the codepoint and stores the
// number of bytes consumed in *length. A truncated sequence (e.g. cut off
// by the end of the buffer) decodes as its lead byte alone.
static inline uint32_t utf8_decode(const char *p, int *length)
{
    uint8_t lead = (uint8_t)p[0];
    int len = utf8_length(lead);

    static const uint8_t leadMask[5] = {0, 0x7F, 0x1F, 0x0F, 0x07};
    uint32_t codepoint = lead & leadMask[len];

    for (int i = 1; i < len; i++)
    {
        uint8_t b = (uint8_t)p[i];
        if (!utf8_is_continuation(b))
        {
            // truncated sequence - treat the lead byte as one character
            *length = 1;
            return lead;
        }
        codepoint = (codepoint << 6) | (b & 0x3F);
    }

    *length = len;
    return codepoint;
}

// Encode a codepoint into out (no null terminator, out must hold 4 bytes).
// Returns the number of bytes written (1-4).
static inline int utf8_encode(uint32_t codepoint, char *out)
{
    if (codepoint < 0x80)
    {
        out[0] = (char)codepoint;
        return 1;
    }
    if (codepoint < 0x800)
    {
        out[0] = (char)(0xC0 | (codepoint >> 6));
        out[1] = (char)(0x80 | (codepoint & 0x3F));
        return 2;
    }
    if (codepoint < 0x10000)
    {
        out[0] = (char)(0xE0 | (codepoint >> 12));
        out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | (codepoint & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (codepoint >> 18));
    out[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
    out[3] = (char)(0x80 | (codepoint & 0x3F));
    return 4;
}
