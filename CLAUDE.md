# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CryoCalc is a Windows XP+ compatible C++ GUI application for converting between temperature units (Celsius, Kelvin, Fahrenheit, Rankine), designed for cryogenic work and LN2 overclocking.

## Build Systems

This project supports two build systems:

### GN Build (Primary)
```bash
# From the gn-legacy root directory (parent of src/cryocalc)
gn gen out/Debug --args='import("//src/cryocalc/assets/debug_args.gn")'
gn gen out/Release --args='import("//src/cryocalc/assets/release_args.gn")'
ninja -C out/Debug cryocalc_all
ninja -C out/Release cryocalc_all
```

Build targets:
- `cryocalc` - Main executable
- `osinfo` - Helper DLL (osinfo.dll)
- `cryocalc_all` - Builds both

### MinGW Makefile (Cross-compilation)
```bash
make                    # Debug build (default)
make BUILDTYPE=Release  # Release build
make clean              # Clean build artifacts
```
Uses i686-w64-mingw32 toolchain for 32-bit Windows binaries.

## Architecture

### Main Application (`src/`)
- **cryocalc.cc/h** - Entry point (`wWinMain`), window registration, main message loop
- **controls.cc/h** - UI control creation, event handling, dialog management
- **converters.cc/h** - Temperature conversion algorithms in namespaces: `kelvin::`, `celsius::`, `fahrenheit::`, `rankine::`
- **utils.cc/h** - Command line parsing, numeric validation, precision handling
- **globals.h** - Extern declarations for window handles and global state
- **framework.h** - Precompiled header with Windows and STL includes
- **constants.h** - Physical constants for conversions
- **resource.h/rc** - Windows resources (icons, menus, dialogs, strings)

### OS Info DLL (`osinfo/`)
A separate DLL providing Windows version detection (Windows 2000 - 11):
- **os_info.cc/h** - Version detection using RtlGetVersion
- **os_info_dll.h** - Exported API functions (`GetOSNameW`, `GetWinVersionW`, etc.)
- **export.h** - DLL export macros

### Control Flow
1. `wWinMain` initializes common controls and loads `osinfo.dll`
2. Command line parsed via `ParseCommandLine()`
3. Window class registered, main window created
4. `InitControls()` creates UI elements
5. `WindowProc()` handles messages, dispatches to control handlers
6. Temperature conversions use namespaced functions (e.g., `kelvin::fromCelsius()`)

## Key Patterns

- Unicode throughout (WCHAR, std::wstring, -municode)
- Windows subsystem version set to 5.00 (Windows 2000 minimum)
- C++17 standard
- Global handles declared extern in `globals.h`, defined in respective .cc files
- Conversion precision configurable via `SetCryoCalcPrecision()`
