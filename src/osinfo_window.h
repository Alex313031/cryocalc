#ifndef CRYOCALC_OSINFO_WINDOW_H_
#define CRYOCALC_OSINFO_WINDOW_H_

#include "framework.h"
#include "globals.h"
#include "resource.h"
#include "strings.h"
#include "utils.h"

// Window class name for osinfo popup window
extern const WCHAR* szOSInfoWindowClass;

constexpr unsigned int OSINFO_WIDTH  = 380u; // Width of OS Info Window
constexpr unsigned int OSINFO_HEIGHT = 300u; // Height of OS Info window

// Our osinfo window has no menu, but it does have a status bar
extern HWND hOsInfoTextOut;
extern HWND hOsInfoStatusBar;

// Buttons to launch shell applets related to Windows version
extern HWND hWinVerButton;
extern HWND hMsInfoButton;
extern HWND hRunAppButton;
extern HWND hCloseOSInfoButton;

// Tests osinfo.dll by showing popup window with OS version info.
bool ShowOsInfo(HWND hWnd);

// Initializes controls in Os Info Window
void InitOsInfoControls(HWND hWnd, HINSTANCE hInst);

// Handles resizing Os Info controls
void HandleOsInfoResize(HWND hWnd);

// Window procedure for osinfo popup window
LRESULT CALLBACK OsInfoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Gets WINNT version string(s) to output to edit control
std::wstring GetWinInfo();

// Creates edit control and outputs Windows NT version info
void OutputOsInfo(HWND hWnd);

// Logs OS info to the console
void LogOsInfo();

// Tests the DllGetVersion function in osinfo.dll
bool TestDllGetVersion();

#endif // CRYOCALC_OSINFO_WINDOW_H_
