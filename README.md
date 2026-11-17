# DSi Cyberdeck – Homebrew Skeleton

This repository contains a **minimal homebrew project** for the Nintendo DSi XL that turns the device into a tiny cyber‑deck.

## Features (skeleton only)
- Bottom screen: on‑screen touch keyboard.
- Top screen: simple ANSI‑style terminal window.
- DSi hardware buttons mapped to common actions (open file manager, toggle theme, launch GPS, etc.).
- Stub for an external mouse / touchpad via I²C (you will need a small MCU to forward USB‑mouse reports).
- Runtime colour‑theme switching via a small JSON/INI file on the SD card.
- Basic file‑manager using `libfat` (placeholder implementation).

The code is deliberately kept **compact** so you can extend it with the needed subsystems (SSH client, GPS parser, offline maps, etc.).

## Prerequisites
1. **Nintendo DSi XL** with the Homebrew Launcher installed (flash‑cart‑free method).
2. **devkitPro** with `devkitARM`, `libnds`, `libfat` installed on your development PC.
   - On macOS: `brew install devkitpro` (or follow the official devkitPro installer).
3. An **SD card** inserted in the DSi (formatted FAT32) with at least 256 MiB free space.
4. (Optional) An external **I²C bridge** (e.g., an Arduino/Teensy) that reads a USB mouse and streams `dx,dy,left,right` bytes to the DSi.

## Building
```bash
# Clone / copy this folder to your workspace and cd into it
cd "/Volumes/NO NAME/dsi_cyberdeck"
make
```
The output `cyberdeck.nds` can be placed on the SD card and launched from the Homebrew Launcher.

## Project layout
- `main.c` – entry point, screen init, main loop, input handling.
- `theme.h` – simple theme struct and a loader for `theme.ini`.
- `keyboard.h` – definition of the on‑screen keyboard layout and drawing helpers.
- `Makefile` – devkitARM build script.

## Extending the skeleton
- **SSH client**: integrate `libssh2` and spawn a shell inside the terminal window.
- **GPS**: add a UART driver (see `libnds` examples) and parse NMEA sentences.
- **File manager**: expand the placeholder to browse directories and open text/HTML files.
- **Mouse support**: implement the I²C read routine in `main.c` and render a cursor sprite.
- **Theme switching**: drop a `theme.ini` on the SD card (see `theme.h`).

---
### License
Feel free to copy, modify, and distribute this skeleton under the MIT license.
