#ifndef CRYOCALC_GLOBALS_H_
#define CRYOCALC_GLOBALS_H_

// Should only depend on framework.h
#include "framework.h"

// Maximum size of string that can be loaded from resource table with LoadString().
#define MAX_LOADSTRING 255

/* Global Variables */

// current instance
extern HINSTANCE hInst;

// Main Window handle
extern HWND hMainWindow;

// Main Window class name
extern WCHAR szWindowClass[MAX_LOADSTRING];

// Module handle to osinfo.dll
extern HMODULE hOsInfoDll;

// Edit control handles
extern HWND hInputEdit;
extern HWND hTempSelectEdit;
extern HWND hPrecisionEdit;
extern HWND hCelsiusEdit;
extern HWND hKelvinEdit;
extern HWND hFahrenheitEdit;
extern HWND hRankineEdit;

// Window handle to Os Info Window
extern HWND hOsInfoWin;

// Convert button handle
extern HWND hConvButton;

// Clear button handle
extern HWND hClearButton;

// Input box for CPU stressor for number of threads
extern HWND hThreadsEdit;

// Progress bar for CPU stressor
extern HWND hProgressBar;

// Start button handle for CPU stresser
extern HWND hStartStresButton; // Intentionally called stres to make fun of
                               // winternals program called CPUSTRES.exe with
                               // product name of "Cpustrese.exe". LOL

// Stop button handle for CPU stresser
extern HWND hStopStresButton;

// About button handle
extern HWND hAboutButton;

// Os Info button handle
extern HWND hOsInfoButton;

// Our status bar
extern HWND hStatusBar;

// Gets our global hInst for the main Window
extern HINSTANCE GetGlobalHinst();

#endif // CRYOCALC_GLOBALS_H_
