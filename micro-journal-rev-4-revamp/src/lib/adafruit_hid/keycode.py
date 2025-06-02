# SPDX-FileCopyrightText: 2017 Scott Shawcroft for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`adafruit_hid.keycode.Keycode`
====================================================

* Author(s): Scott Shawcroft, Dan Halbert
"""


class Keycode:
    """USB HID Keycode constants.

    This list is modeled after the names for USB keycodes defined in
    https://usb.org/sites/default/files/hut1_21_0.pdf#page=83.
    This list does not include every single code, but does include all the keys on
    a regular PC or Mac keyboard.

    Remember that keycodes are the names for key *positions* on a US keyboard, and may
    not correspond to the character that you mean to send if you want to emulate non-US keyboard.
    For instance, on a French keyboard (AZERTY instead of QWERTY),
    the keycode for 'q' is used to indicate an 'a'. Likewise, 'y' represents 'z' on
    a German keyboard. This is historical: the idea was that the keycaps could be changed
    without changing the keycodes sent, so that different firmware was not needed for
    different variations of a keyboard.
    """

    A = 0x04
    """``a`` and ``A``"""
    B = 0x05
    """``b`` and ``B``"""
    C = 0x06
    """``c`` and ``C``"""
    D = 0x07
    """``d`` and ``D``"""
    E = 0x08
    """``e`` and ``E``"""
    F = 0x09
    """``f`` and ``F``"""
    G = 0x0A
    """``g`` and ``G``"""
    H = 0x0B
    """``h`` and ``H``"""
    I = 0x0C
    """``i`` and ``I``"""
    J = 0x0D
    """``j`` and ``J``"""
    K = 0x0E
    """``k`` and ``K``"""
    L = 0x0F
    """``l`` and ``L``"""
    M = 0x10
    """``m`` and ``M``"""
    N = 0x11
    """``n`` and ``N``"""
    O = 0x12
    """``o`` and ``O``"""
    P = 0x13
    """``p`` and ``P``"""
    Q = 0x14
    """``q`` and ``Q``"""
    R = 0x15
    """``r`` and ``R``"""
    S = 0x16
    """``s`` and ``S``"""
    T = 0x17
    """``t`` and ``T``"""
    U = 0x18
    """``u`` and ``U``"""
    V = 0x19
    """``v`` and ``V``"""
    W = 0x1A
    """``w`` and ``W``"""
    X = 0x1B
    """``x`` and ``X``"""
    Y = 0x1C
    """``y`` and ``Y``"""
    Z = 0x1D
    """``z`` and ``Z``"""

    ONE = 0x1E
    """``1`` and ``!``"""
    TWO = 0x1F
    """``2`` and ``@``"""
    THREE = 0x20
    """``3`` and ``#``"""
    FOUR = 0x21
    """``4`` and ``$``"""
    FIVE = 0x22
    """``5`` and ``%``"""
    SIX = 0x23
    """``6`` and ``^``"""
    SEVEN = 0x24
    """``7`` and ``&``"""
    EIGHT = 0x25
    """``8`` and ``*``"""
    NINE = 0x26
    """``9`` and ``(``"""
    ZERO = 0x27
    """``0`` and ``)``"""
    ENTER = 0x28
    """Enter (Return)"""
    RETURN = ENTER
    """Alias for ``ENTER``"""
    ESCAPE = 0x29
    """Escape"""
    BACKSPACE = 0x2A
    """Delete backward (Backspace)"""
    TAB = 0x2B
    """Tab and Backtab"""
    SPACEBAR = 0x2C
    """Spacebar"""
    SPACE = SPACEBAR
    """Alias for SPACEBAR"""
    MINUS = 0x2D
    """``-` and ``_``"""
    EQUALS = 0x2E
    """``=` and ``+``"""
    LEFT_BRACKET = 0x2F
    """``[`` and ``{``"""
    RIGHT_BRACKET = 0x30
    """``]`` and ``}``"""
    BACKSLASH = 0x31
    r"""``\`` and ``|``"""
    POUND = 0x32
    """``#`` and ``~`` (Non-US keyboard)"""
    SEMICOLON = 0x33
    """``;`` and ``:``"""
    QUOTE = 0x34
    """``'`` and ``"``"""
    GRAVE_ACCENT = 0x35
    r""":literal:`\`` and ``~``"""
    COMMA = 0x36
    """``,`` and ``<``"""
    PERIOD = 0x37
    """``.`` and ``>``"""
    FORWARD_SLASH = 0x38
    """``/`` and ``?``"""

    CAPS_LOCK = 0x39
    """Caps Lock"""

    F1 = 0x3A
    """Function key F1"""
    F2 = 0x3B
    """Function key F2"""
    F3 = 0x3C
    """Function key F3"""
    F4 = 0x3D
    """Function key F4"""
    F5 = 0x3E
    """Function key F5"""
    F6 = 0x3F
    """Function key F6"""
    F7 = 0x40
    """Function key F7"""
    F8 = 0x41
    """Function key F8"""
    F9 = 0x42
    """Function key F9"""
    F10 = 0x43
    """Function key F10"""
    F11 = 0x44
    """Function key F11"""
    F12 = 0x45
    """Function key F12"""

    PRINT_SCREEN = 0x46
    """Print Screen (SysRq)"""
    SCROLL_LOCK = 0x47
    """Scroll Lock"""
    PAUSE = 0x48
    """Pause (Break)"""

    INSERT = 0x49
    """Insert"""
    HOME = 0x4A
    """Home (often moves to beginning of line)"""
    PAGE_UP = 0x4B
    """Go back one page"""
    DELETE = 0x4C
    """Delete forward"""
    END = 0x4D
    """End (often moves to end of line)"""
    PAGE_DOWN = 0x4E
    """Go forward one page"""

    RIGHT_ARROW = 0x4F
    """Move the cursor right"""
    LEFT_ARROW = 0x50
    """Move the cursor left"""
    DOWN_ARROW = 0x51
    """Move the cursor down"""
    UP_ARROW = 0x52
    """Move the cursor up"""

    KEYPAD_NUMLOCK = 0x53
    """Num Lock (Clear on Mac)"""
    KEYPAD_FORWARD_SLASH = 0x54
    """Keypad ``/``"""
    KEYPAD_ASTERISK = 0x55
    """Keypad ``*``"""
    KEYPAD_MINUS = 0x56
    """Keyapd ``-``"""
    KEYPAD_PLUS = 0x57
    """Keypad ``+``"""
    KEYPAD_ENTER = 0x58
    """Keypad Enter"""
    KEYPAD_ONE = 0x59
    """Keypad ``1`` and End"""
    KEYPAD_TWO = 0x5A
    """Keypad ``2`` and Down Arrow"""
    KEYPAD_THREE = 0x5B
    """Keypad ``3`` and PgDn"""
    KEYPAD_FOUR = 0x5C
    """Keypad ``4`` and Left Arrow"""
    KEYPAD_FIVE = 0x5D
    """Keypad ``5``"""
    KEYPAD_SIX = 0x5E
    """Keypad ``6`` and Right Arrow"""
    KEYPAD_SEVEN = 0x5F
    """Keypad ``7`` and Home"""
    KEYPAD_EIGHT = 0x60
    """Keypad ``8`` and Up Arrow"""
    KEYPAD_NINE = 0x61
    """Keypad ``9`` and PgUp"""
    KEYPAD_ZERO = 0x62
    """Keypad ``0`` and Ins"""
    KEYPAD_PERIOD = 0x63
    """Keypad ``.`` and Del"""
    KEYPAD_BACKSLASH = 0x64
    """Keypad ``\\`` and ``|`` (Non-US)"""

    APPLICATION = 0x65
    """Application: also known as the Menu key (Windows)"""
    POWER = 0x66
    """Power (Mac)"""
    KEYPAD_EQUALS = 0x67
    """Keypad ``=`` (Mac)"""
    F13 = 0x68
    """Function key F13 (Mac)"""
    F14 = 0x69
    """Function key F14 (Mac)"""
    F15 = 0x6A
    """Function key F15 (Mac)"""
    F16 = 0x6B
    """Function key F16 (Mac)"""
    F17 = 0x6C
    """Function key F17 (Mac)"""
    F18 = 0x6D
    """Function key F18 (Mac)"""
    F19 = 0x6E
    """Function key F19 (Mac)"""

    F20 = 0x6F
    """Function key F20"""
    F21 = 0x70
    """Function key F21"""
    F22 = 0x71
    """Function key F22"""
    F23 = 0x72
    """Function key F23"""
    F24 = 0x73
    """Function key F24"""

    LEFT_CONTROL = 0xE0
    """Control modifier left of the spacebar"""
    CONTROL = LEFT_CONTROL
    """Alias for LEFT_CONTROL"""
    LEFT_SHIFT = 0xE1
    """Shift modifier left of the spacebar"""
    SHIFT = LEFT_SHIFT
    """Alias for LEFT_SHIFT"""
    LEFT_ALT = 0xE2
    """Alt modifier left of the spacebar"""
    ALT = LEFT_ALT
    """Alias for LEFT_ALT; Alt is also known as Option (Mac)"""
    OPTION = ALT
    """Labeled as Option on some Mac keyboards"""
    LEFT_GUI = 0xE3
    """GUI modifier left of the spacebar"""
    GUI = LEFT_GUI
    """Alias for LEFT_GUI; GUI is also known as the Windows key, Command (Mac), or Meta"""
    WINDOWS = GUI
    """Labeled with a Windows logo on Windows keyboards"""
    COMMAND = GUI
    """Labeled as Command on Mac keyboards, with a clover glyph"""
    RIGHT_CONTROL = 0xE4
    """Control modifier right of the spacebar"""
    RIGHT_SHIFT = 0xE5
    """Shift modifier right of the spacebar"""
    RIGHT_ALT = 0xE6
    """Alt modifier right of the spacebar"""
    RIGHT_GUI = 0xE7
    """GUI modifier right of the spacebar"""

    @classmethod
    def modifier_bit(cls, keycode: int) -> int:
        """Return the modifer bit to be set in an HID keycode report if this is a
        modifier key; otherwise return 0."""
        return 1 << (keycode - 0xE0) if cls.LEFT_CONTROL <= keycode <= cls.RIGHT_GUI else 0
