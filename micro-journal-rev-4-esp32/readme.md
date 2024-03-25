# Distraction-Free writing device: WriterDeck 

This is a build guide for a writerDeck based on ESP32, and ILI9341 display with 30 keys hand wired keyboard. 

![Micro Journal Rev.4](/micro-journal-rev-4-esp32/doc/001.webp)

> Turn on and type immediately. Sync with Google Drive. Simple. Let your draft kept always recorded.

WriterDeck is a dedicated writing device, essentially a digital typewriter. One of its key features is providing a distraction-free writing environment.

The build utilizes the ESP32, a powerful microcontroller equipped with onboard WiFi connectivity. This means there's no need to wait for boot time; it prompts you to write immediately upon powering on.

## Motivation

I've been scouring the internet for WriterDeck builds that incorporate microcontrollers like the ESP32 or similar options. Ideally, I'm seeking a device that powers on instantly, creating a conducive writing environment, and has the added convenience of syncing with Google Drive.

I couldn't find such a solution online, likely because it caters to a niche demand in the market. WriterDeck itself represents a niche interest, and the idea of a DIY solution using a microcontroller seemed like an exceptionally rare occurrence rather than a widely recognized concept.

I aimed to acquire a dedicated device that allows me to type immediately, facilitating the collection of meeting notes or capturing fleeting ideas before they fade. While I've explored various iterations of a WriterDeck, none of them offer the crucial feature of instant powering on.

So, I decided to build one by myself. 


## Cost of it

There are commercial products available, ranging from around 120 euros to more high-end options priced at around 600 euros, which should meet your writing needs effectively. However, I understand that the price of commercial products may seem quite steep. This is likely due to the niche demand for such devices, resulting in lower production volumes and higher prices to compensate for production costs.

Indeed, DIYing the WriterDeck would likely result in a significantly lower cost. The key components to purchase would include the ESP32, a display, and keyboard components. With careful selection and sourcing, the total cost should ideally end up around 50 euros. 

Comparing costs alone, DIYing a WriterDeck can be much more economical than purchasing commercial products. However, it's important to note that commercial products typically offer higher quality management, better enclosure quality, and overall finished goods. DIY projects may not directly replace commercial products in terms of quality and finish, but they can still provide a cost-effective solution for those willing to trade off some polish for affordability.

![Had it all](/micro-journal-rev-4-esp32/doc/002.jpg)

> I had all the components to build this, so for me was 0 cost. But should be cheap overall, even buying all those parts.

With all the components readily available as spare parts from previous projects, building one cost me nothing. I'm certain that some of you who already have access to an ESP32 in your DIY arsenal might already have the necessary components in your inventory as well.

