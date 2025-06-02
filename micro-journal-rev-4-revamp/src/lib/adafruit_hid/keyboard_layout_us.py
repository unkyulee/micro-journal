# SPDX-FileCopyrightText: 2017 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`adafruit_hid.keyboard_layout_us.KeyboardLayoutUS`
=======================================================

* Author(s): Dan Halbert
"""

from .keyboard_layout_base import KeyboardLayoutBase


class KeyboardLayoutUS(KeyboardLayoutBase):
    """Map ASCII characters to appropriate keypresses on a standard US PC keyboard.

    Non-ASCII characters and most control characters will raise an exception.
    """

    # The ASCII_TO_KEYCODE bytes object is used as a table to maps ASCII 0-127
    # to the corresponding # keycode on a US 104-key keyboard.
    # The user should not normally need to use this table,
    # but it is not marked as private.
    #
    # Because the table only goes to 127, we use the top bit of each byte (ox80) to indicate
    # that the shift key should be pressed. So any values 0x{8,9,a,b}* are shifted characters.
    #
    # The Python compiler will concatenate all these bytes literals into a single bytes object.
    # Micropython/CircuitPython will store the resulting bytes constant in flash memory
    # if it's in a .mpy file, so it doesn't use up valuable RAM.
    #
    # \x00 entries have no keyboard key and so won't be sent.
    ASCII_TO_KEYCODE = (
        b"\x00"  # NUL
        b"\x00"  # SOH
        b"\x00"  # STX
        b"\x00"  # ETX
        b"\x00"  # EOT
        b"\x00"  # ENQ
        b"\x00"  # ACK
        b"\x00"  # BEL \a
        b"\x2a"  # BS BACKSPACE \b (called DELETE in the usb.org document)
        b"\x2b"  # TAB \t
        b"\x28"  # LF \n (called Return or ENTER in the usb.org document)
        b"\x00"  # VT \v
        b"\x00"  # FF \f
        b"\x00"  # CR \r
        b"\x00"  # SO
        b"\x00"  # SI
        b"\x00"  # DLE
        b"\x00"  # DC1
        b"\x00"  # DC2
        b"\x00"  # DC3
        b"\x00"  # DC4
        b"\x00"  # NAK
        b"\x00"  # SYN
        b"\x00"  # ETB
        b"\x00"  # CAN
        b"\x00"  # EM
        b"\x00"  # SUB
        b"\x29"  # ESC
        b"\x00"  # FS
        b"\x00"  # GS
        b"\x00"  # RS
        b"\x00"  # US
        b"\x2c"  # SPACE
        b"\x9e"  # ! x1e|SHIFT_FLAG (shift 1)
        b"\xb4"  # " x34|SHIFT_FLAG (shift ')
        b"\xa0"  # # x20|SHIFT_FLAG (shift 3)
        b"\xa1"  # $ x21|SHIFT_FLAG (shift 4)
        b"\xa2"  # % x22|SHIFT_FLAG (shift 5)
        b"\xa4"  # & x24|SHIFT_FLAG (shift 7)
        b"\x34"  # '
        b"\xa6"  # ( x26|SHIFT_FLAG (shift 9)
        b"\xa7"  # ) x27|SHIFT_FLAG (shift 0)
        b"\xa5"  # * x25|SHIFT_FLAG (shift 8)
        b"\xae"  # + x2e|SHIFT_FLAG (shift =)
        b"\x36"  # ,
        b"\x2d"  # -
        b"\x37"  # .
        b"\x38"  # /
        b"\x27"  # 0
        b"\x1e"  # 1
        b"\x1f"  # 2
        b"\x20"  # 3
        b"\x21"  # 4
        b"\x22"  # 5
        b"\x23"  # 6
        b"\x24"  # 7
        b"\x25"  # 8
        b"\x26"  # 9
        b"\xb3"  # : x33|SHIFT_FLAG (shift ;)
        b"\x33"  # ;
        b"\xb6"  # < x36|SHIFT_FLAG (shift ,)
        b"\x2e"  # =
        b"\xb7"  # > x37|SHIFT_FLAG (shift .)
        b"\xb8"  # ? x38|SHIFT_FLAG (shift /)
        b"\x9f"  # @ x1f|SHIFT_FLAG (shift 2)
        b"\x84"  # A x04|SHIFT_FLAG (shift a)
        b"\x85"  # B x05|SHIFT_FLAG (etc.)
        b"\x86"  # C x06|SHIFT_FLAG
        b"\x87"  # D x07|SHIFT_FLAG
        b"\x88"  # E x08|SHIFT_FLAG
        b"\x89"  # F x09|SHIFT_FLAG
        b"\x8a"  # G x0a|SHIFT_FLAG
        b"\x8b"  # H x0b|SHIFT_FLAG
        b"\x8c"  # I x0c|SHIFT_FLAG
        b"\x8d"  # J x0d|SHIFT_FLAG
        b"\x8e"  # K x0e|SHIFT_FLAG
        b"\x8f"  # L x0f|SHIFT_FLAG
        b"\x90"  # M x10|SHIFT_FLAG
        b"\x91"  # N x11|SHIFT_FLAG
        b"\x92"  # O x12|SHIFT_FLAG
        b"\x93"  # P x13|SHIFT_FLAG
        b"\x94"  # Q x14|SHIFT_FLAG
        b"\x95"  # R x15|SHIFT_FLAG
        b"\x96"  # S x16|SHIFT_FLAG
        b"\x97"  # T x17|SHIFT_FLAG
        b"\x98"  # U x18|SHIFT_FLAG
        b"\x99"  # V x19|SHIFT_FLAG
        b"\x9a"  # W x1a|SHIFT_FLAG
        b"\x9b"  # X x1b|SHIFT_FLAG
        b"\x9c"  # Y x1c|SHIFT_FLAG
        b"\x9d"  # Z x1d|SHIFT_FLAG
        b"\x2f"  # [
        b"\x31"  # \ backslash
        b"\x30"  # ]
        b"\xa3"  # ^ x23|SHIFT_FLAG (shift 6)
        b"\xad"  # _ x2d|SHIFT_FLAG (shift -)
        b"\x35"  # `
        b"\x04"  # a
        b"\x05"  # b
        b"\x06"  # c
        b"\x07"  # d
        b"\x08"  # e
        b"\x09"  # f
        b"\x0a"  # g
        b"\x0b"  # h
        b"\x0c"  # i
        b"\x0d"  # j
        b"\x0e"  # k
        b"\x0f"  # l
        b"\x10"  # m
        b"\x11"  # n
        b"\x12"  # o
        b"\x13"  # p
        b"\x14"  # q
        b"\x15"  # r
        b"\x16"  # s
        b"\x17"  # t
        b"\x18"  # u
        b"\x19"  # v
        b"\x1a"  # w
        b"\x1b"  # x
        b"\x1c"  # y
        b"\x1d"  # z
        b"\xaf"  # { x2f|SHIFT_FLAG (shift [)
        b"\xb1"  # | x31|SHIFT_FLAG (shift \)
        b"\xb0"  # } x30|SHIFT_FLAG (shift ])
        b"\xb5"  # ~ x35|SHIFT_FLAG (shift `)
        b"\x4c"  # DEL DELETE (called Forward Delete in usb.org document)
    )


KeyboardLayout = KeyboardLayoutUS
