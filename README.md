# BringAuto Pi Watchdog firmware

BringAuto Pi is a device which turns Raspberry Pi into real industrial-grade computer!

This repository contains source files for a BringAuto Pi Watchdog firmware.

The firmware guards device to operate without issues:

- it measures temperature and voltage to reacts and regulate inner environment,
- it provides HW Watchdog functionality

Complete BringAuto Pi Documentation: https://doc.bringauto.com/bringauto-pi

## Build instructions

## Installing Arduino IDE 2

Note: it is assumed the use uses GNU/Linux. Windows users can skip this section and
go directly to "Pico board setup".

Check if user is in dialout group by execute:

``
groups
``

If not add it using - after it log out

``
sudo usermode -a -G dialout $USER
``

Download Arduino IDE 2
https://www.arduino.cc/en/software - Linux AppImage

Install libfuse2

``
sudo add-apt-repository universe
sudo apt update
sudo apt install libfuse2
``
  
- First run > OK

Wait for auto-install packages for Arduino IDE 2

Arduino temporary project is under arduino_wd > arduino_wd.ino

## Pico board setup

Add Pico board definition into Arduino IDE 2 (File > Preferences > Additional board manager URLs)

URL: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

Then install board definitions (On the left panel Board definitions or in Tools > Board > Board Manager ...)
Find definitions for Pico boards by Earle F. Philhower, III (version 3.0.0 tested)

When connecting Pico to the computer you need to press on-board button otherwise scatch
cannot be uploaded to board.
After connecting Pico to the computer select port and board (under Tools)

---------------------------------------------------------------------------------------------------------------------------------------

## Hardware wire-out

It's designed to be used with [BringAuto Pi Watchdog PCBs](https://doc.bringauto.com/) and Raspberry Pi Pico.

- On pin 22 is Enable pin for watchdog - ON when connected to 5V , OFF when connected to GND
- Pin 20 is Master pin for sending ping to Slave.
- Pin 21 is for Slave response (value mimicking)

On board are two jumpers. Jumper JP1 enables MOSFET that restarts RPi4. (It's used for testing watchdog logic, without physically restarting RPi)
Jumper JP2 disables watchdog logic running in Pico. In normal state it should be disconnected.

Complete pinout scheme is available at [Documentation / BringAuto Pi](https://doc.bringauto.com/bringauto-pi)

## Firmware

Slave communicates with the Master via two pins. Master pin sets value (High/Low) that should be mimicked by Slave pin.
After boot Master wait `RPI_BOOT_TIME_MS` for response (after boot is always signal High) from Slave, change state and for other Slave response waits `RESPONSE_TIME_LIMIT_MS`. If slave does not response, Master reboots and again waits `RPI_BOOT_TIME_MS`.

Watchdogs state is signalized via build-in LED:

- LED OFF - Watchdog is DISABLED
- LED ON - Power to RPi4 is currently cut off
- LED ON for `BLINK_ON_OS_BOOTING_MS` and LED OFF for `BLINK_OFF_OS_BOOTING_MS` - os booting
- LED ON for `BLINK_ON_OS_RUNNING_MS` and LED OFF for `BLINK_OFF_OS_RUNNING_MS` - os running and responding

Important variables are stored in [Config.h](arduino_wd/Config.h).

## Upload

> ## WARNING
> Never connect RPi Pico to PC when plugged in board - It must be turned OFF and removed from board

- Build sketch by standard verify Arduino IDE button.
- Connect Watchdog board by a micro-usb cable to the computer. DO NOT forget to disconnect it from the power source!
- Flash the device by Arduino IDE flash button.

Also the Arduino IDE produces uf2 file that can be used to program Pi Pico in a standard way by Pi Pico USB Drive mode

## License

The Software is released under GPL-3.0.

The Software can be used with MIT license if used with original BringAuto Hardware (eg - hardware manufactured directly by BringAuto).