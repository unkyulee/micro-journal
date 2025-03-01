# Micro Journal Rev.7: Kindred Gift

<img src="./images/home.jpg" width="400">

This is the seventh revision of the Micro Journal. When you scroll through the home screen, where all the past revisions are lined up, you can really see how much it has evolved over time. Hard to believe it's been a whole year since this project started. 

For this version, I dialed back the creativity a bit and focused on making something solid and grounded—no surprises, just a design that feels instantly familiar and easy to use.

Rev.7 is a tribute to Hook, Pascal, Stanley, and Frank. The ones who believed in this project even more than I did, right from the start. Their support has meant everything, and this revision is my way of saying thank you to those who've been here since day one.


# Staggered keyboard layout

The key feature of Rev.7 is its keyboard layout. It now uses a staggered layout. The same kind you'll find on most keyboards available on the market. There's no gimmick here, just a familiar design. Well, maybe a little extra: a dedicated cursor navigation cluster and a few additional keys on the right-hand side, all neatly fitting within the rectangular shape. The best part? There's no learning curve. If you've used a computer keyboard before, you'll know exactly where everything is.

Originally, I had planned to gift my previous Micro Journal revision to friends. Featuring an ortholinear keyboard. When I showed it to them, they thought it was cool, but actually sitting down to write with it was a different story. There were pauses, hesitations. Moments of searching for the right keys. I realized that unless someone is deeply motivated to learn a new keyboard layout, adapting to it can feel like a chore.

If I wanted to get my friends (or anyone, really) to type on this, the best approach was to make the keyboard as conventional as possible. This shift is a sign that I'm designing the Micro Journal not just for myself or a small group of enthusiasts. 


# e-Ink display

Another big change in Rev.7 is the display—it now uses an e-ink screen. This type of display mimics the feel of paper when characters appear on the screen, similar to an Amazon Kindle. Once the text is displayed, it stays there until it's electronically refreshed.

I chose an e-ink display because more people were asking for it. Some needed it for eye comfort, while others just thought it was cool. For me, it was more of an experiment—I had to test it myself to see if it was practical. My mindset was simple: if it didn't work, I'd scrap it. But it was worth trying.

## The Downsides: Delays & Ghosting

I wasn't fully convinced at first. E-ink is a fascinating technology, and people talk about it a lot, but it comes with trade-offs—delayed response, ghosting, and a painfully slow refresh rate. Writing on it felt clunky. The letters appeared slightly after I typed them, and remnants of previous text would linger on the screen.

I tested this with an Onyx Boox Air 2 paired with a Bluetooth keyboard. It worked, but something felt off. Was it the lag? Or maybe Android just isn't optimized for writing?

When I first implemented e-ink in Micro Journal, the default setup was terrible. It took a full second for text to appear. A frustrating experience.


## Partial Refresh

The display I used, the T5 e-Paper from Lilygo, supports partial refresh. So, I modified the code to update only the sections that changed instead of refreshing the whole screen. This improved things a lot. The delay dropped to around 100-200ms. Still slow by computing standards, but now it felt usable.

Ghosting remained an issue, but adding a dedicated full-refresh button and an automatic refresh routine helped mitigate it.

By the time I got partial refresh working, I started to wonder, was I just too committed at this point? Had I gone too far to turn back? Maybe. But I also realized… I was actually starting to like it.


## You have to FEEL it

Writing on an e-ink screen, with nothing else on the display, is an experience of its own. The letters appear smudgily, darkening from gray to black with slightly spread edges. There's a subtle analog feel to it. It's like a ghost is writing in front of me with an invisible pencil. It reminds me of typing on a typewriter, where each letter gets stamped onto the page, one keystroke at a time.

Despite all the technical drawbacks, the sensation of writing on e-ink won me over. It's also surprisingly forgiving when drafting. Less pressure to get everything perfect right away.


## An LCD Version Might Happen

I'll probably add an LCD version in the future. It won't be a new revision. Just an alternative screen option while keeping the rest of the design the same.




---

# Design

Let's talk about the design. 

## Some concerns

A standard staggered keyboard is big. Almost twice the size of the ortholinear keyboard from previous revisions. With the smaller layout, it was easier to find a design that felt natural. The keyboard itself was a design element. Just those tightly packed, square keycaps made it look cute on its own.

<img src="./images/design_004.png" width="400">

But with a staggered layout? It suddenly looked like any ordinary office keyboard. That was a concern. Could I make it visually appealing and still well-integrated into the overall design?


## Inspirations

Concerns aside, I went eye-shopping online for inspiration.

<img src="./images/design_001.jpg" width="400">

Then I saw this. It caught my attention immediately. Simple, rounded edges. No unnecessary engravings. Just letting the shape do the talking. A single horizontal line cutting across the body, subtly framing the black surface. So sleek. So sexy.

<img src="./images/design_002.jpg" width="400">

Oooo… that heavy curve! A shape tunneling through the entire body. Simple, but with a bold presence. It reminded me of Rev.6 in a way. Minimal but with a strong statement.

Those two images got stuck in my head. I needed to make something like this. A single shape running through the entire body. Something an artist might create by drawing a circle, extruding it, and calling it a day.

So, I turned to typewriters, wondering if any had that same strong, simple form.

<img src="./images/design_003.png" width="800">

Then this masterpiece appeared. Red. Red. Thick Red. You want a bold design? Here you go. Draw a circle. Extrude. Cut a hole in the middle. Drop a typewriter inside. Done.

Beauty is subjective, but there's something magical about pulling a cylinder out of a block and making it work. I had a strong urge to bring this concept to life.

So I sat down and started drawing a circle.


## Implementation

What I've learned is that the first step is always getting the overall shape and volume right. Details come later. This way, there's less struggle trying to fit everything together at the end.

<img src="./images/design_005.png" width="400">

The shape was coming together. I started with a big extruded box, then cut the corners using a circle. Placed the keyboard inside to figure out the right openings, then cut again.

<img src="./images/design_006.png" width="400">

Next, I refined the layout—adjusting keyboard placement, fitting the display, and hollowing out the back to make space for electronic components.

<img src="./images/design_007.png" width="400">

More refinements—screw holes, compartments for the keyboard controller and battery, and cutouts for switches and USB ports.

<img src="./images/design_008.png" width="400">

Finally, I added knobs and enclosed the display. I used the same hinge mechanism from Rev.2 Revamp, relying on a rubber o-ring for soft friction. Since this display compartment is lighter, I wasn't too worried about the friction holding it in place.


## Looks Good to me

I was curious to see how close the final implementation would be to my original vision. And this time… it was really close. Since the inspiration shape was so simple, there wasn't much room for it to go wrong.

I went through several iterations, but each one led back to the same result. That tells me I really liked this design. I even looked back at a Reddit post from November and compared it to one from January. I spent a lot of time refining this… yet the final design remained nearly identical.

That's a good sign.


---


# Conclusion

It's been a year of revisions and builds. Each one feeling like the ultimate version, bringing an incredible sense of satisfaction to myself. But once a build is finished, the weight of the next one sets in. Can I make something better? Will users see this as a true improvement over the last version? There's always room for refinement. I could push further. Did I stop too soon?

But if there's one thing every iteration has in common, it's that I build something that truly satisfies me. It is keeping a promise to myself.

Micro Journal isn't a job. It's not a business. It's not an obligation. It's a commitment to myself. That's why I put in the extra effort. So that, at the very least, I won't look back and feel ashamed of what I've built.

This is my vision, my work, and my effort. I want to create something that, years from now, will still surprise me.
