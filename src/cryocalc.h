#ifndef CRYOCALC_H_
#define CRYOCALC_H_

#include "globals.h"

// This file is for separating out declarations from the main application about_win.cc file
// Use it for forward declaring the main window processing functions, extern keywords for
// handles, and system DLL pointer types.

#include "controls.h"
#include "framework.h"
#include "painting.h"
#include "resource.h"

// Current instance
extern HINSTANCE hInst;

// Creates the main window with CreateWindowExW()
bool InitInstance(HINSTANCE hInstance, int nCmdShow);

// Handles about button being clicked
bool AboutButtonClicked(HWND hWnd);

// Opens link to online help
bool LaunchHelpEx(HWND hWnd);

// Registers the window class.
ATOM RegisterWndClass(HINSTANCE hInstance);

// Main function, entry point for any GUI Win32 App
extern "C" int APIENTRY wWinMain(HINSTANCE hInstance,
                                 HINSTANCE hPrevInstance,
                                 LPWSTR lpCmdLine,
                                 int nCmdShow);

#endif // CRYOCALC_H_
