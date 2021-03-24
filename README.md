# jgswpzzlbt

> Please keep in mind that this is a very early work in progress.

![Overview image](overview.png)
This is a jigsaw puzzle solving robot I'm designing. It is programmed in C++ using OpenCV for image recognition and LibSerial for communicating with a custom Arduino firmware.

## Dependencies

I generally try to keep dependencies as small as possible, but I'm a one man crew and can therefore only support Ubuntu based Linux distributions as I'm running Linux Mint myself. Anyway, you need to have the following packages installed for everything to work properly:
- OpenSCAD for rendering the CAD files. Install it with `sudo apt install openscad`.
- Arduino IDE as a way to compile the Arduino code. Install it with `sudo apt install arduino`.
- Code::Blocks as an C++ IDE. Install it with `sudo apt install codeblocks`.
- OpenCV as the used computer vision library. Install it with `sudo apt install libopencv-dev`.
- LibSerial as the used serial communication library. Install it with `sudo apt install libserial-dev`.
