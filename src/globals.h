#ifndef CRYOCALC_GLOBALS_H_
#define CRYOCALC_GLOBALS_H_

// Should only depend on framework.h
#include "framework.h"

// Maximum size of string that can be loaded from resource table with LoadString().
#define MAX_LOADSTRING 255

/* Global Variables */

// Main Window handle
extern HWND hMainWindow;

// Main Window class name
extern WCHAR szWindowClass[MAX_LOADSTRING];

// Module handle to osinfo.dll
extern HMODULE hOsInfoDll;

// Handle to cryocalc.ini file
extern HANDLE g_ini_file;

// Edit control handles
extern HWND hInputEdit;
extern HWND hTempSelectCombo;
extern HWND hPrecisionCombo;
extern HWND hCelsiusEdit;
extern HWND hKelvinEdit;
extern HWND hFahrenheitEdit;
extern HWND hRankineEdit;

extern HWND hOsInfoWin; // Window handle to OS Info Window

extern HWND hMonitorWin; // Window handle to system resource monitor Window

extern HWND hOsInfoTextOut; // Static text edit control for logging os info.

extern HWND hConvButton; // Convert button handle

extern HWND hClearButton; // Clear button handle

extern HWND hThreadsEdit; // Input box for CPU stressor for number of threads

extern HWND hProgressBar; // Progress bar for CPU stressor

extern HWND hCPUBar; // Vertical progress bar for CPU usage

extern HWND hCPUPercent; // Edit control showing CPU percentage usage

extern HWND hMEMBar; // Vertical progress bar for RAM usage

extern HWND hMemPercent; // Edit control showing RAM percentage usage

extern HWND hIOBar; // Vertical progress bar disk usage percent

extern HWND hIOPercent; // Edit control showing total disk usage %

extern HWND hCommitBar; // Vertical progress bar showing total RAM + Pagefile allocated usage (commit charge)

extern HWND hCommitPercent; // Edit control showing total RAM + Pagefile allocated percentage usage (commit charge)

extern HWND hCacheSizeCombo; // Combobox to select L2/L3 Cache multiplier

extern HWND hSSE2Checkbox; // Checkbox on whether to use the SSE2 version of the stressor.

extern HWND hAllocMemButton; // Button to allocate huge amounts of memory for RAM stressing.

// Start button handle for CPU stresser
extern HWND hStartStresButton; // Intentionally called stres to make fun of
                               // winternals program called CPUSTRES.exe with
                               // product name of "Cpustrese.exe". LOL

extern HWND hStopStresButton; // Stop button handle for CPU stresser

extern HWND hAboutButton; // About button handle

extern HWND hOsInfoButton; // Os Info button handle

extern HWND hStatusBar; // Our status bar

extern HINSTANCE GetGlobalHinst(); // Gets our global hInst for the main Window

#endif // CRYOCALC_GLOBALS_H_
