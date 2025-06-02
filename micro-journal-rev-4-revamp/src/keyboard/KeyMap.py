from adafruit_hid.keycode import Keycode

KEYMAP = [
    # Index: 0
    {"default": Keycode.A, "alt_gr": Keycode.F1},
    # Index: 1
    {"default": Keycode.B, "alt_gr": Keycode.F2},
    # Index: 2
    {"default": Keycode.C, "alt_gr": Keycode.F3},
    # ...
    # Fill in the rest according to your keyboard layout
    # Example for index 68:
    {"default": Keycode.Z, "alt_gr": Keycode.F12},
]