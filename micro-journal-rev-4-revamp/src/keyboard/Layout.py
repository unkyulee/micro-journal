
import board
import keypad

import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode


from keyboard.KeyMap import KEYMAPS


class KeyboardLayout:
    
    # Default layer
    layer = 0 
    
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
        # Retrieve Keybaord Event
        event = self.km.events.get()
        
        
        # If any key events
        if event:
            # Get key index from event
            key_index = event.key_number
            
            # get keycode from KEYMAP
            if key_index < len(KEYMAPS[self.layer]):
                keycode = KEYMAPS[self.layer][key_index]
            else:
                # If key index is out of bounds, ignore the event
                return

            # if key is layer switch then add the layer
            if keycode == Keycode.F24 and event.pressed:
                self.layer = 1
                return
            elif keycode == Keycode.F24 and event.released:
                self.layer = 0  
                return
            
            # Debug keycode
            # print(f"Key {key_index} pressed: {keycode} (Layer: {self.layer})")
            
            # If key is pressed
            if event.pressed:
                # Send the keycode
                self.kbd.press(keycode)
            else:
                # Release the keycode
                self.kbd.release(keycode)