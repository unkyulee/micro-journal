#!/usr/bin/env python3
"""
Generate a u8g2-format Korean font from a TTF (e.g. NanumGothicCoding).

The output is a C file with a font byte array compatible with the u8g2
font engine bundled in lib/ST7305_MonoTFT_Library (U8g2_for_ST73XX) and
any other u8g2-based renderer in this project.

Glyph set:
  - ASCII 0x20-0x7E
  - Hangul compatibility jamo U+3131-U+3163 (for IME composing display)
  - the 2,350 KS X 1001 Hangul syllables (derived from the euc-kr codec)

The font must be monospaced dual-width: Hangul advance exactly twice the
Latin advance (NanumGothicCoding is designed this way). The generator
fails if that doesn't hold at the chosen pixel size.

Every generated glyph is round-trip verified: the byte array is decoded
with a Python port of the C glyph decoder and compared pixel-for-pixel
against the rasterized source bitmap.

Usage:
  python generate_u8g2_korean_font.py <font.ttf> <pixel_size> <out.c> <font_name>

Example:
  python generate_u8g2_korean_font.py D2CodingBold-Ver1.3.2-20180524.ttf 22 \
      src/display/RLCD/fonts/u8g2_font_nanum_22_t_korean.c u8g2_font_nanum_22_t_korean
"""

import sys
from pathlib import Path
from PIL import Image, ImageFont


# ----------------------------------------------------------------------------
# glyph set
# ----------------------------------------------------------------------------

def korean_charset():
    chars = [chr(c) for c in range(0x20, 0x7F)]          # ASCII
    chars += [chr(c) for c in range(0x3131, 0x3164)]     # compatibility jamo
    # the 2,350 KS X 1001 syllables: rows 0xB0-0xC8 of euc-kr
    for lead in range(0xB0, 0xC9):
        for trail in range(0xA1, 0xFF):
            chars.append(bytes([lead, trail]).decode("euc-kr"))
    return chars


# ----------------------------------------------------------------------------
# rasterization
# ----------------------------------------------------------------------------

class Glyph:
    def __init__(self, encoding, width, height, x, y, delta, bitmap):
        self.encoding = encoding  # unicode codepoint
        self.width = width        # tight bitmap width
        self.height = height      # tight bitmap height
        self.x = x                # left side bearing
        self.y = y                # u8g2 char_y: -(rows below baseline)
        self.delta = delta        # advance
        self.bitmap = bitmap      # list of rows, each list of 0/1


def rasterize(font, ch, ascent, threshold=128):
    encoding = ord(ch)
    delta = round(font.getlength(ch))

    # mode "1" = FreeType monochrome hinting: stems snap to consistent
    # integer pixel widths. Antialiased "L" + threshold makes identical
    # stems come out 2px or 3px depending on their subpixel phase (e.g.
    # the ㅏ stem thicker than the ㅣ stem at the same size).
    mask, (dx, dy) = font.getmask2(ch, mode="1")
    w, h = mask.size

    if w == 0 or h == 0:
        return Glyph(encoding, 0, 0, 0, 0, delta, [])

    img = Image.frombytes("L", (w, h), bytes(mask))
    px = img.load()
    rows = [[1 if px[cx, cy] >= threshold else 0 for cx in range(w)]
            for cy in range(h)]

    # trim to the tight bounding box of set pixels
    set_rows = [r for r, row in enumerate(rows) if any(row)]
    if not set_rows:
        return Glyph(encoding, 0, 0, 0, 0, delta, [])
    set_cols = [c for c in range(w) if any(rows[r][c] for r in set_rows)]
    top, bottom = set_rows[0], set_rows[-1]
    left, right = set_cols[0], set_cols[-1]
    bitmap = [rows[r][left:right + 1] for r in range(top, bottom + 1)]

    width = right - left + 1
    height = bottom - top + 1
    x = dx + left
    # rows of the glyph above the baseline; u8g2 stores char_y so that
    # (height + char_y) equals that count
    above_baseline = ascent - (dy + top)
    y = above_baseline - height

    return Glyph(encoding, width, height, x, y, delta, bitmap)


# ----------------------------------------------------------------------------
# u8g2 encoding
# ----------------------------------------------------------------------------

