#ifndef CRYOCALC_H_
#define CRYOCALC_H_

// This file is for separating out declarations from the main application about_win.cc file
// Use it for forward declaring the main window processing functions, extern keywords for
// handles, and system DLL pointer types.

#include "framework.h"

#include "resource.h"

// Maximum size of string that can be loaded from resource table with LoadString().
#define MAX_LOADSTRING 255

/* Global Variables */

// current instance
extern HINSTANCE hInst;

// Edit control handles
extern HWND hCelsiusEdit;
extern HWND hKelvinEdit;
extern HWND hFahrenheitEdit;
extern HWND hRankineEdit;

// Dummy file output for conhost
static FILE* fDummyFile;

// The title bar text
static const WCHAR* CAPTION_TITLE = L"CryoCalc";
static const WCHAR* CONV_BUTTON = L"Convert";

// The main window class name
static WCHAR szWindowClass[MAX_LOADSTRING];

/* End of global variables */

/* Forward declarations of functions included in this translation unit. */

// Forward declarations

bool OnStartButtonClick(HWND hWnd);

// Creates the main window with CreateWindowW()
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

// Window procedure function https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Registers the window class.
ATOM RegisterWndClass(HINSTANCE hInstance);

// Message handler for the "About" dialog box.
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

#endif // CRYOCALC_H_
