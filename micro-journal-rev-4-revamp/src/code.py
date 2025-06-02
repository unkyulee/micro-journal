import asyncio
import time

# Service Libraries
from Keyboard.Keyboard import Keyboard
from WriterDeck.Writer import Writer
from Display.Display import Display
from Services.Status import Status

# Keyboard Loop
async def keyboard_loop():
    # loop interval: 10 ms
    interval = 0.01
   
    # Setup Keyboard Layout
    keyboard = Keyboard()
    keyboard.setup()
    
    # Task Loop
    while True:
        # Loop Measure Interval - Begin
        start_time = time.monotonic()
        
        # Code
        keyboard.loop()
        
        # Loop Measure Interval - End
        elapsed_time = time.monotonic() - start_time
        sleep_time = max(0, interval - elapsed_time)
        await asyncio.sleep(sleep_time)
       

# WriterDeck Loop
async def writer_loop():
    # loop interval: 1000 ms
    interval = 1
   
    # Setup Keyboard Layout
    writer = Writer()
    writer.setup()
    
    # Task Loop
    while True:
        # Loop Measure Interval - Begin
        start_time = time.monotonic()
        
        # Code
        writer.loop()
        
        # Loop Measure Interval - End
        elapsed_time = time.monotonic() - start_time
        sleep_time = max(0, interval - elapsed_time)
        await asyncio.sleep(sleep_time)



# Display Loop
async def display_loop():
    # loop interval: 50 ms, 20 FPS
    interval = 0.05
   
    # Setup Keyboard Layout
    display = Display()
    display.setup()
    
    # Task Loop
    while True:
        # Loop Measure Interval - Begin
        start_time = time.monotonic()
        
        # Code
        display.loop()
        
        # Loop Measure Interval - End
        elapsed_time = time.monotonic() - start_time
        sleep_time = max(0, interval - elapsed_time)
        await asyncio.sleep(sleep_time)


# Global Status Loop 
# Battery status, and other house keeping tasks can be added here.
async def global_status_loop():
    # loop interval: 60 seconds
    interval = 60
    
    while True:
        # Loop Measure Interval - Begin
        start_time = time.monotonic()
        
        # Code for global status updates can be added here
        
        # Loop Measure Interval - End
        elapsed_time = time.monotonic() - start_time
        sleep_time = max(0, interval - elapsed_time)
        await asyncio.sleep(sleep_time)
        


# Main function to run tasks concurrently
async def main():
    task1 = asyncio.create_task(keyboard_loop())
    task2 = asyncio.create_task(writer_loop())
    task3 = asyncio.create_task(display_loop())
    task4 = asyncio.create_task(global_status_loop())
    
    await asyncio.gather(task1, task2, task3, task4)


# Run the main function
asyncio.run(main())