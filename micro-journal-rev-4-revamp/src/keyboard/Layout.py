
import board
import keypad

import usb_hid
from adafruit_hid.keyboard import Keyboard

from keyboard.KeyMap import KEYMAP


class KeyboardLayout:
    #
    def setup(self):      
        # USB HID Setup
        self.kbd = Keyboard(usb_hid.devices)
          
        # Key Matrix Setup
        self.km = keypad.KeyMatrix(
            row_pins=(board.GP0, board.GP1, board.GP2, board.GP3, board.GP4, board.GP5, board.GP6, board.GP7),
            column_pins=(board.GP13, board.GP14, board.GP15, board.GP16, board.GP17, board.GP18, board.GP19, board.GP20, board.GP21),
            columns_to_anodes=False,
        )

        
    #
    def loop(self): 
        event = self.km.events.get()
        
        if event and event.pressed:
            key_index = event.key_number
            if 0 <= key_index < len(KEYMAP):
                keycode = KEYMAP[key_index].get("default")
                if keycode:
                    self.kbd.send(keycode)
        if event and event.released:
            key_index = event.key_number
            if 0 <= key_index < len(KEYMAP):
                keycode = KEYMAP[key_index].get("default")
                if keycode:
                    self.kbd.send(keycode)
        