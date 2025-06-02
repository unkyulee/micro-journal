# SPDX-FileCopyrightText: 2017 Scott Shawcroft for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`adafruit_hid`
====================================================

This driver simulates USB HID devices.

* Author(s): Scott Shawcroft, Dan Halbert
"""

# imports
from __future__ import annotations

import time

try:
    import supervisor
except ImportError:
    supervisor = None

try:
    from typing import Sequence
except ImportError:
    pass

# usb_hid may not exist on some boards that still provide BLE or other HID devices.
try:
    from usb_hid import Device
except ImportError:
    Device = None

__version__ = "0.0.0+auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_HID.git"


def find_device(
    devices: Sequence[object],
    *,
    usage_page: int,
    usage: int,
    timeout: int = None,
) -> object:
    """
    Search through the provided sequence of devices to find the one with the matching
    usage_page and usage.

    :param timeout: Time in seconds to wait for USB to become ready before timing out.
      Defaults to None to wait indefinitely.
      Ignored if device is not a `usb_hid.Device`; it might be BLE, for instance.
    """

    if hasattr(devices, "send_report"):
        devices = [devices]  # type: ignore
    device = None
    for dev in devices:
        if dev.usage_page == usage_page and dev.usage == usage and hasattr(dev, "send_report"):
            device = dev
            break
    if device is None:
        raise ValueError("Could not find matching HID device.")

    # Wait for USB to be connected only if this is a usb_hid.Device.
    if Device and isinstance(device, Device):
        if supervisor is None:
            # Blinka doesn't have supervisor (see issue Adafruit_Blinka#711), so wait
            # one second for USB to become ready
            time.sleep(1.0)
        elif timeout is None:
            # default behavior: wait indefinitely for USB to become ready
            while not supervisor.runtime.usb_connected:
                time.sleep(1.0)
        else:
            # wait up to timeout seconds for USB to become ready
            for _ in range(timeout):
                if supervisor.runtime.usb_connected:
                    return device
                time.sleep(1.0)
            raise OSError("Failed to initialize HID device. Is USB connected?")

    return device
