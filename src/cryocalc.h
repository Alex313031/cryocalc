#ifndef CRYOCALC_H_
#define CRYOCALC_H_

#include "globals.h"

// This file is for separating out declarations from the main application about_win.cc file
// Use it for forward declaring the main window processing functions, extern keywords for
// handles, and system DLL pointer types.

#include "controls.h"
#include "framework.h"
#include "resource.h"

// Maximum size of string that can be loaded from resource table with LoadString().
#define MAX_LOADSTRING 255

// Dummy file output for conhost
static FILE* fDummyFile;

// Bools set by parsing commandline to control further program behavior
extern bool debug_mode;
extern bool show_version;
extern bool show_help;

// The title bar text
static const WCHAR* CAPTION_TITLE = L"CryoCalc";

// The main window class name
static WCHAR szWindowClass[MAX_LOADSTRING];

// Creates the main window with CreateWindowW()
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

// Handles about button being clicked
bool AboutButtonClicked(HWND hWnd);

// Launches help (if any)
bool LaunchHelp(HWND hWnd);

// Window procedure function https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Registers the window class.
ATOM RegisterWndClass(HINSTANCE hInstance);

#endif // CRYOCALC_H_
