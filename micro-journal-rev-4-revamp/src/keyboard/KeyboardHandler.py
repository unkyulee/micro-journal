
# import necessary libraries
import board
import keypad

# Import USB HID libraries
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# import the keymap
from Keyboard.USBKeyboardLayout import USB_KEYBOARD_KEYMAPS
from Keyboard.WriterDeckKeyboardLayout import WRITER_DECK_KEYMAPS

# Global status
from Services.Status import status

# Writer Deck Class
from Writer.Writer import writer

class KeyboardHandler:
    
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
            
            # Debug keycode
            # print(f"Key {key_index} pressed: {keycode} (Layer: {self.layer})")
            
            # Handle USB Keyboard events based on status mode        
            if status.data["mode"] == "writer":
                self.handle_writerDeck_keyboard_event(event.pressed, key_index)       
            else:
                self.handle_usb_keyboard_event(event.pressed, key_index)       
            
            
                
    #
    # Send out the key press as USB Keyboard
    #     
    layer = 0  # Default layer
    def handle_usb_keyboard_event(self, pressed, key_index):
        # get keycode from KEYMAP
        if key_index < len(USB_KEYBOARD_KEYMAPS[self.layer]):
            keycode = USB_KEYBOARD_KEYMAPS[self.layer][key_index]
        else:
            # If key index is out of bounds, ignore the event
            return

        # if key is layer switch then add the layer
        if keycode == Keycode.F24 and pressed:
            self.layer = 1
            return
        elif keycode == Keycode.F24 and pressed == False:
            self.layer = 0  
            return
        
        if pressed:            
            self.kbd.press(keycode)
        else:                
            self.kbd.release(keycode)
            
            
    #
    # Map WriterDeck key events 
    #  
    shift = False
    alt = False
    #
    def handle_writerDeck_keyboard_event(self, pressed, key_index):
        # Check Shift and Alt keys
        keycode = WRITER_DECK_KEYMAPS[0][key_index]
        if keycode == 14 and pressed:
            self.shift = True
        elif keycode == 14 and not pressed:
            self.shift = False
        
        if keycode == 17:
            self.alt = True
        elif keycode == 17 and not pressed:
            self.alt = False
            
            
        # Determine the layer based on Shift and Alt keys
        layer = 0
        if self.alt: layer = 2
        if self.shift: layer += 1
                       
        # get keycode from KEYMAP
        if key_index < len(WRITER_DECK_KEYMAPS[layer]):
            keycode = WRITER_DECK_KEYMAPS[layer][key_index]
        else:
            # If key index is out of bounds, ignore the event
            return
        
        if pressed:            
            writer.handle_key_event(pressed, keycode)