# SPDX-FileCopyrightText: 2017 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`adafruit_hid.keyboard_layout_base.KeyboardLayoutBase`
=======================================================

* Author(s): Dan Halbert, AngainorDev, Neradoc
"""

try:
    from typing import Tuple

    from .keyboard import Keyboard
except ImportError:
    pass

from time import sleep

__version__ = "0.0.0+auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_HID.git"


class KeyboardLayoutBase:
    """Base class for keyboard layouts. Uses the tables defined in the subclass
    to map UTF-8 characters to appropriate keypresses.

    Non-supported characters and most control characters will raise an exception.
    """

    SHIFT_FLAG = 0x80
    """Bit set in any keycode byte if the shift key is required for the character."""
    ALTGR_FLAG = 0x80
    """Bit set in the combined keys table if altgr is required for the first key."""
    SHIFT_CODE = 0xE1
    """The SHIFT keycode, to avoid dependency to the Keycode class."""
    RIGHT_ALT_CODE = 0xE6
    """The ALTGR keycode, to avoid dependency to the Keycode class."""
    ASCII_TO_KEYCODE = ()
    """Bytes string of keycodes for low ASCII characters, indexed by the ASCII value.
    Keycodes use the `SHIFT_FLAG` if needed.
    Dead keys are excluded by assigning the keycode 0."""
    HIGHER_ASCII = {}
    """Dictionary that associates the ord() int value of high ascii and utf8 characters
    to their keycode. Keycodes use the `SHIFT_FLAG` if needed."""
    NEED_ALTGR = ""
    """Characters in `ASCII_TO_KEYCODE` and `HIGHER_ASCII` that need
    the ALTGR key pressed to type."""
    COMBINED_KEYS = {}
    """
    Dictionary of characters (indexed by ord() value) that can be accessed by typing first
    a dead key followed by a regular key, like ``ñ`` as ``~ + n``. The value is a 2-bytes int:
    the high byte is the dead-key keycode (including SHIFT_FLAG), the low byte is the ascii code
    of the second character, with ALTGR_FLAG set if the dead key (the first key) needs ALTGR.

    The combined-key codes bits are: ``0b SDDD DDDD AKKK KKKK``:
    ``S`` is the shift flag for the **first** key,
    ``DDD DDDD`` is the keycode for the **first** key,
    ``A`` is the altgr flag for the **first** key,
    ``KKK KKKK`` is the (low) ASCII code for the second character.
    """

    def __init__(self, keyboard: Keyboard) -> None:
        """Specify the layout for the given keyboard.

        :param keyboard: a Keyboard object. Write characters to this keyboard when requested.

        Example::

            kbd = Keyboard(usb_hid.devices)
            layout = KeyboardLayout(kbd)
        """
        self.keyboard = keyboard

    def _write(self, keycode: int, altgr: bool = False) -> None:
        """Type a key combination based on shift bit and altgr bool

        :param keycode: int value of the keycode, with the shift bit.
        :param altgr: bool indicating if the altgr key should be pressed too.
        """
        # Add altgr modifier if needed
        if altgr:
            self.keyboard.press(self.RIGHT_ALT_CODE)
        # If this is a shifted char, clear the SHIFT flag and press the SHIFT key.
        if keycode & self.SHIFT_FLAG:
            keycode &= ~self.SHIFT_FLAG
            self.keyboard.press(self.SHIFT_CODE)
        self.keyboard.press(keycode)
        self.keyboard.release_all()

    def write(self, string: str, delay: float = None) -> None:
        """Type the string by pressing and releasing keys on my keyboard.

        :param string: A string of UTF-8 characters to convert to key presses and send.
        :param float delay: Optional delay in seconds between key presses.
        :raises ValueError: if any of the characters has no keycode
            (such as some control characters).

        Example::

            # Write abc followed by Enter to the keyboard
            layout.write('abc\\n')
        """
        for char in string:
            # find easy ones first
            keycode = self._char_to_keycode(char)
            if keycode > 0:
                self._write(keycode, char in self.NEED_ALTGR)
            # find combined keys
            elif ord(char) in self.COMBINED_KEYS:
                # first key (including shift bit)
                cchar = self.COMBINED_KEYS[ord(char)]
                self._write(cchar >> 8, cchar & self.ALTGR_FLAG)
                # second key (removing the altgr bit)
                char = chr(cchar & 0xFF & (~self.ALTGR_FLAG))
                keycode = self._char_to_keycode(char)
                # assume no altgr needed for second key
                self._write(keycode, False)
            else:
                raise ValueError(
                    "No keycode available for character {letter} ({num}/0x{num:02x}).".format(
                        letter=repr(char), num=ord(char)
                    )
                )

            if delay is not None:
                sleep(delay)

    def keycodes(self, char: str) -> Tuple[int, ...]:
        """Return a tuple of keycodes needed to type the given character.

        :param char: A single UTF8 character in a string.
        :type char: str of length one.
        :returns: tuple of Keycode keycodes.
        :raises ValueError: if there is no keycode for ``char``.

        Examples::

            # Returns (Keycode.TAB,)
            keycodes('\t')
            # Returns (Keycode.A,)
            keycode('a')
            # Returns (Keycode.SHIFT, Keycode.A)
            keycode('A')
            # Raises ValueError with a US layout because it's an unknown character
            keycode('é')
        """
        keycode = self._char_to_keycode(char)
        if keycode == 0:
            raise ValueError(
                "No keycode available for character {letter} ({num}/0x{num:02x}).".format(
                    letter=repr(char), num=ord(char)
                )
            )

        codes = []
        if char in self.NEED_ALTGR:
            codes.append(self.RIGHT_ALT_CODE)
        if keycode & self.SHIFT_FLAG:
            codes.extend((self.SHIFT_CODE, keycode & ~self.SHIFT_FLAG))
        else:
            codes.append(keycode)

        return codes

    def _above128char_to_keycode(self, char: str) -> int:
        """Return keycode for above 128 utf8 codes.

        A character can be indexed by the char itself or its int ord() value.

        :param char_val: char value
        :return: keycode, with modifiers if needed
        """
        if ord(char) in self.HIGHER_ASCII:
            return self.HIGHER_ASCII[ord(char)]
        if char in self.HIGHER_ASCII:
            return self.HIGHER_ASCII[char]
        return 0

    def _char_to_keycode(self, char: str) -> int:
        """Return the HID keycode for the given character, with the SHIFT_FLAG possibly set.

        If the character requires pressing the Shift key, the SHIFT_FLAG bit is set.
        You must clear this bit before passing the keycode in a USB report.
        """
        char_val = ord(char)
        if char_val > len(self.ASCII_TO_KEYCODE):
            return self._above128char_to_keycode(char)
        keycode = self.ASCII_TO_KEYCODE[char_val]
        return keycode
