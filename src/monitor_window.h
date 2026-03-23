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

// Opens a monitor window to graph
bool OpenMonitorWindow(HWND hWnd);

// Handles resizing Os Info controls
void HandleMonitorWindowResize(HWND hWnd);

// Window procedure for monitor popup window
LRESULT CALLBACK MonitorWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Create GDI resources used by the graph drawing routines and status bar
void InitMeters(HWND hWnd);

// Append one sample point per counter (0.0–100.0) to the scrolling history.
// Call once per timer tick, after UpdatePerfData().
void PushSamples(float cpu_percent, float ram_percent, float comm_percent, float io_percent);

// Draw a single meter for monitor window with only 1 graph, covering whole client area
void DrawMeter(HDC hdc, const RECT& area);

// Paint the 4 line graphs, each with a label and in its own quadrant relative
// to 0, 0 in client area.
void DrawMeters(HDC hdc, const RECT& area);

// Release GDI resources created by InitMeters().
// Call once from WM_DESTROY.
void CleanupMeters();

#endif // CRYOCALC_MONITOR_WINDOW_H_
