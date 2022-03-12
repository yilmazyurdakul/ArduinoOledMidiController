MI am a musician so I use recording software, vst instruments, playback devices and more. It is easy to use physical knobs or faders but if you are in front of the computer, you have to follow the screen.

It is impossible if you are a drummer :)

I built simple OLED encoder menu system before. In this project I improved that so much and combined with MIDI system.

Specifications
1 joystick to control XY plotter
4 faders (can be increased due to board)
9 buttons
OLED display
Encoder controller
Enable / disable
Settings menu
Enable / disable
Easy to use (for me)
MIDI messages flows over the USB cable to computer. Hairless MIDI receiver takes the serial port signals and sends to MIDI channels.

Wiring is really easy. I will explain step by step

1 - Oled SDA to A4

2 - Oled SCL to A5

3 - Oled GND to GND, VCC to +5

4 - Encoder GND to GND, VCC to +5

5 - Encoder sw to switch pin defined in code

6 - Encoder CLK to Digital 2 pin

7 - Encoder Data to Digital 3 pin

8 - Joystick GND to GND, +5 to +5,

9 - Joystick x to A0, y to A1,

10 - Potantiometers to other analog pins

Hairless MIDI tracks serial messages so baud rates must be same. Hairless midi default baud rate is 115200.

Hairless MIDI receives serial messages so you have to get this data and send to MIDI port. loopMIDI can handle this job really nicely. It provides virtual MIDI input and output ports.

Wiring is really easy. I will explain step by step

1 - Oled SDA to A4

2 - Oled SCL to A5

3 - Oled GND to GND, VCC to +5

4 - Encoder GND to GND, VCC to +5

5 - Encoder sw to switch pin defined in code

6 - Encoder CLK to Digital 2 pin

7 - Encoder Data to Digital 3 pin

8 - Joystick GND to GND, +5 to +5,

9 - Joystick x to A0, y to A1,

10 - Potantiometers to other analog pins

Hairless MIDI tracks serial messages so baud rates must be same. Hairless midi default baud rate is 115200.

Hairless MIDI receives serial messages so you have to get this data and send to MIDI port. loopMIDI can handle this job really nicely. It provides virtual MIDI input and output ports.
