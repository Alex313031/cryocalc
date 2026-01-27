#ifndef CRYOCALC_H_
#define CRYOCALC_H_

#include "globals.h"

// This file is for separating out declarations from the main application about_win.cc file
// Use it for forward declaring the main window processing functions, extern keywords for
// handles, and system DLL pointer types.

#include "controls.h"
#include "framework.h"
#include "resource.h"

// Creates the main window with CreateWindowW()
bool InitInstance(HINSTANCE hInstance, int nCmdShow);

// Handles about button being clicked
bool AboutButtonClicked(HWND hWnd);

// Launches local .chm help file (if any)
bool LaunchHelp(HWND hWnd);

// Opens link to online help
bool LaunchHelpEx(HWND hWnd);

// Registers the window class.
ATOM RegisterWndClass(HINSTANCE hInstance);

// Attaches console to window, only one allowed per HINSTANCE.
bool AttachConsole();

#endif // CRYOCALC_H_
