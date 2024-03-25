# Distraction Free writing device 

This is a build guide for a writerDeck based on ESP32, and ILI9341 display with 30 keys hand wired keyboard. 

![Micro Journal Rev.4](/micro-journal-rev-4-esp32/doc/001.webp)

> Turn on and type immediately. Sync with Google Drive. Simple. Let your draft kept always recorded.

WriterDeck is a device that is dedicated for writing. Simply putting it as digital typewriter. One of the main purpose of writerDeck is to provide you the focused and distraction free writing environment. 

The build here utilizes ESP32, a rather power micro controller that has wifi connectivity on board. This means, you don't have to wait for any boot time, it will prompt you to write immediately as it powers on.


## Motivation

I was looking for a writerDeck builds on the internet. That maybe using ESP32 or other type of micro controllers. Something that would turn on immediately and provide writing environment. Also possible to sync with Google Drive. 

I was not able to find such a solution on the internet. Probably, because it is such a niche demands on the market. WriterDeck is already a niche demand, and DIY on a micro controller, sounded like extremely rare occasion to be rather general idea that anyone would have. 

I wanted to have a dedicated device that I can type on immediately, to collect notes during the meeting, or draft and idea that is sparked on and about to dim out from my brain. I do have other iterations for a writerDeck. But none of it does the immediate turn on feature.

So, I decided to build one by myself. 


## Cost of it

You can buy something that costs around 120 euros? or something quite fancy at around 600 euros. Which should work really well fit for your writing needs. I think the commercial product price is rather on the high side. It's totally understandable. Only few people would be wanting to have those type of devices... less demands, high prices to make it up to the production. 

DIY the writerDeck, will cost much less. Key components to buy would be ESP32, Display, and the keyboard components. Should possibly end in 50 euros in total. 

![Had it all](/micro-journal-rev-4-esp32/doc/002.jpg)

> I had all the components to build this, so for me was 0 cost. But should be cheap overall, even buying all those parts.

I had all the components at hand as the spare parts of other projects. So, it cost me nothing to build one. I am sure, some of you guys who already has access to ESP32 in your diy arsenal... the components that I used, you may already have it in your inventory. 

