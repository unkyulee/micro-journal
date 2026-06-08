# Process of building a Micro Journal Rev.8: Iron Man

In this revision, I would like to show, as raw as possible, how a new revision is invented.

A step by step record of how things get added, changed, removed, and evolve along the way.

Hopefully, I will remain vigilant in writing down the steps.



## Initiatives

A monochrome LCD screen. Some call it a Sharp Memory LCD. I do not think what I got should be called a Sharp Memory LCD screen. But I got something that resembles it.

<img src="./images/b001.png" width=400 />

There have been quite a few posts on Reddit about this module. It is just black and white, and it should become clearer under brighter light. Meaning it would become more visible under the sun. Almost like a key feature of an e-ink screen, but with a much better refresh rate.

It's called [Osptek Display with model number YDP420H001-V3](https://ko.aliexpress.com/item/1005012114591531.html). 

It has 300 x 400 dot matrix TFT LCD module. Very thin, and looks too good to be true. The only problem I see is that this module is produced by only one company. I do not think it is a common module.

Well... I going to try it anyhow. 

It is interfaced with SPI and an ST7306 display controller. It is a perfect fit for a microcontroller build.


## Buying the module

So, I bought one module to test it out.

I also bought a [FPC Adapter - 0.5-2.54MM, 24P](https://ko.aliexpress.com/item/1005007617729176.html) which breaks out the film cable coming out from the display. A place to solder and connect wires to the ESP32 S3 module.

I am going to use an ESP32 S3 microcontroller. I thought about RP2040. I think it should be much easier to run it with RP2040. But I ended up with ESP32... because Wi-Fi is a fundamental feature for syncing text files.

It is going to take around two weeks to arrive.

It did arrive. Actually, the connector was more expensive than the display and took longer to arrive.


## Janky Janky Janky

So... I need to know if this hardware is going to work at all. I mean, the hardware should be fine. It is more that I do not know if my skill will be enough to make it work.

<img src="./images/b002.jpeg" width=600 />

So, I picked up a cardboard, laid the modules on it, taped them down, and soldered them on. It is the moment when a commitment is made. I cannot return this anymore. I soldered the display to the ESP32 S3 first. It uses an SPI interface, so I used exactly the same wire map as Rev.6.


## Printing a letter on the Display

<img src="./images/b003.jpeg" width=600 />

When I saw STxxxx and SPI interface, I thought this was going to be a piece of cake to make work. I have done this numerous times. I was cocky.

Apparently, there are not many resources available on this display module. But seeing that it uses ST7306, which is supposed to be supported by Arduino... I kept digging.

I found a GitHub library that should support the build

[ST7305_MonoTFT_Library](https://github.com/FT-tele/ST7305_MonoTFT_Arduino_Library)

I tried the example code and tried to run it.

<img src="./images/b004.jpeg" width=600 />

The first sample code actually ran. The good news is that it displayed something intended. But I could not use the entire space of the display. Hmm... this is bad. Because I do not really have the skillset to get into the details of driving the hardware.

After numerous attempts trying out changing parameters at the sample code, I could not figure out how to fill up the display. I tried rotating it. I tried using another library. It always ended up using only half of the surface.

I got frustrated. I almost gave up and threw away everything.

Then... I thought about looking into the hardware driver code. I am going to be honest. I asked AI to explain each line of the driver code, to understand what was going on at a low level. I actually asked AI to write the driver for me. That story will come a bit later.

I learned, well... AI told me that the original library was using only 1 bit to send display information. Whereas it looked like I should send 2 bits, which means the display has grayscale. It is not mono. It has some shades of gray on it.

So, I started modifying the data communication structure, little by little. I watched the changes from each line, just to make sure that I understood what I was doing before moving to higher level code.

<img src="./images/b005.jpeg" width=600 />

Not gonna lie. It was really, really difficult and, at the same time, not really. Because something was displaying on the screen. So it was not really bad. I could feel that it was something really simple to change, but it could be something at a very fundamental level.

The driver library was not so long. So, I was able to understand what it does and control which parts I should change. It took some time. 

The lesson I learned here is that when working on fundamental stuff, I should understand it, first. I cannot just copy and paste AI-generated code and hope it will work. It may work, but because I did not understand anything... when a little issue arises that I need to fix, I cannot fix it.

Understanding the mechanism with the help of AI, but having control over what changes. This felt like a good line to keep.

Probably, this is the situation most businesses face with AI. There are tasks that you can simply ask AI to generate. But certainly, not all of it can be taken over. Humans still need to take control and make important decisions.

For sure, with the help of AI, I could read explanations of the code when, before, it would have taken so long or been almost impossible. That really helped a lot. I would even say that part made the project feasible. Otherwise, it could have been thrown away.

But what changes, and why it changes, has to be explained by myself. I was not able to handle the bug... because I could not understand it. Asking AI to solve it was just generating piles of code that eventually were not a solution. Then I was out of tokens. After that, I changed my attitude. I asked AI to explain the code, and the code that went in was written by me. Through my decisions.

Then it worked!

Seeing Hello World on the screen was such a pleasure. Yeah, I would not have felt this way if the code had all been written by AI.



## Reflective LCD is it good?

After testing the display was done, I attached the keyboard PCB and started testing the user interaction. Then the quality of the display started to reveal its real face.

<img src="./images/b006.jpeg" width=600 />

It is a nice screen. It feels really good. Like an old engineering calculator. But the resolution is not very high. When you look closer, you can see that "W" is a little bit smudged. It is because the display screen has a large physical surface, but it only has 400 x 300 pixels. So, it will look literally pixelated. It is probably a matter of the display resolution. It is not the densest display. But for the purpose of writing, this was plenty acceptable.

<img src="./images/b007.jpeg" width=600 />

Also, since it requires light to reveal the contents on the screen, there is quite a difference in how the letters appear depending on the light. It is not invisible. It is more like this. It shows so well under a nice light, then suddenly transitions to not so visible. This transition is a bit awkward.

<img src="./images/b008.jpeg" width=600 />

As advertised, under the light, the letters appear very clearly and the contrast is really good. It is really clear. Such a thin display showing the contents so well without a backlight. I was very happy with it.

As a conclusion, there is a bit of pixelation due to the low resolution given the large surface area. It also can look a bit blurred when you look at it close up. Although, under decent light and with a bit of distance, this display works really well.



# To be continued.

Next step will be designing an enclosure and some stories how I thought about it, will come. 

