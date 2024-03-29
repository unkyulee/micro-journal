# Micro Journal: Distraction-Free Writing Device: WriterDeck

I aimed to create a specialized writing device, focusing on the tactile experience of mechanical keyboards. This device would be portable, with a compact form factor, designed to enhance the writing process in a focused environment. The goal was to provide a distraction-free writing experience, allowing users to immerse themselves fully in their creative work.


## First iteration: Using Raspberry Pi and 30 Keys mechanical keyboards. 

I experimented with various iterations to explore different options for creating a distraction-free writing device. The initial version involved using a Raspberry Pi 2W, a small Linux computer, paired with a 30-key mechanical keyboard.

![Micro Journal Rev.2](/micro-journal-rev-2-raspberypi/images/rev2_6.png)

A crucial aspect of achieving distraction-free writing is to eliminate all possible distractions. The Raspberry Pi 2W excelled in this regard due to its Linux terminal-based interface, devoid of any graphical user interface. This meant there were no avenues for distractions like YouTube or other similar distractions.

Upon powering on the device, I utilized the midnight commander to navigate to the desired folder and used the "micro" editor (similar to nano or vi) to begin writing. The screen transformed into a simple black-and-white display, solely focused on the text being written. This immersive environment facilitated quick entry into the flow state, resulting in remarkable productivity and quality of writing. When the brain is compelled to focus solely on one task, the results can be truly impressive.

This is the write up for the build guide of ths raspberry pi build.

[Micro Journal Rev.2 Build Details](/micro-journal-rev-2-raspberypi/readme.md) 

[Reddit Post](https://new.reddit.com/r/writerDeck/comments/1b6ff16/micro_journal_rev_2/)

## Second iteration: Using an old phone to make a writerDeck

In the next iteration, I explored building a writing device using easily accessible components, such as an old phone. Old phones still possess sufficient power to serve as effective writing tools when coupled with a keyboard.

![Micro Journal Rev.3](/micro-journal-rev-3-samsung-galaxy-s8/images/001.jpg)

The primary focus of this build was to provide a platform for my daughter to write journals without the complexities associated with Linux commands. Since she was accustomed to tablet environments, this solution aligned well with her needs. She could simply pick up the device and begin writing.

This particular build involved 3D prints and hand-wired keyboards, making it slightly more complex. However, the core idea of combining an old phone with a keyboard can be easily replicated for similar purposes.

It's important to note that achieving a distraction-free environment was challenging with this build. Phones are inherently versatile devices, offering access to various forms of entertainment like YouTube and music streaming services. Consequently, maintaining focus on writing proved difficult. Despite this limitation, the device still served its purpose well by assisting my daughter in continuing her journal writing.

Here are build guides:

[Micro Journal Rev.3 Build Details](/micro-journal-rev-3-samsung-galaxy-s8/readme.md)

[Reddit Post](https://new.reddit.com/r/writerDeck/comments/1bfhnfz/writerdeck_with_samsung_s8_using_an_old_mobile/)


## Third iteraction: Immeidate power on and writing

Transitioning from the previous iteration using an old phone, I sought a solution that would provide an instant writing experience similar to commercial WriterDecks, utilizing an ESP32 microcontroller.

![Micro Journal Rev.4](/micro-journal-rev-4-esp32/doc/001.webp)

This version represents the most challenging DIY build among my iterations. Handwiring the keyboard directly to the ESP32 and connecting the display to the board required a hardcore DIY approach, as resources and guides for this specific setup were scarce. It demanded some prior knowledge and experimentation to get it up and running.

Despite the challenges, this version comes closest to my ideal writing device. It powers on almost instantaneously (within 1 second) and prompts you to start writing immediately. Additionally, it offers Google Drive sync functionality, allowing seamless file synchronization with your cloud storage.

With these features in place, this build proves to be highly practical. It's perfect for capturing sudden bursts of inspiration or quickly jotting down important tasks or ideas. It's a device that can be kept on your bedside table, ready to record your dreams at a moment's notice. All you need to do is find the power switch before immortalizing your weirdest dream ever in writing.

[Micro Journal Rev.4 Build Details](/micro-journal-rev-4-esp32/readme.md)


# Further Information

https://new.reddit.com/r/writerDeck

https://www.writerdeck.org/