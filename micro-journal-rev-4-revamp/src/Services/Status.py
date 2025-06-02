class Status:
    # Define global context
    data = {
        'mode': 'keyboard',        
        'display': {          
            'screen': 'gif',
            'brightness': 0.3
        }        
    }
        
    #
    def setup(self):      
        # TBD
        # read the configuaration file
        # load last backlight level setup        
        
        #
        self.text = ""
        
    #
    def loop(self):         
        # read the battery status 
        
        #
        self.text = ""


# Global instance
status = Status()