class BitWriter:
    def __init__(self):
        self.data = bytearray()
        self.bit_pos = 0  # bits already used in the last byte (LSB first)

    def write(self, value, cnt):
        for i in range(cnt):
            bit = (value >> i) & 1
            if self.bit_pos == 0:
                self.data.append(0)
            if bit:
                self.data[-1] |= 1 << self.bit_pos
            self.bit_pos = (self.bit_pos + 1) % 8

    def write_signed(self, value, cnt):
        self.write(value + (1 << (cnt - 1)), cnt)


def bits_for_unsigned(maxval):
    n = 1
    while (1 << n) - 1 < maxval:
        n += 1
    return n


def bits_for_signed(minval, maxval):
    n = 2
    while not (-(1 << (n - 1)) <= minval and maxval <= (1 << (n - 1)) - 1):
        n += 1
    return n


def rle_pairs(bitmap, width, height, max0, max1):
    """Flatten the bitmap row-major and produce (zeros, ones) pairs whose
    concatenation reproduces it exactly, respecting the field limits."""
    stream = [b for row in bitmap for b in row]
    total = width * height
    assert len(stream) == total

    pairs = []
    i = 0
    while i < total:
        n0 = 0
        while i < total and stream[i] == 0:
            n0 += 1
            i += 1
        n1 = 0
        while i < total and stream[i] == 1:
            n1 += 1
            i += 1
        while n0 > max0:
            pairs.append((max0, 0))
            n0 -= max0
        while n1 > max1:
            pairs.append((n0, max1))
            n0 = 0
            n1 -= max1
        pairs.append((n0, n1))
    return pairs


def encode_glyph_payload(g, layout):
    """Bit-packed glyph fields + RLE bitmap (no encoding/size prefix)."""
    w = BitWriter()
    w.write(g.width, layout["bits_w"])
    w.write(g.height, layout["bits_h"])
    w.write_signed(g.x, layout["bits_x"])
    w.write_signed(g.y, layout["bits_y"])
    w.write_signed(g.delta, layout["bits_d"])

    if g.width > 0:
        max0 = (1 << layout["bits_0"]) - 1
        max1 = (1 << layout["bits_1"]) - 1
        pairs = rle_pairs(g.bitmap, g.width, g.height, max0, max1)
        i = 0
        while i < len(pairs):
            j = i
            while j + 1 < len(pairs) and pairs[j + 1] == pairs[i]:
                j += 1
            a, b = pairs[i]
            w.write(a, layout["bits_0"])
            w.write(b, layout["bits_1"])
            for _ in range(j - i):
                w.write(1, 1)  # repeat the same pair
            w.write(0, 1)      # move on
            i = j + 1
    return bytes(w.data)


