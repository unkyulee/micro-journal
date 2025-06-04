import board
import time
import busio
import displayio
import gc
import gifio
#
from adafruit_st7735r import ST7735R
# global status
from Services.Status import status

# Display Services
from Display.GIF import GIF
from Display.Menu import Menu   
from Display.Word import Word   
from Display.Timer import Timer

# Handles the display 
# In case of keyboard mode it will display GIF. 
# There can be cases of starting up, idle, typing, and sleep mode.
# During sleep mode it will turn off the backlight
class Display:
    # default screen
    screen = ""
    
    #
    def setup(self):      
        # Pins
        displayio.release_displays()
        
        self.clk = board.GP10
        self.mosi = board.GP11
        self.rst = board.GP12
        self.dc = board.GP8
        self.cs = board.GP9
        self.bl = board.GP25
        
        self.width = 160
        self.height = 80
        
        #
        self.spi = busio.SPI(clock=self.clk, MOSI=self.mosi)
        
        #
        self.display_bus = displayio.FourWire(
            self.spi, command=self.dc, chip_select=self.cs, reset=self.rst)
        
        #
        self.display = ST7735R( 
            self.display_bus, 
            width=self.width, 
            height=self.height,
            rotation=90,
            rowstart=1,
            colstart=26,
            invert=True,
            backlight_pin=self.bl)
        
        # set default brightness
        self.display.brightness = status.data["display"]["brightness"]
       
        # Display Services
        self.gif = GIF()
        self.menu = Menu()
        self.word = Word(self.display)
        self.timer = Timer()
        
        
    #
    def loop(self): 
        
        # Define display modes
        mode = status.data["mode"]
        screen = status.data["display"]["screen"]
        
        # print(f"Display loop: mode={mode}, screen={screen} prev={self.screen}")
                                       
        if mode == "keyboard":
            # If USB Keyboard mode then display GIF
            screen = "gif"            
                        
            if self.screen != screen: self.gif.setup()
            # If USB Keyboard mode then display GIF
            self.gif.loop()

        elif mode == "writer":    
            #            
            if screen == "menu":
                if self.screen != screen: self.menu.setup()
                # If menu mode then display menu
                self.menu.loop()
            elif screen == "timer":
                if self.screen != screen: self.timer.setup()
                # If timer mode then display timer
                self.timer.loop()
            else:
                if self.screen != "word" or self.screen != screen: self.word.setup()
                # by default WriterDeck mode
                screen = "word"
                
                # If WriterDeck mode then display text
                self.word.loop()
       
                
                
        # save prev screen mode
        status.data["display"]["screen"] = screen
        self.screen = screen
        

