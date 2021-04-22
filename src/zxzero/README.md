
# Hermit Retro ZXZero

The Hermit Retro ZXZero is a Raspberry Pi Zero W-powered custom
board designed to fit into original, and reproduction, ZX Spectrum and
ZX Spectrum + cases.

Powered by a super-fast booting Linux and the highly-regarded Fuse emulator,
the Hermit Retro ZXZero in a ZX Spectrum case is an authentic
retro experience!

The Hermit Retro ZXZero Board is primarily designed for quick and easy playing of games in an authentic way but with modern benefits. It is not designed as a total replacement for an original ZX Spectrum supporting all the original hardware. For more information, see the [Design Notes](doc/DESIGN.md).

# Current Features

The Hermit Retro ZXZero Board features:

* Raspberry Pi Zero W
* Full-size HDMI output
* DB9 Joystick connector supporting any Atari-pinout joysticks
* External MicroSD card socket (for storing "educational programs")
* I2C expansion port
* 5V DC power input
* Optional USB inputs which can support:
    * Keyboards
    * Mice
    * Game controllers
    * Memory sticks (for storing game ROMs)

The Hermit Retro ZXZero runs:

* Heavily-stripped down Raspbian Linux via buildroot which boots from cold
  in around 6 seconds
* Customised Fuse ZX Spectrum emulator

# Future Features

* ZXDB integration enabling easy download of content via WiFi
* Bluetooth controllers
* Custom controllers via I2C/Bluetooth

# Goals

The high-level goals for this project are as follows:

* The files and instructions to build a Hermit Retro ZXZero will
  be made available (excluding layout/Gerbers)
* Any PCBs should be solderable by persons with a modicum of soldering
  ability (i.e., a minumum of surface mount components that are critical
  to the system)
* The bill of materials should be readily available from online sources
* Expensive components should be avoided, if possible

# Current Status

The Hermit Retro ZXZero is V1.8.0

# Purchasing

Hermit Retro products are available from [ZX Renew](https://zxrenew.co.uk/Hermit-Retro-ZX-Spectrum-Board-c72555098)

# Licensing

```
MIT License

Copyright (c) 2021 Hermit Retro Products Ltd.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, schematics and associated documentation files 
(the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```


# Links

* [Fuse - Spectrum Emulator](http://fuse-emulator.sourceforge.net/)
* [Buildroot](https://buildroot.org/)