* [ESP32 DEVKIT 38 PINS](https://www.amazon.it/dp/B071P98VTG/ref=sspa_dk_detail_3?psc=1&pd_rd_i=B071P98VTG&pd_rd_w=ElqhN&content-id=amzn1.sym.7ef04998-a968-4397-9cab-635d55e2c83b&pf_rd_p=7ef04998-a968-4397-9cab-635d55e2c83b&pf_rd_r=0XFB2NG9Y7T80TEE0NH3&pd_rd_wg=BxnW7&pd_rd_r=fb934b1f-5c03-4e3e-8b9e-9b08944b0c92&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw)

* [2.8" ILI9341 240 * 320 TFT LCD](https://www.amazon.it/ILI9341-Display-seriale-Arduino-Raspberry/dp/B07YTWRZGR/ref=sr_1_1?crid=33UFYFUYU690D&dib=eyJ2IjoiMSJ9.udn7BQTNxA_kZGil72kEjGLU3KU29SBvOivMdzGqh-axe86XOvomcD128OXFebcp.3fztLxViWf7xlXiJEpEBoHr-hREGQj7m4Bu12lS9KAQ&dib_tag=se&keywords=ili9431&qid=1711364443&s=industrial&sprefix=ili9431%2Cindustrial%2C115&sr=1-1)

* [Keyboard Switches](https://www.amazon.it/Akko-Meccanica-Coperchio-Trasparente-Interruttore/dp/B0BM9KR6QH/ref=sr_1_2?crid=12469BI9O89HC&dib=eyJ2IjoiMSJ9.JDei6vrNn9K1FN4Mxt5pdeDb7diKXCGtdp3LXFRaTXMDO1YdB-EALyrVMXf0h8YFBMaKVEiAak38yoPQkMhHlLkEBGk7GUsvWQMXZ3ZKlT_x752xa397S7Vy-08O8YGFEZeSd8SzbfT0n-xkLEc6FqXNE1MWIAKfVKDSmHgk05U.2u-rursn1nMjZ3I57wr6VxJqtPlR2B33kRpYVOJGDCg&dib_tag=se&keywords=akko+creamy+blue+v3&qid=1711364517&sprefix=akko+creamy+blue+v3%2Caps%2C123&sr=8-2)

* Keycaps, SDA or XDA profile - all same height among all the keycaps


## Challenges - finding a software

I searched for an open-source solution that had previously developed a device similar to what I envisioned: a microcontroller with a keyboard and integrated word processing software for typing on the go. While I found a few projects related to keyboards and microcontrollers, none of them included the necessary software to handle display and word processing. This presented a considerable challenge. In theory, I considered that terminal-based word processors from the past could potentially be ported to the ESP32. Perhaps someone had already attempted it, but I couldn't find any such project.

Consequently, I made the decision to develop an ESP32 firmware myself. This firmware would not only provide basic word processing capabilities but also include functionality for syncing files with Google Drive, enabling seamless integration with personal cloud storage.

![word processor](/micro-journal-rev-4-esp32/doc/003.jpg)

> In order to transform the ESP32 into a WriterDeck, it required specialized software, which I couldn't find available. Therefore, I took it upon myself to develop the necessary software from scratch.

The most significant challenge of the project was developing the software. While the software I built may not yet match the sophistication of commercially produced devices, it was the best solution available to me at the time.

# Step 1. Handwired Keyboard

![Handwired Keyboard](/micro-journal-rev-4-esp32/doc/004.jpg)

DIY projects involving microcontrollers often require hands-on wiring and assembly. One aspect I tackled personally was building the keyboard from scratch. My vision was to split the keyboard and position the display in the center, which required careful manual construction.

![Split Keyboard](/micro-journal-rev-4-esp32/doc/005.jpg)

I didn't even consider purchasing off-the-shelf options for the dimensions of the split keyboard and display. Instead, I relied on resources available on platforms like YouTube for guidance on building a hand-wired keyboard. For me, the simpler approach involved 3D printing the keyboard plate and installing the switches manually, while others might prefer the PCB method.

The key steps involve placing the switches and wiring each leg horizontally and vertically to create a matrix of columns and rows. To prevent ghosting, it's necessary to install diodes on either the horizontal or vertical connections. The specific build method chosen depends on individual preferences and needs.

STL files for the keyboard can be found that the following folder in the repository

[/micro-journal-rev-4-esp32/doc/stl](/micro-journal-rev-4-esp32/doc/stl)


# Step 2. ESP32

I utilized a devkit with 38 pins for this project, but other ESP32 boards can also be used. As long as the board supports GPIO interactions, like the S2 or S3 variants, compatibility shouldn't be an issue. Just ensure that the selected board has a sufficient number of GPIO pins available for the project's requirements.

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

I opted to use a breakout board to wire up the components and tested them before proceeding to the enclosure stage. Depending on the specific requirements of your project, you may need to rearrange some of the pins. In my case, I utilized nearly all the available pins allowed through the breakout board, which made it a tight fit for all the components. However, I observed that some pins remained available directly on the ESP32 board. This means there's still the possibility to utilize these additional pins for integrating extra sensors or enabling touchpad functionality for future enhancements to the project.


## Solder the wire

Once you have finalized all the wiring and pin configurations, it's time to solder them onto the ESP32 board. To create more space for the components, I removed the metal pins that were originally on the ESP32 board. This step ensures a snug fit and optimal layout within the device.


## Installing the firmware

To access the project repository for the micro-journal firmware, you can clone the repository from the following link: 

https://github.com/unkyulee/micro-journal. 

Once cloned, navigate to the "/micro-journal-rev-4-esp32" folder to find the PlatformIO source project.

To work with the project, you'll need to open it using Visual Studio with the PlatformIO plugin installed. From there, you'll be able to compile and upload the firmware to the ESP32 microcontroller.

Feel free to modify the keyboard layout codes as needed to suit your preferences.

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

I salvaged an 18650 battery from an old laptop battery pack. Due to its age, I closely monitored it during charging. It reached a full charge of 4.2 volts and completed a single charge cycle, suggesting a capacity around 3000 mAh. Over the past week, I've been using it for one-hour sessions without needing to recharge, indicating its reliability. With this performance, I expect this battery system to provide a very decent writing time, likely around 5 hours or more, even with the aged battery.


* [LiPo Charge Controller and 5V output module](https://www.amazon.it/Ouitble-caricabatteria-integrato-scarico-caricabatterie/dp/B09MQ6ND2Q/ref=sr_1_12?crid=8O78303YIA7M&dib=eyJ2IjoiMSJ9.ogstJhP7PWO-g81-wnxKh6VUbIZRJhLdyBc18r5jSRC5YsVWLrFZEKaf5mNfcg3UK9LZdo_N8zQ_iPBbUhlwr3lt_ZKxrB8_a-TVVGLO5m8y3xMHqBYbCl5NF8diqwd4Kg68cxOloVVTeY8NWgio7k0ON1HuFfkIrApVw4revVO-RqwcBmSR8IBHGI2L8TYDwmDItWB_bpQt9TZzSWhU2c5RgGiAslF4rDhHRQsu67HdDcQBRHaJdhroxiRJUNBHL3CnXyxrpiOCMKIH7pG8nBPgcX4mhEASN-Sw3NmjFsQ.qikh6iFDKhjEch8EOAYNdKsQmJk30ucZsf9OFwOfRzg&dib_tag=se&keywords=lipo+charger+module&qid=1711365173&sprefix=lipo+charger+module%2Caps%2C145&sr=8-12)

I bought the charge module that provides 5V output. 

# Step 4. Enclosure

Once all the wires were soldered to each component, the next step was cramming all the components into the enclosure. I utilized a fair amount of hot glue to secure the display and ESP32 in place, along with some cable ties to prevent the display case from pressing down on the cables. Using 2.5mm threaded inserts and screws, I successfully closed them inside the enclosure, ensuring a secure fit.

![Enclosure](/micro-journal-rev-4-esp32/doc/009.jpg)
![ESP32](/micro-journal-rev-4-esp32/doc/008.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/010.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/011.jpg)
![Enclosure](/micro-journal-rev-4-esp32/doc/012.jpg)



# Typing Test Video

[![Typing Test](/micro-journal-rev-4-esp32/doc/001.webp)](https://youtu.be/S1f8ps_NdyE)


# Google Drive Sync

Once all the components are assembled and the SD card is prepared, place a file named "config.json" inside the SD card with the following content:

```config.json
{
  "network": {
    "type": "wifi",
    "access_points": [
      {
        "ssid": "WIFI AP NAME",
        "password": "WIFIPASSWORD"
      }
    ]
  },
  "sync": {
    "url": "Google App Script URL goes here"
  }
}
```

Update the contents of the file with your WiFi connection information. This configuration will be saved on the SD card and utilized when sync is activated.

To enable syncing to your Google Drive, you'll need to install a Google Apps Script on your Google Drive and obtain its URL. The code for the Google Apps Script can be found in the following path:

[/micro-journal-rev-4-esp32/google/sync.js](/micro-journal-rev-4-esp32/google/sync.js)

Create a new Google Apps Script document in your Google Drive, then copy the provided code into it. Deploy the script as a web app, which will generate a URL linked to your Google Drive. Place this URL in the "url" field of the config.json file. Once configured, the device should be able to sync files to your Google Drive seamlessly.


# Enjoy

I'm confident that this project will inspire others and demonstrate that creating an ESP32-based WriterDeck is indeed possible. Your initiative to push ESP32 projects into more user-friendly, standalone territory is commendable. By sharing your experience and insights, you're paving the way for more creativity and innovation within the community. I believe that others will pick up on the ideas and contributions presented here, fostering a more positive and collaborative environment for creators worldwide. Your efforts are sure to make a meaningful impact in the DIY and maker communities.

Thanks for reading up to here.