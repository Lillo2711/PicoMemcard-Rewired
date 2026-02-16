# PicoMemcard – Simplified Fork (Button + WS2812 Edition)

This project is a simplified and hardware-oriented fork of the original PicoMemcard by Daniele Giuliani.

Modified and maintained by **Alber Multi House**.

This version focuses on standalone operation using a physical button and WS2812 RGB LED, removing controller-based commands and simplifying memory card management.

It emulates a PlayStation (PSX) Memory Card using a Raspberry Pi Pico or RP2040-Zero and allows save management directly via USB Mass Storage.

---

## Main Differences from Original PicoMemcard

- Physical button replaces controller input
- Automatic memory card creation
- Any `.MCR` filename supported
- Persistent last selected card
- WS2812 RGB LED status system
- Simplified firmware architecture
- Designed for RP2040 Zero and Pico
- No controller combo required
- Optimized SD card workflow
- Focused on reliability and simplicity

---

## Features

- Faithful PSX Memory Card emulation
- USB Mass Storage access to SD card
- Automatic creation of first memory card
- Supports multiple `.MCR` images
- Physical button control:
  - Short press → switch memory card
  - Long press (5 seconds) → create new memory card
- Remembers last selected card
- WS2812 RGB LED feedback
- SD card storage (SPI)
- Works on PSX and PS2 (PSX memory mode)

---

## Hardware Requirements

- Raspberry Pi Pico or RP2040-Zero
- SPI MicroSD module
- WS2812 RGB LED
- Push button
- PSX Memory Card connector or controller cable

---

## Button Control

| Action | Function |
|--------|----------|
| Short press | Switch to next memory card |
| Hold 5 seconds | Create new empty memory card |

---

## Memory Card Files

- Must be exactly **128KB (131072 bytes)**
- Must use `.MCR` extension
- Any filename allowed (example: `game1.mcr`, `slot2.mcr`)

Behavior:

- If no `.MCR` exists → firmware creates one automatically
- Last selected card is saved to `selectMC.txt`
- On boot, previously selected card is restored

---

## LED Status (WS2812 RGB)

| Color | Meaning |
|------|---------|
| Off | Idle |
| Orange | Writing / syncing |
| Red blinking | Error |
| Blue blinking | Memory card switching |
| Green blinking | New memory card created |

---

## SD Card

- FAT formatted
- Uses SPI interface
- All `.MCR` files must be in root directory

---

## Firmware Installation

1. Hold BOOTSEL on Pico / RP2040
2. Plug USB
3. Drag UF2 firmware
4. Device appears as USB Mass Storage
5. Copy `.MCR` files to SD card

---

## Important Warning

Never power PicoMemcard from USB while connected to PSX.

This will feed 5V into the console’s 3.3V rail.

If debugging via USB, disconnect VBUS from PSX.

---

## Project Philosophy

This fork prioritizes:

- Hardware simplicity
- No controller dependency
- Direct physical interaction
- Reliability over feature complexity
- Embedded-friendly design

It is intended for real installations inside memory card shells or custom enclosures.

---

## License

GPL-3.0 (same as original PicoMemcard)

This project is a derivative work of PicoMemcard by Daniele Giuliani.

Original repository:
https://github.com/dangiu/PicoMemcard

---

## Credits

Original PicoMemcard:
Daniele Giuliani

PSX protocol documentation:
Martin Korth (NO$PSX)
Andrew J. McCubbin

FatFS:
ChaN

TinyUSB:
Ha Thach

---

## Author

Alber Multi House  
Brazil – 2026  

Simplified Fork Edition