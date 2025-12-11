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

// About button handle
extern HWND hAboutButton;

// Gets our global hInst for the main Window
extern HINSTANCE GetGlobalHinst();

#endif // CRYOCALC_GLOBALS_H_
