# jgswpzzlbt

> Please keep in mind that this is a **very early work in progress**.

![Overview image](overview.png)

This is a jigsaw puzzle solving robot I'm designing. It is programmed in C++ using OpenCV for image recognition and LibSerial for communicating with a custom Arduino firmware.

## Dependencies

I generally try to minimize dependencies, but I'm a one man crew and can therefore only support Ubuntu based Linux distributions as I'm running Linux Mint myself. Anyway, you need to have the following packages installed for everything to work properly:

- OpenSCAD for rendering the CAD files. Install it with `sudo apt install openscad`.
- Arduino IDE as a way to compile the Arduino code. Install it with `sudo apt install arduino`.
- Code::Blocks as an C++ IDE. Install it with `sudo apt install codeblocks`.
- OpenCV as the used computer vision library. Install it with `sudo apt install libopencv-dev`.
- LibSerial as the used serial communication library. Install it with `sudo apt install libserial-dev`.

## How it works

The process of solving a jigsaw puzzle automatically involves the following seven major steps:

1. Locating all the randomly positioned pieces.
2. Moving each piece to a known position inside a predefined matrix.
3. Creating detailed pictures of all the pieces.
4. Stitching all the pictures together to construct the final outcome.
5. Locating each piece in the constructed picture.
6. Translating the digital position to a real world one.
7. Moving all the pieces to their final position.
