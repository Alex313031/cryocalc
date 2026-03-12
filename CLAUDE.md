# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CryoCalc is a Windows 2000+ compatible C++ GUI application for converting between temperature units (Celsius, Kelvin, Fahrenheit, Rankine), designed for cryogenic work and liquid nitrogen overclocking.
It also has an OS Info child window that logs OS Information and provides buttons for launching system utilities.

## Build Systems

This project supports two build systems:

### GN Build (Primary)
```bash
# From the gn-legacy root directory (parent of src/cryocalc)
. set_env.sh # To set path to gn/ninja/MinGW binaries in gn-legacy/tools/*
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
- **controls.cc/h** - UI control creation, event handling, dialog management; dispatches to `osinfo_window.cc` for OS Info window
- **converters.cc/h** - Temperature conversion algorithms in namespaces: `kelvin::`, `celsius::`, `fahrenheit::`, `rankine::`
- **utils.cc/h** - Command line parsing, numeric validation, precision handling, .ini settings handling
- **ui_utils.cc/h** - UI utility functions: tooltips, window rect helpers (`GetMainWinRect`, `GetDesktopRect`, `GetRightOfWindow`), `GetCacheSize()`, `SetCPUBarPos()`, MessageBox wrappers (`InfoBox`, `WarnBox`, `ErrorBox`), shell applet launcher
- **painting.cc/h** - Font creation and management (`CreateMainFont`, `GetFont`, `SetFontForControl`, `SetFontAllControls`)
- **osinfo_window.cc/h** - OS Info child window: window proc (`OsInfoWndProc`), controls init (`InitOsInfoControls`), resize handling (`HandleOsInfoResize`), OS version text output
- **strings.h** - Localized UI string constants; supports English (`AFX_TARG_ENU`), Filipino (`AFX_TARG_PH`), Russian (`AFX_TARG_RU`), Spanish (`AFX_TARG_ES`), with English fallback
- **globals.h** - Extern declarations for window handles and global state, including CPU/memory monitor bars (`hCPUBar`, `hMEMBar`), stressor controls (`hProgressBar`, `hThreadsEdit`, `hSSE2Checkbox`, `hCacheSizeCombo`, `hAllocMemButton`)
- **framework.h** - Precompiled header with Windows and STL includes
- **constants.h** - Physical constants for conversions, and UI layout metric constants
- **resource.h/rc** - Windows resources (icons, menus, dialogs, strings), and VERSIONINFO resource, using constants from version.h
- **version.h** - Version constants, and defines `_WIN32_WINNT=0x0500` to target Windows 2000 at a minimum

### CPU Stressor (`src/stress/`)
A subsystem for CPU and memory stress testing:
- **stress.cc/h** - Two stressor modes: `StressCPUVec()` (matrix/vector math targeting L2/L3 cache) and `StressCPUSSE2()` (inline SSE2 SIMD assembly); thread management (`LaunchThreads`, `StopAllThreads`); CPU monitor helpers (`StartCPUMon`, `StopCPUMon`, `PauseCPUMon`)
- **cpu.cc/h** - CPU usage monitoring via `NtQuerySystemInformation`; `GetCPUPercent()`, `MonitorCPU()`, `GetLogicalProcessorCount()`; uses `GetSystemTimes` on XP+ via `GetProcAddress`

### Logging Library (`src/logging`)
A static library that aims to make a small, self contained portable Win32 implementation of Chromium style LOG() macros and functions.
- **logging.cc/h** - Core `LogMessage` class (lazy: logs on destruction); `LOG()`, `CLOG()` (console only), `FLOG()` (file only), `DLOG()`, `VLOG()` macros; `InitLogging()`/`DeInitLogging()`; log levels: INFO, DEBUG, WARN, ERROR, FATAL, VERBOSE
- **logging_base.h** - `LogDest` enum and base types; minimal Windows/STL includes
- **console.cc/h** - Console window support for debug output
- **file_util.cc/h** - Log file creation and write utilities
- **check.cc/h** - Provides CHECK() and DCHECK() macros for asserting conditions, and intentionally crashing with a log message if failed

### OS Info DLL (`osinfo/`)
A separate DLL providing Windows NT version detection functions (Windows NT 4.0 - 11):
- **os_info.cc/h** - Version detection using RtlGetVersion
- **os_info_dll.h** - Exported API functions (`GetOSNameW`, `GetWinVersionW`, etc.)
- **resource.rc** - Provides VERSIONINFO resource for the DLL.
- **stdafx.h** - Precompiled header with Windows and STL includes
- **export.h** - DLL export macros
- **version.h** - Version constants for the resource.rc and `DllGetVersion()` in os_info.cc

### Control Flow
1. `wWinMain` initializes common controls, calls `InitLogging()`, and loads `osinfo.dll`
2. Command line parsed via `ParseCommandLine()`
3. Window class registered, main window created
4. `InitControls()` creates UI elements (temperature inputs, stressor controls, CPU/mem bars)
5. `HandleResize()` handles resizing of main window controls
6. `WindowProc()` handles messages, dispatches to control handlers
7. OS Info child window has its own `OsInfoWndProc()` and `HandleOsInfoResize()` in `osinfo_window.cc`
8. Temperature conversions use namespaced functions (e.g., `kelvin::fromCelsius()`)
9. CPU stressor threads launched via `LaunchThreads()`; `MonitorCPU()` runs on its own thread updating `hCPUBar`

## Key Patterns

- Unicode throughout (WCHAR, std::wstring, -municode)
- Windows subsystem version set to 5.00 (Windows 2000 minimum)
- When needed, functions from osinfo.dll are used to run the correct Win32 function
  depending on if the host system is Windows 2000, XP, or higher. For example using `IsAtLeast()` in src/controls.cc
- API functions that don't exist on Windows 2000 (e.g. `GetSystemTimes`, `GetNativeSystemInfo`) are loaded at runtime via `GetProcAddress()` with nullptr checks for graceful fallback
- C++17 standard
- Global handles declared extern in `globals.h`, defined in respective .cc files
- Conversion precision configurable via `SetCryoCalcPrecision()`
- UI strings are compile-time localized via `AFX_TARG_*` preprocessor defines in `strings.h`
- Logging initialized with `InitLogging()` at startup; use `LOG(INFO)`, `LOG(WARN)`, etc. throughout
