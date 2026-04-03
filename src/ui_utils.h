#ifndef CRYOCALC_UI_UTILS_H_
#define CRYOCALC_UI_UTILS_H_

#include "utils.h"

typedef int(WINAPI* RUN_FILE_DLG_)(HWND hwndParent,
                                   HICON hIcon,
                                   LPCTSTR lpszWorkingDir,
                                   LPCTSTR lpszTitle,
                                   LPCTSTR lpszPrompt,
                                   DWORD dwFlags);

// Adds a tooltip to a control.
HWND AddTooltip(HWND hWndParent, HWND hWndControl, HINSTANCE hInst, const wchar_t* tooltipText);

// Set client rects for future resizing of a Window
bool SetClientRects(HWND hWnd, HINSTANCE hInst);

// Gets the RECT of the entire Window, including titlebar
// Must call SetClientRects() before running this
const RECT GetMainWinRect();

// Gets the RECT of the client area of Window, excluding titlebar
// Must call SetClientRects() before running this
const RECT GetMainClientRect();

// Gets the size of the desktop window (usually the screen), to position app in the middle of it
const RECT GetDesktopRect(HINSTANCE hInstance);

// Initializes menu states
void InitMenus(HWND hWnd);

// Launches local .chm help file (if any)
bool LaunchHelp(HWND hWnd);

// Gets the position to place a window to the right of another window
void GetRightOfWindow(HWND hWnd, int* outX, int* outY);

// Confirms the user wants to clear the controls, doesn't actually clear them.
bool ConfirmClearControls(HWND hWnd);

// Brings window to foreground, and optional second param sets keyboard focus
bool ResetFocus(HWND foreground, HWND keyb_focus);

// Opens the "Run" applet
void OpenRunDialog(HWND hWnd);

// Launcher function to open shell applets
bool RunShellApplet(HWND hWnd, const wchar_t* executable);

// Get default cache multiplier to use, from hCacheSizeCombo
const size_t GetCacheSize();

// Helper functions for MessageBoxW
int InfoBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

int WarnBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

int ErrorBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

// Handles child window clicks
bool HandleChildClick(HWND parent, HWND child);

// Gets _about_handled which is set if
// about dialog completes successfully.
bool GetAboutHandledState();

// Sets extern _about_handled
void SetAboutHandled(bool handled);

// Handles the About dialog
bool ShowAboutDialog(HWND hWnd);

// Message handler for the "About" dialog box.
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif // CRYOCALC_UI_UTILS_H_
