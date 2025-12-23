#ifndef CRYOCALC_OSINFO_H_
#define CRYOCALC_OSINFO_H_

#include "framework.h"
#include "resource.h"
#include "globals.h"
#include "utils.h"

// Window class name for osinfo popup window
extern const WCHAR* szOSInfoWindowClass;

// Our osinfo window has no menu, but it does have a status bar
extern HWND hOsInfoTextOut;
extern HWND hOsInfoStatusBar;

// Tests osinfo.dll by showing popup window with OS version info.
bool ShowOsInfo(HWND hWnd);

// Window procedure for osinfo popup window
LRESULT CALLBACK OsInfoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Gets WINNT version string(s) to output to edit control
std::wstring GetWinInfo();

// Creates edit control and outputs Windows NT version info
void OutputOsInfo(HWND hWnd);

// Logs OS info to the console
void LogOsInfo();

#endif // CRYOCALC_OSINFO_H_
