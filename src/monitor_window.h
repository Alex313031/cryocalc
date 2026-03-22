#ifndef CRYOCALC_MONITOR_WINDOW_H_
#define CRYOCALC_MONITOR_WINDOW_H_

#include "constants.h"
#include "globals.h"
#include "resource.h"
#include "utils.h"

// Window class name for monitor popup window
extern const WCHAR* szMonitorWindowClass;

static inline constexpr int kDesiredClientW = MONWIN_WIDTH;
static inline constexpr int kDesiredClientH = MONWIN_HEIGHT;
// Minimum outer window dimensions
static inline constexpr int kMinOuterW = 180;
static inline constexpr int kMinOuterH = 160;

// Status bar for showing percentages
extern HWND hMonitorStatusBar;

// Opens a monitor window to graph either CPU, RAM, Commit Charge, or I/O
bool OpenMonitorWindow(UINT type, HWND hWnd);

// Handles resizing Os Info controls
void HandleMonitorWindowResize(HWND hWnd);

// Window procedure for monitor popup window
LRESULT CALLBACK MonitorWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Create GDI resources used by the graph drawing routines.
void InitMeters();

// Append one CPU sample point (0-100%) to the scrolling history.
// Call once per timer tick, after UpdatePerfData().
void PushCpuSample(int percent);

// Paint the CPU line graph into the double-buffered HDC, confined to
// the RECT. top-left should be 0, 0.
void DrawMeters(HDC hdc, const RECT& area);

// Release GDI resources created by InitMeters().
// Call once from WM_DESTROY.
void CleanupMeters();

#endif // CRYOCALC_MONITOR_WINDOW_H_
