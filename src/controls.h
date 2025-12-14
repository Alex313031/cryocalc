#ifndef CRYOCALC_CONTROLS_H_
#define CRYOCALC_CONTROLS_H_

#include "utils.h"
#include "converters.h"
#include "framework.h"
#include "resource.h"
#include "globals.h"

extern bool _about_handled;

enum Scale {
  kScaleCelsius = 0,
  kScaleKelvin = 1,
  kScaleFahrenheit = 2,
  kScaleRankine = 3,
  kScaleUnknown = -1,
  kMaxScale = 4
};

Scale parseScale(const std::wstring& wscale);

// Handles the About button
bool AboutButtonClicked(HWND hWnd);

// Handles the Convert button
bool OnStartButtonClick(HWND hWnd);

// Handles conversion events
bool HandleConvert(HWND hWnd);

// Creates main controls
void InitControls(HWND hWnd, HINSTANCE hInst);

// Creates the status bar
void InitStatusBar(HWND hWnd, HINSTANCE hInst);

// Set client rects for future resizing
void SetClientRects(HWND hWnd, HINSTANCE hInst);

// Clears the input box
void ClearInput(HWND hWnd);

// Clears output controls for new calculation
void ClearControls(HWND hWnd);

// Handled input from the input edit box and temp selector control
bool InputEntered(HWND hWnd);

// Gets extern _about_handled above which is
// set if about dialog completes successfully.
bool GetAboutHandledState();

// Sets extern _about_handled
void SetAboutHandled(bool handled);

// Handles the About dialog
bool ShowAboutDialog(HWND hWnd);

// Message handler for the "About" dialog box.
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

#endif // CRYOCALC_CONTROLS_H_
