# SPDX-FileCopyrightText: 2017 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`adafruit_hid.keyboard.Keyboard`
====================================================

* Author(s): Scott Shawcroft, Dan Halbert
"""

from micropython import const

from . import find_device
from .keycode import Keycode

try:
    from typing import Sequence

    import usb_hid
except ImportError:
    pass

_MAX_KEYPRESSES = const(6)


class Keyboard:
    """Send HID keyboard reports."""

    LED_NUM_LOCK = 0x01
    """LED Usage ID for Num Lock"""
    LED_CAPS_LOCK = 0x02
    """LED Usage ID for Caps Lock"""
    LED_SCROLL_LOCK = 0x04
    """LED Usage ID for Scroll Lock"""
    LED_COMPOSE = 0x08
    """LED Usage ID for Compose"""

    # No more than _MAX_KEYPRESSES regular keys may be pressed at once.

    def __init__(self, devices: Sequence[usb_hid.Device], timeout: int = None) -> None:
        """Create a Keyboard object that will send keyboard HID reports.

        :param timeout: Time in seconds to wait for USB to become ready before timing out.
          Defaults to None to wait indefinitely.

        Devices can be a sequence of devices that includes a keyboard device or a keyboard device
        itself. A device is any object that implements ``send_report()``, ``usage_page`` and
        ``usage``.
        """
        self._keyboard_device = find_device(devices, usage_page=0x1, usage=0x06, timeout=timeout)

        # Reuse this bytearray to send keyboard reports.
        self.report = bytearray(8)

        # report[0] modifiers
        # report[1] unused
        # report[2:8] regular key presses

        # View onto byte 0 in report.
        self.report_modifier = memoryview(self.report)[0:1]

        # List of regular keys currently pressed.
        # View onto bytes 2-7 in report.
        self.report_keys = memoryview(self.report)[2:]

        # No keyboard LEDs on.
        self._led_status = b"\x00"

    def press(self, *keycodes: int) -> None:
        """Send a report indicating that the given keys have been pressed.

        :param keycodes: Press these keycodes all at once.
        :raises ValueError: if more than six regular keys are pressed.

        Keycodes may be modifiers or regular keys.
        No more than six regular keys may be pressed simultaneously.

        Examples::

            from adafruit_hid.keycode import Keycode

            # Press ctrl-x.
            kbd.press(Keycode.LEFT_CONTROL, Keycode.X)

            # Or, more conveniently, use the CONTROL alias for LEFT_CONTROL:
            kbd.press(Keycode.CONTROL, Keycode.X)

            # Press a, b, c keys all at once.
            kbd.press(Keycode.A, Keycode.B, Keycode.C)
        """
        for keycode in keycodes:
            self._add_keycode_to_report(keycode)
        self._keyboard_device.send_report(self.report)

    def release(self, *keycodes: int) -> None:
        """Send a USB HID report indicating that the given keys have been released.

        :param keycodes: Release these keycodes all at once.

        If a keycode to be released was not pressed, it is ignored.

        Example::

            # release SHIFT key
            kbd.release(Keycode.SHIFT)
        """
        for keycode in keycodes:
            self._remove_keycode_from_report(keycode)
        self._keyboard_device.send_report(self.report)

    def release_all(self) -> None:
        """Release all pressed keys."""
        for i in range(8):
            self.report[i] = 0
        self._keyboard_device.send_report(self.report)

    def send(self, *keycodes: int) -> None:
        """Press the given keycodes and then release all pressed keys.

        :param keycodes: keycodes to send together
        """
        self.press(*keycodes)
        self.release_all()

    def _add_keycode_to_report(self, keycode: int) -> None:
        """Add a single keycode to the USB HID report."""
        modifier = Keycode.modifier_bit(keycode)
        if modifier:
            # Set bit for this modifier.
            self.report_modifier[0] |= modifier
        else:
            report_keys = self.report_keys
            # Don't press twice.
            for i in range(_MAX_KEYPRESSES):
                report_key = report_keys[i]
                if report_key == 0:
                    # Put keycode in first empty slot. Since the report_keys
                    # are compact and unique, this is not a repeated key
                    report_keys[i] = keycode
                    return
                if report_key == keycode:
                    # Already pressed.
                    return
            # All slots are filled. Shuffle down and reuse last slot
            for i in range(_MAX_KEYPRESSES - 1):
                report_keys[i] = report_keys[i + 1]
            report_keys[-1] = keycode

    def _remove_keycode_from_report(self, keycode: int) -> None:
        """Remove a single keycode from the report."""
        modifier = Keycode.modifier_bit(keycode)
        if modifier:
            # Turn off the bit for this modifier.
            self.report_modifier[0] &= ~modifier
        else:
            report_keys = self.report_keys
            # Clear the at most one matching slot and move remaining keys down
            j = 0
            for i in range(_MAX_KEYPRESSES):
                pressed = report_keys[i]
                if not pressed:
                    break  # Handled all used report slots
                if pressed == keycode:
                    continue  # Remove this entry
                if i != j:
                    report_keys[j] = report_keys[i]
                j += 1
            # Clear any remaining slots
            while j < _MAX_KEYPRESSES and report_keys[j]:
                report_keys[j] = 0
                j += 1

    @property
    def led_status(self) -> bytes:
        """Returns the last received report"""
        # get_last_received_report() returns None when nothing was received
        led_report = self._keyboard_device.get_last_received_report()
        if led_report is not None:
            self._led_status = led_report
        return self._led_status

    def led_on(self, led_code: int) -> bool:
        """Returns whether an LED is on based on the led code

        Examples::

            import usb_hid
            from adafruit_hid.keyboard import Keyboard
            from adafruit_hid.keycode import Keycode
            import time

            # Initialize Keyboard
            kbd = Keyboard(usb_hid.devices)

            # Press and release CapsLock.
            kbd.press(Keycode.CAPS_LOCK)
            time.sleep(.09)
            kbd.release(Keycode.CAPS_LOCK)

            # Check status of the LED_CAPS_LOCK
            print(kbd.led_on(Keyboard.LED_CAPS_LOCK))

        """
        return bool(self.led_status[0] & led_code)
