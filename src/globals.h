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
extern HWND hTempSelectCombo;
extern HWND hPrecisionCombo;
extern HWND hCelsiusEdit;
extern HWND hKelvinEdit;
extern HWND hFahrenheitEdit;
extern HWND hRankineEdit;

// Window handle to OS Info Window
extern HWND hOsInfoWin;

// Static text edit control for logging os info.
extern HWND hOsInfoTextOut;

extern RECT kMainWinRect;
extern RECT kMainClientRect;

// Convert button handle
extern HWND hConvButton;

// Clear button handle
extern HWND hClearButton;

// Input box for CPU stressor for number of threads
extern HWND hThreadsEdit;

// Progress bar for CPU stressor
extern HWND hProgressBar;

// Combobox to select L2/L3 Cache multiplier
extern HWND hCacheSizeCombo;

// Checkbox on whether to use the SSE2 version of the stressor.
extern HWND hSSE2Checkbox;

// Button to allocate huge amounts of memory for RAM stressing.
extern HWND hAllocMemButton;

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

extern HANDLE g_ini_file;

#endif // CRYOCALC_GLOBALS_H_
