#!/usr/bin/env python3
"""
Generate a GFXfont-format Korean font from a TTF, for the EPD (Rev.7)
display backend (lib/EPD, epdiy-style GFXfont + 4-bit grayscale nibble
bitmap - a different binary format from the u8g2 fonts used by RLCD/
ILI9341/ST7735/CARDPUTER; see generate_u8g2_korean_font.py for those).

Binary layout mirrors lib/EPD/epd_driver.h's GFXglyph/GFXfont/UnicodeInterval
structs and lib/EPD/font.c's draw_char()/get_glyph():
  - bitmap: all glyphs' pixels concatenated, 4 bits/pixel (0=background,
    15=ink), 2 pixels packed per byte, low nibble = even column.
  - glyph table: one GFXglyph per codepoint, grouped into maximal runs of
    consecutive codepoints (UnicodeInterval), sorted ascending - required
    by get_glyph()'s linear interval scan.
  - compressed = false (raw bitmap): the zlib per-glyph compression this
    format supports isn't needed here, and skipping it avoids depending on
    the ESP32 zlib build being byte-compatible with Python's.

Reuses generate_u8g2_korean_font.py's rasterizer and charset (ASCII, Hangul
compatibility jamo, the 2,350 KS X 1001 syllables) and the same round-trip
verification discipline: every generated glyph is decoded back (a Python
port of get_glyph()/draw_char()'s bitmap unpacking) and compared against
the rasterized source before the file is written.

Usage:
  python generate_gfxfont_korean_font.py <font.ttf> <pixel_size> <out.h> <var_name>
"""

import os
import sys
from PIL import ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from generate_u8g2_korean_font import rasterize, korean_charset


