#ifndef CRYOCALC_GLOBALS_H_
#define CRYOCALC_GLOBALS_H_

// Should only depend on framework.h
#include "framework.h"

/* Global Variables */

// current instance
extern HINSTANCE hInst;

// Edit control handles
extern HWND hInputEdit;
extern HWND hTempSelectEdit;
extern HWND hPrecisionEdit;
extern HWND hCelsiusEdit;
extern HWND hKelvinEdit;
extern HWND hFahrenheitEdit;
extern HWND hRankineEdit;

// Convert button handle
extern HWND hConvButton;

// Clear button handle
extern HWND hClearButton;

// Input box for CPU stressor for number of threads
extern HWND hThreadsEdit;

// Start button handle for CPU stresser
extern HWND hStartStresButton; // Intentionally called stres to make fun of
                               // winternals program called CPUSTRES.exe with
                               // product name of "Cpustrese.exe". LOL

// Stop button handle for CPU stresser
extern HWND hStopStresButton;

// About button handle
extern HWND hAboutButton;

// Our status bar
extern HWND hStatusBar;

// Gets our global hInst for the main Window
extern HINSTANCE GetGlobalHinst();

#endif // CRYOCALC_GLOBALS_H_
