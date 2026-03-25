#ifndef CRYOCALC_MONITOR_WINDOW_H_
#define CRYOCALC_MONITOR_WINDOW_H_

#include "constants.h"
#include "globals.h"

// Window class name for monitor popup window
extern const WCHAR* szMonitorWindowClass;

static inline constexpr int kDesiredClientW = MONWIN_WIDTH;
static inline constexpr int kDesiredClientH = MONWIN_HEIGHT;

// Status bar for showing percentages
extern HWND hMonitorStatusBar;

// Current update interval in ms (kSpeedLow/Med/High). Shared between windows.
extern UINT g_update_interval;

// Restart the monitoring timer on the main window at the new rate and
// radio-check both speed menus. Safe to call from either window's handler.
void SetUpdateSpeed(UINT interval, UINT menu_id);

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
void PushSamples(float cpu_percent, float kernel_percent,
                 float ram_percent, float comm_percent, float io_percent);

// Draw a single meter for monitor window with only 1 graph, covering whole client area
void DrawMeter(HDC hdc, const RECT& area);

// Paint the 4 line graphs, each with a label and in its own quadrant relative
// to 0, 0 in client area.
void DrawMeters(HDC hdc, const RECT& area);

// Release GDI resources created by InitMeters().
// Call once from WM_DESTROY.
void CleanupMeters();

#endif // CRYOCALC_MONITOR_WINDOW_H_
