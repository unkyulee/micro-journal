import asyncio
import time

from keyboard.Layout import KeyboardLayout

# Keyboard Loop
async def keyboard_loop():
    # loop interval: 10 ms
    interval = 0.01
   
    # Setup Keyboard Layout
    keyboardLayout = KeyboardLayout()
    keyboardLayout.setup()
    
    # Task Loop
    while True:
        # Loop Measure Interval - Begin
        start_time = time.monotonic()
        
        # Code
        keyboardLayout.loop()
        
        # Loop Measure Interval - End
        elapsed_time = time.monotonic() - start_time
        sleep_time = max(0, interval - elapsed_time)
        await asyncio.sleep(sleep_time)
       


# Main function to run tasks concurrently
async def main():
    task1 = asyncio.create_task(keyboard_loop())
    
    await asyncio.gather(task1)


# Run the main function
asyncio.run(main())