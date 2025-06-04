import board
import digitalio
import microcontroller

class Status:
    # Define global context
    data = {
        # keyboard, writer
        'mode': 'writer',        
        'display': {      
            # gif, menu, timer, word    
            'screen': 'gif',
            'brightness': 0.3
        }
    }
        
    #
    def setup(self):      
        # TBD
        # read the configuaration file
        # load last backlight level setup        
        
        # Setup Writer and Keyboard Switch
        self.switch = digitalio.DigitalInOut(board.GP27)

        self.switch.direction = digitalio.Direction.INPUT
        self.switch.pull = digitalio.Pull.UP
        
        # save the initial switch value
        self.switch_prev_value = self.switch.value
        
        # switch value is True when the switch is not pressed: Keyboard mode
        if self.switch.value:
            # Keyboard mode
            # self.data['mode'] = 'keyboard'
            self.data['mode'] = 'writer'
        else:
            # Writer mode
            self.data['mode'] = 'writer'    
        
         
    #
    def loop(self):         
        # self.switch.value is True when the switch is not pressed: Keyboard mode
        # self.switch.value is False when the switch is pressed: Writer mode
        if self.switch.value != self.switch_prev_value:
            # Restart the device
            microcontroller.reset()
        


# Global instance
status = Status()