def pack_bitmap(glyphs):
    """Concatenate every glyph's pixels into one nibble-packed byte array
    (2 px/byte, low nibble = even column), and record each glyph's byte
    offset into it."""
    bitmap = bytearray()
    offsets = []
    for g in glyphs:
        offsets.append(len(bitmap))
        if g.width == 0 or g.height == 0:
            continue
        byte_width = (g.width + 1) // 2
        for row in g.bitmap:
            row_bytes = bytearray(byte_width)
            for x, bit in enumerate(row):
                if not bit:
                    continue
                nibble = 15
                if x % 2 == 0:
                    row_bytes[x // 2] |= nibble
                else:
                    row_bytes[x // 2] |= nibble << 4
            bitmap += row_bytes
    return bytes(bitmap), offsets


def build_intervals(glyphs):
    """Group glyphs (already sorted by encoding) into maximal runs of
    consecutive codepoints. Returns a list of (first, last, glyph_offset)
    - glyph_offset is the index into the glyph array of the run's first
    entry, matching get_glyph()'s `interval->offset + (cp - interval->first)`
    indexing."""
    intervals = []
    start = 0
    for i in range(1, len(glyphs) + 1):
        if i == len(glyphs) or glyphs[i].encoding != glyphs[i - 1].encoding + 1:
            intervals.append((glyphs[start].encoding, glyphs[i - 1].encoding, start))
            start = i
    return intervals


# ----------------------------------------------------------------------------
# verification: a faithful port of get_glyph() + draw_char()'s bitmap unpack
# ----------------------------------------------------------------------------

def decode_glyph(glyphs, intervals, bitmap, encoding):
    glyph = None
    for first, last, offset in intervals:
        if first <= encoding <= last:
            glyph = glyphs[offset + (encoding - first)]
            break
        if encoding < first:
            break
    if glyph is None:
        return None

    w, h = glyph.width, glyph.height
    if w == 0 or h == 0:
        return [], glyph.delta

    byte_width = (w + 1) // 2
    data_offset = glyph._data_offset
    rows = []
    for y in range(h):
        row = []
        for x in range(w):
            byte = bitmap[data_offset + y * byte_width + x // 2]
            nibble = (byte & 0xF) if x % 2 == 0 else (byte >> 4)
            row.append(1 if nibble else 0)
        rows.append(row)
    return rows, glyph.delta


def verify(glyphs, intervals, bitmap):
    for g in glyphs:
        decoded = decode_glyph(glyphs, intervals, bitmap, g.encoding)
        assert decoded is not None, f"U+{g.encoding:04X} not found"
        rows, delta = decoded
        assert delta == g.delta, f"U+{g.encoding:04X} advance mismatch"
        assert rows == g.bitmap, f"U+{g.encoding:04X} bitmap mismatch"


# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------

def main():
    ttf_path, size, out_path, var_name = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]

    font = ImageFont.truetype(ttf_path, size)
    ascent, descent = font.getmetrics()

    latin_adv = round(font.getlength("M"))
    hangul_adv = round(font.getlength("한"))
    if hangul_adv != 2 * latin_adv:
        sys.exit(f"font is not dual-width at size {size}: "
                 f"latin {latin_adv}, hangul {hangul_adv}")

    glyphs = [rasterize(font, ch, ascent) for ch in korean_charset()]
    glyphs.sort(key=lambda g: g.encoding)

    bitmap, offsets = pack_bitmap(glyphs)
    for g, off in zip(glyphs, offsets):
        g._data_offset = off
    intervals = build_intervals(glyphs)

    verify(glyphs, intervals, bitmap)

    # sample glyphs printed for a quick visual sanity check
    for sample in "Mg한글ㄱ":
        decoded = decode_glyph(glyphs, intervals, bitmap, ord(sample))
        if decoded is None:
            print(f"'{sample}': not in font")
            continue
        rows, delta = decoded
        print(f"'{sample}' U+{ord(sample):04X} delta={delta}")
        for row in rows:
            print("".join("#" if v else "." for v in row))
        print()

    a = rasterize(font, "A", ascent)
    advance_y = min(255, size + size // 4)  # cosmetic only - see header comment

    with open(out_path, "w", newline="\n") as f:
        f.write(
            "#pragma once\n\n"
            "/*\n"
            f"  {var_name}\n"
            f"  Generated by script/generate_gfxfont_korean_font.py from\n"
            f"  {ttf_path.rsplit(chr(92), 1)[-1].rsplit('/', 1)[-1]} rasterized at {size}px.\n"
            f"  Glyphs: {len(glyphs)} (ASCII, Hangul compatibility jamo,\n"
            f"  KS X 1001 syllables). Latin advance {latin_adv}px, Hangul {hangul_adv}px.\n"
            "  advance_y/ascender/descender are cosmetic: this project's EPD\n"
            "  renderer manages line spacing itself and never sets DRAW_BACKGROUND,\n"
            "  the only paths that read them (see lib/EPD/font.c).\n"
            "*/\n\n"
            "#include \"epd_driver.h\"\n\n"
        )

        f.write(f"const uint8_t {var_name}Bitmap[{len(bitmap)}] = {{\n")
        for i in range(0, len(bitmap), 16):
            chunk = ", ".join(f"0x{b:02x}" for b in bitmap[i:i + 16])
            f.write(f"  {chunk},\n")
        f.write("};\n\n")

        f.write(f"const GFXglyph {var_name}Glyphs[{len(glyphs)}] = {{\n")
        for g in glyphs:
            f.write(f"  {{ {g.width}, {g.height}, {g.delta}, {g.x}, {g.height + g.y}, "
                   f"0, {g._data_offset} }}, // U+{g.encoding:04X}\n")
        f.write("};\n\n")

        f.write(f"const UnicodeInterval {var_name}Intervals[{len(intervals)}] = {{\n")
        for first, last, offset in intervals:
            f.write(f"  {{ 0x{first:04x}, 0x{last:04x}, {offset} }},\n")
        f.write("};\n\n")

        f.write(
            f"const GFXfont {var_name} = {{\n"
            f"  (uint8_t *){var_name}Bitmap,\n"
            f"  (GFXglyph *){var_name}Glyphs,\n"
            f"  (UnicodeInterval *){var_name}Intervals,\n"
            f"  {len(intervals)},\n"
            f"  false,\n"
            f"  {advance_y},\n"
            f"  {a.height + a.y},\n"
            f"  0,\n"
            "};\n"
        )

    print(f"wrote {out_path}: bitmap {len(bitmap)} bytes, {len(glyphs)} glyphs, "
         f"{len(intervals)} intervals, latin {latin_adv}px hangul {hangul_adv}px")


if __name__ == "__main__":
    main()