* [ESP32 DEVKIT 38 PINS](https://www.amazon.it/dp/B071P98VTG/ref=sspa_dk_detail_3?psc=1&pd_rd_i=B071P98VTG&pd_rd_w=ElqhN&content-id=amzn1.sym.7ef04998-a968-4397-9cab-635d55e2c83b&pf_rd_p=7ef04998-a968-4397-9cab-635d55e2c83b&pf_rd_r=0XFB2NG9Y7T80TEE0NH3&pd_rd_wg=BxnW7&pd_rd_r=fb934b1f-5c03-4e3e-8b9e-9b08944b0c92&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw)

* [2.8" ILI9341 240 * 320 TFT LCD](https://www.amazon.it/ILI9341-Display-seriale-Arduino-Raspberry/dp/B07YTWRZGR/ref=sr_1_1?crid=33UFYFUYU690D&dib=eyJ2IjoiMSJ9.udn7BQTNxA_kZGil72kEjGLU3KU29SBvOivMdzGqh-axe86XOvomcD128OXFebcp.3fztLxViWf7xlXiJEpEBoHr-hREGQj7m4Bu12lS9KAQ&dib_tag=se&keywords=ili9431&qid=1711364443&s=industrial&sprefix=ili9431%2Cindustrial%2C115&sr=1-1)

* [Keyboard Switches](https://www.amazon.it/Akko-Meccanica-Coperchio-Trasparente-Interruttore/dp/B0BM9KR6QH/ref=sr_1_2?crid=12469BI9O89HC&dib=eyJ2IjoiMSJ9.JDei6vrNn9K1FN4Mxt5pdeDb7diKXCGtdp3LXFRaTXMDO1YdB-EALyrVMXf0h8YFBMaKVEiAak38yoPQkMhHlLkEBGk7GUsvWQMXZ3ZKlT_x752xa397S7Vy-08O8YGFEZeSd8SzbfT0n-xkLEc6FqXNE1MWIAKfVKDSmHgk05U.2u-rursn1nMjZ3I57wr6VxJqtPlR2B33kRpYVOJGDCg&dib_tag=se&keywords=akko+creamy+blue+v3&qid=1711364517&sprefix=akko+creamy+blue+v3%2Caps%2C123&sr=8-2)

* Keycaps, SDA or XDA profile - all same height among all the keycaps


## Challenges - finding a software

I tried to find a open source solution that has already done this type of device before. Micro controller with keyboard, and a little word processing software to handle the typing experiece on the go. 

There are few projects that has to do with the keyboard and micro controller, but not the software that can handle the display and the word processing experience. This was really a challenge. In theory, those terminal based word processors that existed quite some time ago, could be ported to ESP32? ... Anyone would have done it? Maybe, but I couldn't find one. 

So, I decided to build a ESP32 firmware that can handle some word processing experience, and also handle the google drive sync, that can sync files to the personal drive. 

![word processor](/micro-journal-rev-4-esp32/doc/003.jpg)

> To make ESP32 behave like a writerDeck, you need a software to do so. I couldn't find the one. So I built one myself.

This is the biggest challenge of the project. Software that I built, is not anywhere going to be close to the commercially built devices. At least at the moment. But this was the best shot I had. 


# Step 1. Handwired Keyboard

![Handwired Keyboard](/micro-journal-rev-4-esp32/doc/004.jpg)

DIY with micro controller means, you may need to wire things on your own. One of the things that I went on building by hand is the keyboard. I wanted to split the keyboard and put the display in the middle. 

![Split Keyboard](/micro-journal-rev-4-esp32/doc/005.jpg)

The dimension of the keyboard, and being it splitted and also the the display. I didn't even look for an option that I could buy off the shelf. 

You can find many resources from the youtube for building a hand wired keyboard. The easier approach for me was to print the keyboard plate and place the switches. Others may find the PCB approach easier. This is really up to you to find the build method that fits your needs. 

Things that you need to do, is to place the switches, and wired each legs one in horizontal direction and the others in vertical direction. Creating columns and rows matrix. One of the horizontal or vertical needs to have diodes installed to prevent the ghosting. 

STL files for the keyboard can be found that the following folder in the repository

[/micro-journal-rev-4-esp32/doc/stl](/micro-journal-rev-4-esp32/doc/stl)


# Step 2. ESP32

I have used devkit that has 38 pins. You can use other types of ESP32 boards. It is built based on GPIO interactions only, so shouldn't be a problem if you use other ESP32 board such as S2 or S3. Make sure that they have sufficient GPIO pins available. 

## Pin out

| PIN   | PURPOSE   | 
| --- | --- | 
| 19   | MISO - SD, DISPLAY   | 
| 23   | MOSI - SD  |
| 18   | SCLK - SD, DISPLAY  |
| 21   | CS - DISPLAY  |
| 22   | DC - DISPLAY  |
|  5  | CS - SD   |
|  32  | CS - Keyboard Rows, Q   |
|  33  | CS - Keyboard Rows, A   |
|  25  | CS - Keyboard Rows, Z   |
|  26  | CS - Keyboard Columns, Q   |
|  27  | CS - Keyboard Columns, W   |
|  14  | CS - Keyboard Columns, E   |
|  12  | CS - Keyboard Columns, R   |
|  13  | CS - Keyboard Columns, T   |
|  15  | CS - Keyboard Columns, Y   |
|  2  | CS - Keyboard Columns, U   |
|  4  | CS - Keyboard Columns, I   |
|  16  | CS - Keyboard Columns, O   |
|  17  | CS - Keyboard Columns, P   |

![ESP32](/micro-journal-rev-4-esp32/doc/006.jpg)

I have used a break out board to wire them up and tested before getting into the enclosure. Some of the pins may be rearranged depending on your project needs. In my case, most of the ... or actaully all the pins were used that was allowed through the breakout board... so was really close call to putting all the components. But I noticed that some pins are still available directly on the ESP32 board. So, it is still possible to use other availble pins for additional sensors or enabling touch pad for your project.

## Solder the wire

Once you have all the wiring and pin out figured out. Solder them on the ESP32 board. I have removed the metal pins that were on the esp32 in order to make more space. 


## Installing the firmware

You can clone the respository (https://github.com/unkyulee/micro-journal) and the find the platformio source project in the following folder. "/micro-journal-rev-4-esp32"

You need to open this project with the visual studio, having the platformio plugin installed, then you should be able to compile and upload the firmware to the ESP32 micro controller. 

You may want to modify the keyboard layout codes. 

## Keyboard Layout

### Layer 1

| Q   | W   | E   | R   | T     | Y   | U   | I   | O   | P     |
| --- | --- | --- | --- | ----- | --- | --- | --- | --- | ----- |
| A   | S   | D   | F   | G     | H   | J   | K   | L   | ENTER |
| Z   | X   | C   | V   | SPACE | B   | N   | M   | NUM  | SHIFT |

### Layer 2 - when NUM key is pressed

| 1    | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 0     |
| ---- | --- | --- | --- | --- | --- | --- | --- | --- | ----- |
| `    | \|  | -   | =   | [   | ]   | ;   | '   |     | ENTER |
| MENU |     |     |     | BS  | ,   | .   | /   | NUM  | SHIFT |



# Step 3. Power Supply

![Power Supply](/micro-journal-rev-4-esp32/doc/007.jpg)

I have harvested one 18650 from old laptop battery pack. It was very old, so I monitored very closed while it was charging up. It was able to charge up to 4.2 volts and complete one charge cycle. I think it should provide in the level of 3000 mAh... and I have been using it for a week (1 hour per session) and still working without needing to charge. I think this type of battery system should provide very decent writing time. I think at least 5 hours even with the old battery. 

* [LiPo Charge Controller and 5V output module](https://www.amazon.it/Ouitble-caricabatteria-integrato-scarico-caricabatterie/dp/B09MQ6ND2Q/ref=sr_1_12?crid=8O78303YIA7M&dib=eyJ2IjoiMSJ9.ogstJhP7PWO-g81-wnxKh6VUbIZRJhLdyBc18r5jSRC5YsVWLrFZEKaf5mNfcg3UK9LZdo_N8zQ_iPBbUhlwr3lt_ZKxrB8_a-TVVGLO5m8y3xMHqBYbCl5NF8diqwd4Kg68cxOloVVTeY8NWgio7k0ON1HuFfkIrApVw4revVO-RqwcBmSR8IBHGI2L8TYDwmDItWB_bpQt9TZzSWhU2c5RgGiAslF4rDhHRQsu67HdDcQBRHaJdhroxiRJUNBHL3CnXyxrpiOCMKIH7pG8nBPgcX4mhEASN-Sw3NmjFsQ.qikh6iFDKhjEch8EOAYNdKsQmJk30ucZsf9OFwOfRzg&dib_tag=se&keywords=lipo+charger+module&qid=1711365173&sprefix=lipo+charger+module%2Caps%2C145&sr=8-12)

I bought the charge module that provides 5V output. 

# Step 4. Enclosure

Once all the wires are soldered to each component. Not it is just a matter of cramming all the components into the enclosure. I have used... quite a few hot glue in order to place the display and esp32. and some cable ties to make sure that the display case, doesn't step on the cables. Using 2.5M threaded inserts and screws, I was able to close then inside the enclosure.

![Enclosure](/micro-journal-rev-4-esp32/doc/009.jpg)
![ESP32](/micro-journal-rev-4-esp32/doc/008.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/010.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/011.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/012.jpg)



# Typing Test Video

[![Typing Test](/micro-journal-rev-4-esp32/doc/001.webp)](https://youtu.be/S1f8ps_NdyE)


# Enjoy

I hope this project gives some idea or vision that creating an ESP32 based writerDeck is possible. I hope to provide an initiatives to take the ESP32 projects into rather standalone human useable territory. I really hope that community picks up bits and pieces that are provided here and bring up more positivity to the creators world. 

Thanks for reading up to here.