def build_font(glyphs, font_metrics):
    glyphs = sorted(glyphs, key=lambda g: g.encoding)
    ascii_glyphs = [g for g in glyphs if g.encoding <= 255]
    unicode_glyphs = [g for g in glyphs if g.encoding > 255]

    layout = {
        "bits_w": bits_for_unsigned(max(g.width for g in glyphs)),
        "bits_h": bits_for_unsigned(max(g.height for g in glyphs)),
        "bits_x": bits_for_signed(min(g.x for g in glyphs),
                                  max(g.x for g in glyphs)),
        "bits_y": bits_for_signed(min(g.y for g in glyphs),
                                  max(g.y for g in glyphs)),
        "bits_d": bits_for_signed(min(g.delta for g in glyphs),
                                  max(g.delta for g in glyphs)),
    }

    # pick the RLE bit widths that give the smallest total size
    best = None
    for b0 in range(2, 9):
        for b1 in range(2, 9):
            layout["bits_0"], layout["bits_1"] = b0, b1
            size = sum(len(encode_glyph_payload(g, layout)) for g in glyphs)
            if best is None or size < best[0]:
                best = (size, b0, b1)
    layout["bits_0"], layout["bits_1"] = best[1], best[2]

    payloads = {g.encoding: encode_glyph_payload(g, layout) for g in glyphs}

    # section of encodings <= 255: [enc][size][payload], size covers all 3 parts
    ascii_section = bytearray()
    pos_upper_A = pos_lower_a = 0
    for g in ascii_glyphs:
        if g.encoding == ord("A"):
            pos_upper_A = len(ascii_section)
        if g.encoding == ord("a"):
            pos_lower_a = len(ascii_section)
        payload = payloads[g.encoding]
        size = 2 + len(payload)
        assert size <= 255, f"glyph {g.encoding} record too large"
        ascii_section += bytes([g.encoding, size]) + payload
    ascii_section += bytes([0, 0])  # terminator (size byte 0)

    # unicode section: jump table + blocks of [enc_hi][enc_lo][size][payload]
    BLOCK = 100
    blocks = [unicode_glyphs[i:i + BLOCK]
              for i in range(0, len(unicode_glyphs), BLOCK)]
    block_bytes = []
    for block in blocks:
        bb = bytearray()
        for g in block:
            payload = payloads[g.encoding]
            size = 3 + len(payload)
            assert size <= 255, f"glyph {g.encoding} record too large"
            bb += bytes([g.encoding >> 8, g.encoding & 0xFF, size]) + payload
        block_bytes.append(bytes(bb))

    # jump table rows: (offset delta, last encoding of the block); the first
    # delta skips the table itself, each next one skips the previous block.
    # A final (delta, 0xFFFF) row guarantees the search loop terminates.
    table = bytearray()
    n_rows = len(blocks) + 1
    deltas = [4 * n_rows] + [len(bb) for bb in block_bytes[:-1]]
    for delta, block in zip(deltas, blocks):
        last_enc = block[-1].encoding
        table += bytes([delta >> 8, delta & 0xFF, last_enc >> 8, last_enc & 0xFF])
    last_delta = len(block_bytes[-1]) if block_bytes else 0
    table += bytes([last_delta >> 8, last_delta & 0xFF, 0xFF, 0xFF])

    unicode_section = bytes(table) + b"".join(block_bytes) + bytes([0, 0])

    start_pos_unicode = len(ascii_section)

    header = bytes([
        len(glyphs) & 0xFF,             # 0 glyph count (informational)
        0,                              # 1 bbx mode
        layout["bits_0"],               # 2
        layout["bits_1"],               # 3
        layout["bits_w"],               # 4
        layout["bits_h"],               # 5
        layout["bits_x"],               # 6
        layout["bits_y"],               # 7
        layout["bits_d"],               # 8
        max(g.width for g in glyphs),   # 9  max char width
        max(g.height for g in glyphs),  # 10 max char height
        0,                              # 11 x offset
        font_metrics["y_offset"] & 0xFF,      # 12 y offset
        font_metrics["ascent_A"] & 0xFF,      # 13
        font_metrics["descent_g"] & 0xFF,     # 14
        font_metrics["ascent_para"] & 0xFF,   # 15
        font_metrics["descent_para"] & 0xFF,  # 16
        pos_upper_A >> 8, pos_upper_A & 0xFF,       # 17-18
        pos_lower_a >> 8, pos_lower_a & 0xFF,       # 19-20
        start_pos_unicode >> 8, start_pos_unicode & 0xFF,  # 21-22
    ])

    return header + bytes(ascii_section) + unicode_section, layout


# ----------------------------------------------------------------------------
# verification: a faithful port of the C glyph decoder
# ----------------------------------------------------------------------------

class BitReader:
    def __init__(self, data, pos):
        self.data = data
        self.pos = pos
        self.bit_pos = 0

    def read(self, cnt):
        val = self.data[self.pos] >> self.bit_pos
        if self.bit_pos + cnt >= 8:
            self.pos += 1
            val |= self.data[self.pos] << (8 - self.bit_pos)
            self.bit_pos -= 8
        val &= (1 << cnt) - 1
        self.bit_pos += cnt
        return val

    def read_signed(self, cnt):
        return self.read(cnt) - (1 << (cnt - 1))


def decode_find_glyph(font, encoding):
    """Port of u8g2_font_get_glyph_data()."""
    def word(pos):
        return (font[pos] << 8) | font[pos + 1]

    pos = 23
    if encoding <= 255:
        if encoding >= ord("a"):
            pos += word(19)
        elif encoding >= ord("A"):
            pos += word(17)
        while True:
            if font[pos + 1] == 0:
                return None
            if font[pos] == encoding:
                return pos + 2
            pos += font[pos + 1]
    else:
        pos += word(21)
        table = pos
        while True:
            pos += word(table)
            e = word(table + 2)
            table += 4
            if e >= encoding:
                break
        while True:
            e = word(pos)
            if e == 0:
                return None
            if e == encoding:
                return pos + 3
            pos += font[pos + 2]


