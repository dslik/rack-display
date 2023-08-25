## 1U RGB LED Display

A 1U rack-mountable 10x100 pixel programmable RGB LED display that says, among other things:

![image](https://github.com/dslik/rack-display/assets/5757591/24ec1902-c861-4dba-aabb-070941035a4c)

Features:
* Load and display bitmap images over a serial connection
* Modular, with separate display and controller board

Possible future enhancements:
* Wireless support?
* USB connection for power and serial data?
* A nice 3-D printed enclosure for using on your desk?

# Build Journal

_Feel free to ask questions by opening an issue!_

## 2023-08-25

Software is now in pretty good shape.

You can retrieve and set sensor values via SNON:

**Bootstrap discovery:**

Send to device:
```
{}
```

Receive from device:
```
[{"eC":"device","eID":"A8C9ED44-7FD5-57EA-A467-703181738827","eN":{"*":"1910A 1U Rack Displa
y"}}, {"eC":"value","eID":"AEB49E46-B8DC-5F3A-A2FF-851915E41B23","eN":{"*":"Entities"},"v":[
"A8C9ED44-7FD5-57EA-A467-703181738827","AEB49E46-B8DC-5F3A-A2FF-851915E41B23","3F338A51-F643
-5B40-9541-49550B036D3A","03057376-6765-565E-9837-E287D4CC27D0","D54D2FC6-33F0-57BE-B5A9-2D4
509A5AAE5","D45B191B-DBBA-5B99-B253-ECB6872A012C","42601086-6372-5819-8C2C-2FC7CAE1CCA1","68
86D28C-BC5A-524B-8D25-4F3A21580869","97FE1583-E0F5-5AF3-B504-E4274FBA7B5F","A1E5005D-BA77-57
99-B64A-87F93055C175","211902E9-E950-58EA-81E4-79377DB83664","28A7F370-5153-56A6-9D94-44FC6F
3B8644","E87C3F67-CB65-560B-9315-BFB993C7E03A"],"vT":["2023-08-25T08:34:16.758313Z"]}, {"meU
":"s","meT":"iso8601","meAq":"count","eC":"measurand","eID":"3F338A51-F643-5B40-9541-49550B0
36D3A","eN":{"*":"Time Measurand"}}, {"meU":"s","meT":"numeric","meAq":"count","meUS":{"*":"
s"},"meUSx":{"*":"seconds"},"meR":"1","meAc":"1","eC":"measurand","eID":"03057376-6765-565E-
9837-E287D4CC27D0","eN":{"*":"Seconds Measurand"}}, {"eC":"sensor","eID":"D54D2FC6-33F0-57BE
-B5A9-2D4509A5AAE5","eN":{"*":"Device Time Sensor"},"eR":{"child_of":["A8C9ED44-7FD5-57EA-A4
67-703181738827"],"measurand":["3F338A51-F643-5B40-9541-49550B036D3A"]}}, {"eC":"sensor","eI
D":"D45B191B-DBBA-5B99-B253-ECB6872A012C","eN":{"*":"Device Uptime Sensor"},"eR":{"child_of"
:["A8C9ED44-7FD5-57EA-A467-703181738827"],"measurand":["03057376-6765-565E-9837-E287D4CC27D0
"]}}, {"eC":"series","eID":"42601086-6372-5819-8C2C-2FC7CAE1CCA1","eN":{"*":"Device Time Ser
ies"},"eR":{"measurand":["3F338A51-F643-5B40-9541-49550B036D3A"],"child_of":["A8C9ED44-7FD5-
57EA-A467-703181738827"],"values":["97FE1583-E0F5-5AF3-B504-E4274FBA7B5F"]}}, {"eC":"series"
,"eID":"6886D28C-BC5A-524B-8D25-4F3A21580869","eN":{"*":"Device Uptime Series"},"eR":{"measu
rand":["03057376-6765-565E-9837-E287D4CC27D0"],"child_of":["A8C9ED44-7FD5-57EA-A467-70318173
8827"],"values":["A1E5005D-BA77-5799-B64A-87F93055C175"]}}, {"eC":"value","eID":"97FE1583-E0
F5-5AF3-B504-E4274FBA7B5F","eN":{"*":"Device Time"},"vT":["2023-08-25T08:41:52.070132Z"],"v"
:["2023-08-25T08:41:52.070132Z"]}, {"eC":"value","eID":"A1E5005D-BA77-5799-B64A-87F93055C175
","eN":{"*":"Device Uptime"},"vT":["2023-08-25T08:41:52.084258Z"],"v":["455367557"]}, {"eC":
"value","eID":"211902E9-E950-58EA-81E4-79377DB83664","eN":{"*":"Device Temp"},"v":["24.33"],
"vT":["2023-08-25T08:41:51.426916Z"]}, {"eC":"value","eID":"28A7F370-5153-56A6-9D94-44FC6F3B
8644","eN":{"*":"Debug LED RGB"},"v":["0A000A"],"vT":["2023-08-25T08:34:16.762645Z"]}, {"eC"
:"value","eID":"E87C3F67-CB65-560B-9315-BFB993C7E03A","eN":{"*":"Display Text"},"v":[""],"vT
":["2023-08-25T08:41:41.965866Z"]}]
```

**Read a sensor (display text):**

Send to device:
```
{"eID":"E87C3F67-CB65-560B-9315-BFB993C7E03A"}
```

Receive from device:
```
{"eC":"value","eID":"E87C3F67-CB65-560B-9315-BFB993C7E03A","eN":{"*":"Display Text"},"v":[""
],"vT":["2023-08-25T08:41:41.965866Z"]}
```

**Set a sensor value (display text):**

Send to device:
```
{"eID":"E87C3F67-CB65-560B-9315-BFB993C7E03A","v":["THIS IS A TEST"]}
```

Receive from device:
```
{"eC":"value","eID":"E87C3F67-CB65-560B-9315-BFB993C7E03A","eN":{"*":"Display Text"},"v":["T
HIS IS A TEST"],"vT":["2023-08-25T08:42:26.915808Z"]}
```

Almost done! Just need to add some Emoji support, and a few other odds and ends.

## 2023-08-17

The updated controller board came in with the latest batch of PCBs. Panelization looks good:

![image](https://github.com/dslik/rack-display/assets/5757591/bf6dc66a-b94f-4f45-b9c4-5c5e032f0f33)

I soldered on the through-hole components, but I held off on soldering the controller directly onto the display, since it would be painful to desolder if there was a problem with the board.

Fortunately, everything worked!

![image](https://github.com/dslik/rack-display/assets/5757591/6ef56b8f-49a6-4d40-b83d-6ff71b247950)

I got serial communications going, and I added a simple command line program that allows me to set the values of individual pixels:

![image](https://github.com/dslik/rack-display/assets/5757591/606ef0cc-e67f-4762-bcc8-6aa48c85d0cc)

The example source code is checked in under 1910-9101.

There may still be some issues with hardware reset, so I should do some more testing in this area.

Next steps will be to add some support for [SNON](https://www.snon.org/) to allow for structured programatic updates.

## 2023-08-07

The fixed LED PCB is all working!

![image](https://github.com/dslik/rack-display/assets/5757591/67a721bf-9912-4cb7-bf7e-81ed378c1a37)

It looks really good.

Next steps are setting up the controller board once I receive it, and updating the software to allow images to be loaded over the serial port. I should also consider a LORA version.

## 2023-07-27

Next rev of the LED PCB should arrive tomorrow.

In the meantime, I have updated (and uploaded) the 1910-0501 controller board. It allows you to power the display off of a 5V wall wort, and control the display using an RP2040, connected via RS-232.

I'm not 100% happy with this board, since I'd prefer for it to be able to be USB controlled as well, so I'll likely do another major revision at some point in the future. But for the project that triggered creating this display, this should be more than enough.

![image](https://github.com/dslik/rack-display/assets/5757591/d7fc7049-2b41-442f-a71b-ce2f49b97ea7)

This board will go out for manufacturing in the next PCB batch.

## 2023-07-11

The PCBs have arrived, and are ready for assembly:

![image](https://github.com/dslik/rack-display/assets/5757591/35b9402c-f4ce-4884-8227-31d826511c65)

Here is what the controller board looks like all assembled:

![image](https://github.com/dslik/rack-display/assets/5757591/8709bf87-52c0-46c4-b503-f980dc5afe84)

This assembly attaches to the back of the display, and allows you to control the display over a standard RS-232 serial connection.

Unfortunately, after doing some testing, I determined that I made a few minor design mistakes in the controller board (one of them was a footprint that didn't match with the part), so I will have to do another rev. I also need to find some different barrel connectors, since the ones I have don't work with most standard power adapters.

I'm having better luck with the main display board. I was able to get most of it working after a false alarm regarding power draw.

Here's what it looks like on the half of the display that works:

![image](https://github.com/dslik/rack-display/assets/5757591/b7be0e9d-5592-422a-b5a7-5d3fad9839e3)

The reson why only half of the board is working is because I missed a trace between D177 and D178. Opps!

I'll have to do another revision of this board as well, which hurts a lot more since each LED board has a BOM cost of $100.

![image](https://github.com/dslik/rack-display/assets/5757591/0109787f-93cc-46a0-a70b-7d20af890955)

All in all, this has proven the concept, and it looks really good in person!

I am getting around 20 frames per second for half of the screen. Driving each side of the screen sequentially would give me 10 frames per second for full-screen animations.

## 2023-06-22

Yes, I am crazy enough to make this. Well, at least when LCSC gets my favourite RGB LEDs back in stock.

Presenting the Kilopixel rack display, a 1U RGB display for your rack.

Show titles, status, charts and other information in full RGB. Also usable for source indicators in broadcast plants, and anywhere else a 1U device will fit!

![image](https://github.com/dslik/rack-display/assets/5757591/0d8502b4-44a8-4803-b035-05ead2894af1)

And it shall be beautiful and terrible as the Morning and the Night! Fair as the Sea and the Sun and the Snow upon the Mountain!

Oh, wait. Wrong description.

Well, it certainly will be the most expensive board I've ever had manufactured, at over $100 in LEDs alone!

And this long boi will also draw power like there is no tomorrow.... 5 amps if you blasted it at full power, So I'm going to have to be very careful that I make sure the brightness never goes above around 10% of what is capable of.
