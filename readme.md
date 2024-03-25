# Micro Journal

I wanted to make a device that can be specialized in writing. Particularly with the mechanical keyboards. A portable, or rather small form factor device that helps you to write and in a focused environment. A distraction free writing device. 

Recommended Sites:

https://new.reddit.com/r/writerDeck

https://www.writerdeck.org/

## First iteration: Using Raspberry Pi and 30 Keys mechanical keyboards. 

I have tried several iteration to explore each options that I can try. First version was using a Raspberry PI 2W, which is a small linux computer that is coupled with 30 keys mechanical keyboard. 

![Micro Journal Rev.2](/micro-journal-rev-2-raspberypi/images/rev2_6.png)

Important factor of the distraction free writing. Is the distraction free. That you should be forced not to be distracted. 

Raspberry Pi 2W, does a very good job on the distraction. Because, it is based on linux terminal. Without Graphical user interface. So, there is no possibilities to run youtube or other type of distrations. 

When I turn this device on, I use mid night commander to reach the folder I want to work on and use "micro" editor (something like nano, or vi) to start writing on it. The screen becomes black and white with just texts that I write and you can't do anything else then writing. 

It helps to you get into the flow very quickly. And the sheer amount of texts that you can write down in the flow is amazing. also, the quality is pretty good. When your brain is constrained to do just one thing. It does deliver pretty awesome results. 

This is the write up for the build guide of ths raspberry pi build.

[Micro Journal Rev.2 Build Details](/micro-journal-rev-2-raspberypi/readme.md) 

[Reddit Post](https://new.reddit.com/r/writerDeck/comments/1b6ff16/micro_journal_rev_2/)

## Second iteration: Using an old phone to make a writerDeck

Next iteration was to build a writing device using the component that can be easily acquired. Like a phone. Old phone still has pretty good power to be used as a writer. And getting connected to the keyboard. This can be a great solution for many of you, that can be built rather easily without going through looking for too techy components like linux or raspberry pi.

![Micro Journal Rev.3](/micro-journal-rev-3-samsung-galaxy-s8/images/001.jpg)

Focus of the build was to be able for my daughter to write journals. And not having to go through difficulties of searhcing for a linux commands. She was used to the table environment. So this build actually fitting well with the purpose. She could pick it up and start writing on it.

This build specifically has 3d prints and hand wired keyboards, so itself may not be the perfect example of an easy build. But just taking the idea of combining an old phone, and a keyboard. You can build the similar purposed device on your own.

One thing to note that. Distraction Free can't be achieved with this build. Phone is such a versatile device and it provide so much entertainment that, it is almost impossible to focus on writing. You can constantly distracted to youtube or media streaming such as spotify and so on. I didn't have so much success on this, but was a good enough device to help my daughter to continue writing journals on it.

Here are build guides:

[Micro Journal Rev.3 Build Details](/micro-journal-rev-3-samsung-galaxy-s8/readme.md)

[Reddit Post](https://new.reddit.com/r/writerDeck/comments/1bfhnfz/writerdeck_with_samsung_s8_using_an_old_mobile/)


## Third iteraction: Immeidate power on and writing

One of the nice thing about the previous old phone iteration is that, it can be turned on immediately and start writing. Compared to thge raspberry pi version, where you need to wait for around 15 seconds before able to type on. 

So, I thought about a solution that can handle such instant writing experience. 

Using ESP32 (a micro controller) this experience of instant writing can be achieved. With a screen and the keyboard attached to it. It can become a nice device that helps you writing. Almost similar to the [commercial WriterDecks](https://www.writerdeck.org/).

![Micro Journal Rev.4](/micro-journal-rev-4-esp32/doc/001.webp)

It is most challenging DIY build among the other iterations. Handwired keyboard is necessary as you need to attach the keyboard wiring directly to the ESP32. Also, Display needs to be wired to the board. This is rather hardcore DIY path, as you may not find much resources on the internet. It may not be simply a follow the steps and you are done. You may need to have some prior knowledge in order to get this running. 

With that said, this is the version as close to the ideal writing device I can think of. It turns on immediately when powered on (1 second?) and prompts you to write on. Also provides Google Drive sync so it can sync your files to your own cloud system. With that achieved. I would say, it is quite useful build. It is something that I can pull out when I have a spark of an idea that needs to be recorded. Or if a boss passes by to drop a big idea or tasks, I can start writing on it without a delay. This would be a fastest device and methods that I can think of to start writing.

You can leave it on the bed stand and be a device that can record your dreams. It requires you to just able to find the power switch before writing your weirdest dream ever. 

[Micro Journal Rev.4 Build Details](/micro-journal-rev-4-esp32/readme.md)

