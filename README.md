# CryoCalc &nbsp;<img src="./assets/icon_256.png" width="38">

![screenshot](./assets/screenshot.png?raw=true "Screenshot")

## About

A small Windows 2000+ compatible program that converts between temperature units, for cryogenic work and LN2 overclocking.  
It has a CPU stresser/burner feature similar to cpuburner.exe from [Furmark](https://geeks3d.com/public/jegx/2017q2/furmark-1-19-geforce-gtx1070-stress-test.jpg).  
I recently also added some system monitoring functionality including a cool mini "Task Manager" style System Monitor window.

### Motivation
The app is written in raw Win32 API, using C++.
I wanted it to be an exercise in C++ and Win32, to make a "complete" app with menus, buttons, status bar, an installer, etc.
It is compiled using a custom MinGW toolchain to support Windows 2000/XP, and is written to support these old OSes.  
Despite being written for legacy Windows NT, it uses modern C++17. The reason it does not use newer standards is due to bugs
in both MSVC and MinGW in the STL, and indeed the v141_xp toolset in Visual Studio will not even let you target C++20 or higher.

## Building

### Via MinGW
Use the Makefile or `build_mingw.sh`. It should work on Linux, and with MinGW on Windows.  
You can also use my fork of [w64devkit](https://github.com/skeeto/w64devkit), called [win32-devkit](https://github.com/Alex313031/win32-devkit).

Using this method, you can compile on Linux, or Windows XP+ (using win32-devkit).

```
make -B all -j# (where # is number of jobs)

```

### With GN/Ninja
[Chromium](https://www.chromium.org) uses a build system with [GN](https://gn.googlesource.com/gn/+/refs/heads/main/README.md) and [Ninja](https://ninja-build.org/).

I have made a minimal, modified version configured specifically for compiling Win32 programs
for legacy Windows called [gn-legacy](https://github.com/Alex313031/gn-legacy).  
It can be used on Windows 7+ or Linux. (Unlike the regular MinGW method above, gn.exe does not work on Windows XP/Vista.)

Really, it is a meta-build system. GN stands for "Generate Ninja" and can use __BUILD.gn__ files to
generate `.ninja` files. These are used by Ninja (the actual build system), to run the commands to compile it.  
The compiler itself is dependant on the host platform:  
On Linux special MinGW build I compiled on Ubuntu 24.04 to support legacy Windows and use static linkage is used.
On Windows, it simply uses an extracted toolchain from win32-devkit mentioned above.

### With Visual Studio
Only Visual Studio 2017/2019/2022 are supported.  
 - (Broken right now, need to add configs for osinfo.dll).  
Open the `CryoCalc.sln` file. You will need the [v141_xp toolchain](https://learn.microsoft.com/en-us/cpp/build/configuring-programs-for-windows-xp) installed to support Windows XP.
