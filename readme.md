# Micro Journal - Rev. 2

![First Revision](/images/rev2_6.png)

Micro journal is distraction free writing device. Featuring 30-key mechanical keyboard, and 8-inch wide screen, and powered by Raspberry Pi Zero 2W, offers a focused writing environment.

I would like to explain the inspiration, design challenges, and the evolution of the unique cyberdeck-like creation, ulitimately resulting in a compact and stable writing companion. 

![First Revision](/images/rev2_5.png)

I use this for keeping journals. Been using it since the beginning of 2024. While typing, there are only texts on the screen. Nothing else. Provides immersive experience. Focused. Something about a device that it only does one thing and it does really well. 

This is built by myself. Got hooked into the keyword, "Cyberdeck" while ago. Been searching around for quite a while. I wanted something, that is not just a show off toy, but rather useful with unconventional form factor. 

# Inspired by

![Penkesu Computer](/images/penkesu.png)

http://penkesu.computer/

Came across the a device called penkesu computer. I was inspired. Felt like really close to what I was looking for. A device that delivers mechanical keyboard sensation on a portable format. It is more like, Wuuut? is this even possible? Cuuuuuuuute! Immediately, I wanted to possess this and feel it with my own hands.

So, I built one. I couldn't use the design files that were provided at the penkesu website. Because I was using slightly different screen and also different keyboard. I mean, the main reason for not able to use the design files is, I don't know how to use CAD software, yet. I learned to use fusion 360 along the project.

So, I tried to build up from zero. Watched several youtube tutorial to design boxes and cases. At the end, I have achieved some what similar look and feel.

Satisfied. Very.

**First Version**
![First Revision](/images/rev1.jpg)

(now that I look back at the first version. It does look more slick tho... too bad I am not able to resolve the hinge problem.)

![First Revision](/images/rev1_1.png)

But there were two problems.

1) Hinges
2) Chunky size

Hinges were not at all holding the screen. There is a sense of tick in the hinge activation. But it is absolutely not possible to hold it in place. I had to place some object to support behind. 

If you are planning to follow the design of penkesu. Please, be aware that hinges are not going to the support the screen part of the case.

Second part of the problem was the rather chunky size. I used the cherry mx switch. It is used on the regular mechanical keyboards. They have quite a height. From the bottom to the keycap tip... it takes around 3 ~ 4 cm. That is chunky. It's like 1 litre bottle that you'd take to the hiking.

Probably, size issue could have been improved if I was using low profile switches. In my defense, I had very little knowledge about mechanical keyboards, and didn't know the difference between low profile switches to cherry mx swtiches.

But, I loved the cherry mx switches and the how easy to get supplies of switches. Also, found a switch (Akko Cream Blue Pro) that felt so satisfying. So, I decided to stick with the cherry mx profile. 

Collecting those problems. I went to make the newer revision. Removing hinges making a solid flat platform and for the size, just gave up pocketability. Reason being, it has been mostly used on the desk and occassionaly on the sofa. So, it didn't really need to be closeable box form. 


# Micro Journal - a second revision

![First Revision](/images/rev2.png)

I need a stable positioning to write. I can't have the screen swinging around while writing. It's a small form factor, and means can be repositioned many times while writing. Every time if the screen has to be stabilized. It was hurting the flow. 

I went with the accountant calculator form factor to resolve that. Also, other commercial focus writing devices were using flat panel designs. Having a large flat panel provides a lot of stability while typing on the desk and also writing lying down on the sofa.

Results were very satisfying. With this form factor. With the screen slightly inclined. Provided much stability. 

I could just power on and start typing. No need to find the place to stabilize the hinge. I liked it very much. 


# Writing Experience

I write journal everyday with this device since the beginning of 2024. I missed 4 days since. Only reason I missed writing is because these 4 days were when I diassembled the device and preparing the last revision. 

This means, average 30 minutes of use per day. Nothing compare to the daily driver laptops. Also, I wouldn't use it for work or any producitivy related tasks. It's not designed for that.

This is absolutely amazing to write with this device. Provides shortcut to get into the flow. Those who writes text, or writes code, you guys know, how long it takes to get into the flow. Also, how easy to break the flow... by just receiving an email (or whatsapp) notification on the screen. 

Experience is pleasant. I would say, 80% of the pleasure is from the mechanical keyboard. Also, being able to write on the 30 keys, gives some weird satisfaction that no one would ever care, that it is a pervertiuous sensation for me and I like it. Also, the switches provides a cute girlfriend vibe. Cuddling with the keys... sometimes I just continue to type random words making no sense, just because I wanted to keep feel the switches. 

Oh. right. The joural. I am not sure if I am on the device for writing a journal or just to use the keyboard. One thing for sure, this device, helps a lot to write. Mostly, due to pleasurable experience with keyboard and the fact with such a low power, it can't do anything else than that.


# Keyboard

TU30 Type-c Hot Swappable Mechanical Keyboard DIY Kits 30 Planck Programmable Keyboard Kit Aluminum Plate Plastic Shell

https://www.aliexpress.com/item/1005005719514694.html?spm=a2g0o.order_list.order_list_main.25.3ac41802PKP80N

![First Revision](/images/rev2_2.png)

This is 30 keys keyboard. Not 30%, it has less keys than that. It took me three days of commitment to get used to and within couple of weeks I was typing as normal. But can't get rid of thinking... If I had just, just... three keys more. But writing in general is not a problem once you get used to it.



# Screen

Wisecoco 7.84 Inch 1280x400 LCD Display Stretched Bar LCD Monitor Aida64 Laptop Raspberry Pi Long Strip Panel MIPI Driver Board

(Screen with board)

https://www.aliexpress.com/item/1005004986951553.html?spm=a2g0o.order_list.order_list_main.5.3ac41802PKP80N

![First Revision](/images/rev2_3.png)

Wide screen format. This should be the screen that is almost exactly same size as the keyboard. How cute to picture them cuddling together to cyber deck. 

# STL files

You can find the stl files in the stl folder.
You just need to print two parts. Enclosure and the cover. 

![First Revision](/images/cad_render.png)


# Assembly

I am using raspberry pi zero 2w on this device. But any other SBC that runs linux can and will do. You wouldn't need any fancy software or hardware. It is just a linux terminal with text editors. (nano, vi, micro... ) 

You would need to figure out the power supply part of the design. There are plenty of space under the screen to put a battery and charge controllers and so on. 

https://www.amazon.it/gp/product/B091YQRZPL/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1

I used this parts to supply the power. 

If any soldering or wiring is a tough deal then, you can simply pass through a usb cable to power the device. Most of the time, you would be seating on the desk while using this device. Having a battery wasn't really mandatory anyways.

# Conclusion

Micro Journal, is a pursuit of a simple yet effective writing solution. Born from inspiration of many cyberdeck ideas from the internet, overcoming design challenges, and fueled by a passion for mechanical keyboards, this device has found its place as a cherished tool for daily journaling. While not a daily driver, it provides a delightful writing experience and serves as a proof that mechanical keyboards can indeed find a place in a portable writing device. For those seeking niche solution for the on-the-go writing pleasure, this can open up a new channel for creativity and expression.
