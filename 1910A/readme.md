## 1U RGB LED Display

A 1U 10x100 RGB LED display.

Features:
* Load and display bitmap images over a serial connection
* Modular, with separate display and controller board

Possible future enhancements:
* Wireless support?
* USB connection for power and serial data?
* A nice 3-D printed enclosure for using on your desk?

# Build Journal

_Feel free to ask questions by opening an issue!_

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
