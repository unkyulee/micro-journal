
# import necessary libraries
import board
import keypad

# Import USB HID libraries
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# import the keymap
from Keyboard.Layout import KEYMAPS

# Global status
from Services.Status import status

# Writer Deck Class
from WriterDeck.Writer import writer

class Keyboard:
    
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
            
            # Handle USB Keyboard events based on status mode        
            if status.data.mode == "writer":
                writer.handle_key_event(event.pressed, keycode)       
            else:
                self.handle_key_event(event.pressed, keycode)       
            
            
                
    #
    # Send out the key press as USB Keyboard
    #     
    def handle_key_event(self, pressed, keycode):
        if pressed:            
            self.kbd.press(keycode)
        else:                
            self.kbd.release(keycode)