def decode_glyph(font, encoding):
    """Port of u8g2_font_decode_glyph(); returns (Glyph, delta) or None."""
    pos = decode_find_glyph(font, encoding)
    if pos is None:
        return None

    bits_0, bits_1 = font[2], font[3]
    bits_w, bits_h, bits_x, bits_y, bits_d = font[4], font[5], font[6], font[7], font[8]

    r = BitReader(font, pos)
    w = r.read(bits_w)
    h = r.read(bits_h)
    x = r.read_signed(bits_x)
    y = r.read_signed(bits_y)
    d = r.read_signed(bits_d)

    bitmap = [[0] * w for _ in range(h)] if w > 0 else []
    if w > 0:
        lx = ly = 0
        while True:
            a = r.read(bits_0)
            b = r.read(bits_1)
            while True:
                for bit, length in ((0, a), (1, b)):
                    cnt = length
                    while cnt > 0:
                        rem = w - lx
                        cur = min(cnt, rem)
                        if bit:
                            for i in range(cur):
                                bitmap[ly][lx + i] = 1
                        if cnt < rem:
                            lx += cnt
                            cnt = 0
                        else:
                            cnt -= rem
                            lx = 0
                            ly += 1
                if r.read(1) == 0:
                    break
            if ly >= h:
                break

    return Glyph(encoding, w, h, x, y, d, bitmap), d


def verify(font_data, glyphs):
    for g in glyphs:
        decoded = decode_glyph(font_data, g.encoding)
        assert decoded is not None, f"U+{g.encoding:04X} not found"
        dg, delta = decoded
        assert (dg.width, dg.height, dg.x, dg.y, delta) == \
               (g.width, g.height, g.x, g.y, g.delta), \
               f"U+{g.encoding:04X} fields mismatch"
        assert dg.bitmap == g.bitmap, f"U+{g.encoding:04X} bitmap mismatch"


def load_u8g2_c_array(src_path, array_name):
    """Extract a `const uint8_t name[N] = "..." "...";` byte array from a
    u8g2 C source file (handles octal/named escapes across concatenated
    string literals, e.g. the fonts bundled in lib/ST7305_MonoTFT_Library).
    """
    with open(src_path, "r", encoding="latin-1") as f:
        text = f.read()
    i = text.index(f"const uint8_t {array_name}[")
    i = text.index("=", i) + 1  # skip past U8G2_FONT_SECTION("...")

    escapes = {"n": 10, "t": 9, "r": 13, "\\": 92, "'": 39, '"': 34,
               "a": 7, "b": 8, "f": 12, "v": 11}
    data = bytearray()
    while True:
        while text[i] in " \t\r\n":
            i += 1
        if text[i] == ";":
            return bytes(data)
        assert text[i] == '"', f"unexpected char {text[i]!r} at {i}"
        i += 1
        while text[i] != '"':
            c = text[i]
            if c == "\\":
                i += 1
                if text[i] in "01234567":
                    j = i
                    while j < len(text) and j < i + 3 and text[j] in "01234567":
                        j += 1
                    data.append(int(text[i:j], 8))
                    i = j
                else:
                    data.append(escapes[text[i]])
                    i += 1
            else:
                data.append(ord(c))
                i += 1
        i += 1  # closing quote


def load_ascii_source(src_path, array_name):
    """Load ASCII glyphs (0x20-0x7E) and header metrics from an existing,
    already-baked u8g2 font (e.g. the bundled profont22_mf), for splicing
    into a generated font as its Latin half. Uses the same decoder that
    verifies every generated font, so the reused bitmaps are known-good."""
    font = load_u8g2_c_array(src_path, array_name)
    glyphs = []
    for cp in range(0x20, 0x7F):
        decoded = decode_glyph(font, cp)
        if decoded is None:
            continue
        g, delta = decoded
        g.delta = delta
        glyphs.append(g)

    metrics = {
        "ascent_A": font[13] - 256 if font[13] > 127 else font[13],
        "descent_g": font[14] - 256 if font[14] > 127 else font[14],
        "ascent_para": font[15] - 256 if font[15] > 127 else font[15],
        "descent_para": font[16] - 256 if font[16] > 127 else font[16],
        "y_offset": font[12] - 256 if font[12] > 127 else font[12],
    }
    return glyphs, metrics


