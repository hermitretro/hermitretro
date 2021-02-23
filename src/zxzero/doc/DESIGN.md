
# Hermit Retro ZXZero Design Notes

We thought it might be useful to elaborate on some of the design choices and decisions in building the Hermit Retro ZXZero.

The primary design goals were as follows:

* Quick and easy to setup and play games on modern displays
* Authentic ZX Spectrum experience but with modern benefits
* Integrated joystick support
* No desire to support original ZX Spectrum hardware/expansions
* Modifiable software stack by using well-understood languages and packages
* Can be assembled by hobbyists
* Fits original and reproduction ZX Spectrum and ZX Spectrum+ cases
* Low cost

The following sections explain the rationale in more detail.

# Quick and Easy Game Play

The Hermit Retro ZXZero Board is designed to boot almost immediately into Fuse enabling to start
playing games quickly and easily.

The games are loaded from a second Micro SD card which is exposed through the backplane slot
making it easy to transfer content onto the device. The internal SD card attached to the
Raspberry Pi Zero only hosts the ZXZero firmware.

The board has a full-size HDMI socket for robustness and ease of connection to modern displays.

The board also has a full-size USB Type B socket enabling you to attach modern peripherals such as keyboards, storage devices and game controllers (these are future enhancements).

A DB9 joystick socket with Atari wiring is also included and mapped by default to "Kempston" mode.

# No legacy support

With the exception of joysticks, there is no attempt to support any original Spectrum hardware. We
didn't feel it was really required. If you want a board with original expansion capability, there
are plenty of other options such as the OMNI, Harlequin, etc.

# Can be assembled by hobbyists

It was decided that the Hermit Retro ZX Spectrum Board might be of interest in kit form for
self-assembly as well as being purchaseable as a built product. As such, the design uses
through-hole components as opposed to surface-mount except where absolutely necessary. The only 
surface-mount components are the external Micro SD card and corresponding decoupling capacitor which
can be soldered with drag soldering and care.

# Modifiable Software Stack

One of the interests around using a Raspberry Pi Zero (or Zero W) as the core for this project is
that the entire software stack is built of a core of Raspbian, buildroot and Fuse, all Open Source
projects. As such, there's no specific hardware knowledge required to make changes to the system,
e.g., no FPGA core work or low-level hardware knowledge.

This arguably makes it a simpler and more extensible system for hobbyists to work with.

