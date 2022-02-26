# jgswpzzlbt

## Overview

![Overview image](overview.png)

This is a jigsaw puzzle solving robot I designed. It is programmed in C++ using OpenCV for image recognition and LibSerial for communicating with a custom Arduino firmware. For further information take a look at the overview video in the file `overview.mp4`.

## Dependencies

I generally try to minimize dependencies, but I'm a one man crew and can therefore only support Debian as I'm running it myself. Anyway, you need to have the following packages installed for everything to work properly:

- FreeCAD for editing and exporting the CAD file. Install it with `sudo apt install freecad`.
- A2Plus for assemblying the robot virtually in FreeCAD. Install it via the integrated addon manager.
- fcgear for creating the needed gears in FreeCAD. Install it via the integrated addon manager.
- Arduino IDE as a way to compile the Arduino code. Install it with `sudo apt install arduino`.
- GCC, G++, GDB and CMake for building the C++ code. Install it with 'sudo apt install `sudo apt install build-essential gdb cmake`.
- VSCodium as universal text editor and IDE. To install it visit their [website](https://vscodium.com/#install).
- OpenCV as the used computer vision library. Install it with `sudo apt install libopencv-dev`.
- LibSerial as the used serial communication library. Install it with `sudo apt install libserial-dev`.

## How it works

The process of solving a jigsaw puzzle automatically involves the following major steps:

1. Locating the somewhat randomly positioned parts.
2. Creating detailed pictures of all the parts.
3. Moving the parts to known positions inside a predefined matrix.
4. Locating each part in the provided overview picture of the puzzle.
5. Translating the digital positions to a real world ones.
6. Moving all the pieces to their final position.

## How to build it

jgswpzzlbt's mechanics mainly consist of a big 110x120cm chipboard, a 1m long 20x80mm aluminium extrusion and some custom 3D printed parts. An assembly overview image can be found in the file `cad/assembly.png` as well as an assembly overview CAD model in the file `cad/assembly.FCStd`. But please don't expect any parts to fit perfectly or be easy to assamble as only one prototype has been build so far.

![Assembly image](cad/assembly.png)

Below is a full bill of materials with German sources for all non printable parts:

| Quantity: | Item: | Source: |
| --- | --- | --- |
| 1 | MKS Base motherboard | [Amazon](https://www.amazon.de/Base-V1-6-RepRap-3D-Drucker-Steuerung/dp/B07G9K1T28) |
| 1 | HBV-1716WA USB camera | [Amazon](https://www.amazon.de/USB-Kameramodul-HBV-1716WA-Millionen-Weitwinkelobjektiv-OV2710-Chip/dp/B07QXXFGSM) |
| 1 | USB extension | [Amazon](https://www.amazon.de/deleyCON-Speed-Verl%C3%A4ngerungskabel-Stecker-Buchse/dp/B00WHZ7GBW) |
| 3 | NEMA 17 1.8deg stepper motor | [Amazon](https://www.amazon.de/STEPPERONLINE-Schrittmotor-63-74oz-4-Draht-Stepper/dp/B07MZJGNZN) |
| 1 | NEMA 11 1.8deg stepper motor | [Amazon](https://www.amazon.de/STEPPERONLINE-Schrittmotor-Bipolar-28x28x31mm-DRUCKER/dp/B07HHZQMDW) |
| 1 | Power supply | [Amazon](https://www.amazon.de/Netzteil-Netzadapter-Transformator-Kaltger%C3%A4testecker-Streifen/dp/B07FNMKTBL) |
| 1 | SG90 micro servo | [Amazon](https://www.amazon.de/ARCELI-Flugzeug-Zubeh%C3%B6r-Arduino-Hubschrauber/dp/B07MY2Y253) |
| 2 | Endstop | [Amazon](https://www.amazon.de/GTIWUNG-Mechanischer-Endschalter-3D-Drucker-Makerbot/dp/B07VWRS24K) |
| 1 | Vacuum pump | [Amazon](https://www.amazon.de/KSTE-Vakuumpumpe-Mikrovakuumpumpe-Mini-Luftpumpe-Behandlungsinstrument/dp/B08BJWLKBW) |
| 1 | Vacuum nozzle | [Amazon](https://www.amazon.de/SRJQXH-Pneumatischer-TS-M10-L80-11-Saugnapfhalter-Industrieller/dp/B098SCMZWY) |
| 1 | Vacuum valve | [Amazon](https://www.amazon.de/Magnetventil-%C3%96ffner-Fl%C3%BCssigkeit-Pneumatik-Elektroventil/dp/B0858Y2KSG) |
| 1 | Vacuum tubing | [Amazon](https://www.amazon.de/JBL-Luftschlauch-Länge-Durchmesser-Aquaschlauch/dp/B000H6SRMU) |
| 1 | Vacuum tubing connectors | [Amazon](https://www.amazon.de/Uniclife-Aquariumschl%C3%A4uche-Luftschlauchverbinder-Kunststoff-St%C3%BCck/dp/B07Q9Q9C8Y) |
| 1 | GT2 belt | [Amazon](https://www.amazon.de/OZUAR-Zahnriemen-3D-Drucker-Breite-Abstand/dp/B07D4GCYXW) |
| 1 | Cable ties and cable tie mounts | [Amazon](https://www.amazon.de/Hipeqia-Nylon-Kabelbinder-Kabelbinderhalter-Selbstklebend-Hitzebest%C3%A4ndig/dp/B08CL2JFCH) |
| 1 | Double sided tape | [Amazon](https://www.amazon.de/doppelseitiges-Klebeband-klebend-verschiedene-Breiten/dp/B00G6SCYCM) |
| 1 | Instant glue | [Amazon](https://www.amazon.de/Pattex-Sekundenkleber-L%C3%B6sungsmittelfreier-Gel-Kleber-Transparent/dp/B00O23B8IE) |
| 1 | Cable chain | [Amazon](https://www.amazon.de/Haobase-Plastic-Towline-Cable-Chain/dp/B06XKZ35VH) |
| 1 | Cables | [Amazon](https://www.amazon.de/Donau-Elektronik-GMBH-Original-Kupfer/dp/B01BI1G88C) |
| 1 | Pin header | [Amazon](https://www.amazon.de/IZOKEE-Männlich-Stiftleiste-Buchsenleiste-Lochrasterplatine/dp/B07DBY753C) |
| 1 | Jigsaw puzzle mat | [Amazon](https://www.amazon.de/maDDma-Bastelfilz-Meterware-Taschenfilz-Filzstoff/dp/B0797QR4LL) |
| 1 | 110x120cm chipboard 8mm | [Globus Baumarkt](https://www.globus-baumarkt.de/spanplatte-roh-8-mm) |
| 42 | 3x12mm TX10 wood screw | [Globus Baumarkt](https://www.globus-baumarkt.de/detail/index/sArticle/27670/fs/116194815/sCategory/47135) |
| 1 | Aluminium profile 20x80 I-type slot 5 1000mm | [Motedis](https://www.motedis.com/shop/Aluprofile/20-I-Typ-Nut-5/Aluprofil-20x80-I-Typ-Nut-5::3798.html) |
| 8 | M5 t-nut I-type groove 5 | [Motedis](https://www.motedis.com/shop/Nutprofil-Zubehoer/In-der-Nut/Nutenstein-Glatt-I-Typ-Nut-5-M5::5846.html) |
| 1 | GT2 motor pulley 20 teeth, 5mm bore | [Motedis](https://www.motedis.com/shop/Dynamik/Zahnriemen-und-Zahnraeder/GT2-Riemenscheibe-20-Zaehne-5mm-Bohrung-fuer-6mm-Riemen::4248.html) |
| 11 | 625 2RS ball bearing | [Motedis](https://www.motedis.com/shop/Dynamik-Basiselemente/Kugellager/Rillenkugellager/Rillenkugellager-625-2RS-5x16x5::4290.html) |
| 4 | DIN 912 M2x20 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m2-8-8-galvanisch-verzinkt-zylinderschraube_200581_13656) |
| 4 | DIN 934 M2x0.4 nut | [Schraubenkasten](https://www.schraubenkasten.de/muttern/sechskantmutter/din-934/edelstahl-a2_202500_23008) |
| 4 | DIN 912 M2.5x8 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m2-5-8-8-galvanisch-verzinkt-zylinderschraube_200582_13676) |
| 32 | DIN 912 M3x8 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m3-8-8-galvanisch-verzinkt-zylinderschraube_200583_13715) |
| 18 | DIN 934 M3x0.5 nut | [Schraubenkasten](https://www.schraubenkasten.de/muttern/sechskantmutter/din-934/edelstahl-a2_202502_23016) |
| 12 | DIN 912 M5x14 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m5-8-8-galvanisch-verzinkt-zylinderschraube_200585_13871) |
| 5 | DIN 912 M5x25 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m5-8-8-galvanisch-verzinkt-zylinderschraube_200585_13883) |
| 3 | DIN 912 M5x50 screw | [Schraubenkasten](https://www.schraubenkasten.de/din-912-mit-innensechskant/m5-8-8-galvanisch-verzinkt-zylinderschraube_200585_13899) |
| 16 | DIN 934 M5 nut | [Schraubenkasten](https://www.schraubenkasten.de/muttern/sechskantmutter/din-934/edelstahl-a2_202504_23024) |
| 3 | DIN 7349 M5 washer | [Schraubenkasten](https://www.schraubenkasten.de/scheiben/unterlegscheiben/din-7349-schweren-spannstift/5-3-din-7349-scheiben-schrauben-mit-spannstifte-a2-edelstahl_203612_23840) |
| 9 | Cable clip | 3D printer |
| 8 | Cable tie mount | 3D printer |
| 1 | C axis gear | 3D printer |
| 1 | C axis motor mount | 3D printer |
| 1 | C axis turntable | 3D printer |
| 8 | Puzzle mat clamp | 3D printer |
| 1 | Vacuum tube fitting | 3D printer |
| 1 | X axis bottom carriage | 3D printer |
| 4 | X axis camera spacer | 3D printer |
| 1 | X axis motor mount | 3D printer |
| 1 | X axis pulley | 3D printer |
| 1 | X axis pulley mount | 3D printer |
| 1 | X axis pulley spacer | 3D printer |
| 3 | X axis roller | 3D printer |
| 6 | X axis roller spacer | 3D printer |
| 1 | X axis top carriage | 3D printer |
| 2 | Y axis carriage | 3D printer |
| 4 | Y axis gear | 3D printer |
| 1 | Y axis stop | 3D printer |
| 12 | Y axis track | 3D printer |
| 1 | Z axis carriage | 3D printer |
| 1 | Z axis gear | 3D printer |
| 1 | Z axis motor mount | 3D printer |

All parts were printed on my Sovol SV01 in standard PLA with a custom super fast profile. The following settings have been used with great success:

- 0.3mm layer height
- 0.6mm line width
- 1.2mm wall thickness
- 0.9mm top/bottom tickness
- 20% line infill
- 215°C nozzle temperature
- 60°C build plate temperature
- 75mm/s print speed (37.5mm/s for the initial layer, 75mm/s for infill, 37.5mm/s for walls, 120mm for travel)
- 100% cooling from the second layer on
- Zig zag support and 5mm raft, where necessary

## How to create a jigsaw puzzle

As this robot uses a special simplified type of jigsaw puzzles, you have to create the puzzles to solve with it yourself. You can simply use the given example puzzle by going to a photo shop (they offer a way better print quality than an average home printer), printing the file `reference-with-cutting-lines.png` in the common 15x10cm format and cutting it into pieces, but creating your own puzzle entirely isn't to difficult either. To do so you need to create a reference image of the whole jigsaw puzzle, a variant of this image with cutting lines as well as part images to help with labeling the cut parts later on. Then just print it out in your desired size and cut it into pieces (keep in mind that the parts shouldn't get to small to work properly).

## How to use it

After installing all the necessary tools and libraries mentioned above as well as building the hardware, you can flash the firmware to the MKS Base motherboard via the Arduino IDE and compile the camera calibration, manual control and main program with CMake. Then test the whole system with the manual control program. If everything works correctly, the next step is to perform the automatic camera calibration. Finally you can run the dang awesome thing with the main software to solve some puzzles!