def ascii_art(font_data, ch):
    decoded = decode_glyph(font_data, ord(ch))
    if decoded is None:
        return f"U+{ord(ch):04X} '{ch}': not in font"
    g, delta = decoded
    lines = [f"U+{ord(ch):04X} '{ch}' w={g.width} h={g.height} x={g.x} y={g.y} delta={delta}"]
    for row in g.bitmap:
        lines.append("".join("#" if v else "." for v in row))
    return "\n".join(lines)


# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------

def main():
    args = sys.argv[1:]

    # optional: --ascii-font <u8g2_src.c> <array_name> - reuse an existing,
    # already-baked monospace bitmap font (e.g. profont22_mf) for ASCII
    # instead of rasterizing Latin glyphs from the TTF. Use this when the
    # TTF's own Latin glyphs are not fixed-width (many "pixel" fonts style
    # Hangul as a uniform full-width cell but keep Latin proportional -
    # incompatible with this project's fixed dual-width column model).
    ascii_source = None
    if "--ascii-font" in args:
        idx = args.index("--ascii-font")
        ascii_source = (args[idx + 1], args[idx + 2])
        del args[idx:idx + 3]

    ttf_path, size, out_path, font_name = args[0], int(args[1]), args[2], args[3]
    source_font = Path(ttf_path).name

    font = ImageFont.truetype(ttf_path, size)
    ascent, descent = font.getmetrics()

    hangul_adv = round(font.getlength("한"))

    if ascii_source:
        ascii_glyphs, ascii_metrics = load_ascii_source(*ascii_source)
        latin_adv = ascii_glyphs[0].delta
        for g in ascii_glyphs:
            if g.delta != latin_adv:
                sys.exit(f"--ascii-font is not monospace: U+{g.encoding:04X} "
                         f"advance {g.delta} != {latin_adv}")
        if hangul_adv != 2 * latin_adv:
            sys.exit(f"hangul advance {hangul_adv} at size {size} is not "
                     f"2x the ascii font's advance {latin_adv}")

        hangul_charset = [c for c in korean_charset() if ord(c) > 0x7F]
        hangul_glyphs = [rasterize(font, ch, ascent) for ch in hangul_charset]
        glyphs = ascii_glyphs + hangul_glyphs
        metrics = ascii_metrics
    else:
        latin_adv = round(font.getlength("M"))
        if hangul_adv != 2 * latin_adv:
            sys.exit(f"font is not dual-width at size {size}: "
                     f"latin {latin_adv}, hangul {hangul_adv}")

        glyphs = [rasterize(font, ch, ascent) for ch in korean_charset()]

        a = rasterize(font, "A", ascent)
        g_ = rasterize(font, "g", ascent)
        metrics = {
            "y_offset": -descent,
            "ascent_A": a.height + a.y,
            "descent_g": g_.y,
            "ascent_para": ascent,
            "descent_para": -descent,
        }

    font_data, layout = build_font(glyphs, metrics)
    verify(font_data, glyphs)

    for sample in "Mg한글ㄱ":
        print(ascii_art(font_data, sample))
        print()

    with open(out_path, "w", newline="\n") as f:
        f.write(
            "/*\n"
            f"  {font_name}\n"
            f"  Generated by script/generate_u8g2_korean_font.py from\n"
            f"  {source_font} rasterized at {size}px.\n"
            f"  Glyphs: {len(glyphs)} (ASCII, Hangul compatibility jamo,\n"
            f"  KS X 1001 syllables). Latin advance {latin_adv}px, Hangul {hangul_adv}px.\n"
            f"  ascent {ascent} descent {descent}\n"
            "*/\n"
            "#include <u8g2_fonts.h>\n\n"
            f"const uint8_t {font_name}[{len(font_data)}] U8G2_FONT_SECTION(\"{font_name}\") = {{\n"
        )
        for i in range(0, len(font_data), 16):
            chunk = ", ".join(f"0x{b:02x}" for b in font_data[i:i + 16])
            f.write(f"  {chunk},\n")
        f.write("};\n")

    print(f"wrote {out_path}: {len(font_data)} bytes, {len(glyphs)} glyphs, "
          f"latin {latin_adv}px, hangul {hangul_adv}px, "
          f"ascent {ascent}, descent {descent}, "
          f"rle bits ({layout['bits_0']},{layout['bits_1']})")


if __name__ == "__main__":
    main()
