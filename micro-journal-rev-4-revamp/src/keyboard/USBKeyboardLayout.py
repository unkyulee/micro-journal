from adafruit_hid.keycode import Keycode

USB_KEYBOARD_KEYMAPS = [
    [
        # Layer 0 (Default)
        Keycode.ESCAPE, Keycode.ONE, Keycode.TWO, Keycode.THREE, Keycode.FOUR, Keycode.FIVE, Keycode.SIX, Keycode.SEVEN, Keycode.EIGHT, Keycode.NINE, Keycode.ZERO, Keycode.MINUS, Keycode.EQUALS, Keycode.BACKSPACE, Keycode.DELETE,
        Keycode.TAB, Keycode.Q, Keycode.W, Keycode.E, Keycode.R, Keycode.T, Keycode.Y, Keycode.U, Keycode.I, Keycode.O, Keycode.P, Keycode.LEFT_BRACKET, Keycode.RIGHT_BRACKET, Keycode.ENTER, Keycode.PAGE_UP,
        Keycode.CAPS_LOCK, Keycode.A, Keycode.S, Keycode.D, Keycode.F, Keycode.G, Keycode.H, Keycode.J, Keycode.K, Keycode.L, Keycode.SEMICOLON, Keycode.QUOTE, Keycode.BACKSLASH, Keycode.PAGE_DOWN,
        Keycode.LEFT_SHIFT, Keycode.GRAVE_ACCENT, Keycode.Z, Keycode.X, Keycode.C, Keycode.V, Keycode.B, Keycode.N, Keycode.M, Keycode.COMMA, Keycode.PERIOD, Keycode.FORWARD_SLASH, Keycode.RIGHT_SHIFT, Keycode.UP_ARROW, Keycode.END,
        Keycode.LEFT_CONTROL, Keycode.LEFT_GUI, Keycode.LEFT_ALT, Keycode.SPACE, Keycode.RIGHT_ALT, Keycode.F24, Keycode.HOME, Keycode.LEFT_ARROW, Keycode.DOWN_ARROW, Keycode.RIGHT_ARROW,
    ],
    [
        # Layer 1 (FN0)
        Keycode.GRAVE_ACCENT, Keycode.F1, Keycode.F2, Keycode.F3, Keycode.F4, Keycode.F5, Keycode.F6, Keycode.F7, Keycode.F8, Keycode.F9, Keycode.F10, Keycode.F11, Keycode.F12, Keycode.BACKSPACE, Keycode.DELETE,
        Keycode.TAB, Keycode.Q, Keycode.W, Keycode.E, Keycode.R, Keycode.T, Keycode.Y, Keycode.U, Keycode.I, Keycode.O, Keycode.P, Keycode.LEFT_BRACKET, Keycode.RIGHT_BRACKET, Keycode.ENTER, Keycode.PRINT_SCREEN,
        Keycode.CAPS_LOCK, Keycode.A, Keycode.S, Keycode.D, Keycode.F, Keycode.G, Keycode.H, Keycode.J, Keycode.K, Keycode.L, Keycode.SEMICOLON, Keycode.QUOTE, Keycode.BACKSLASH, Keycode.PAGE_DOWN,
        Keycode.LEFT_SHIFT, Keycode.GRAVE_ACCENT, Keycode.Z, Keycode.X, Keycode.C, Keycode.V, Keycode.B, Keycode.N, Keycode.M, Keycode.COMMA, Keycode.PERIOD, Keycode.FORWARD_SLASH, Keycode.RIGHT_SHIFT, Keycode.UP_ARROW, Keycode.END,
        Keycode.LEFT_CONTROL, Keycode.LEFT_GUI, Keycode.LEFT_ALT, Keycode.SPACE, Keycode.RIGHT_ALT, Keycode.F24, Keycode.HOME, Keycode.LEFT_ARROW, Keycode.DOWN_ARROW, Keycode.RIGHT_ARROW,
    ]
]
