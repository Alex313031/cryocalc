#ifndef CRYOCALC_UTILS_H_
#define CRYOCALC_UTILS_H_

#include <cassert>

#include "constants.h"
#include "converters.h"
#include "framework.h"
#include "globals.h"
#include "logging/logging.h"
#include "stress/stress.h"

#include <os_info_dll.h>

// Global precision to use
extern unsigned int g_precision_;

// Bools set by parsing commandline to control further program behavior
extern bool debug_mode;
extern bool enable_logging;
extern bool show_version;
extern bool show_help;

// Typedefs for accessing system .dll functions through GetProcAddress()
#define RFD_NOBROWSE            0x00000001
#define RFD_NODEFFILE           0x00000002
#define RFD_USEFULLPATHDIR      0x00000004
#define RFD_NOSHOWOPEN          0x00000008
#define RFD_WOW_APP             0x00000010
#define RFD_NOSEPMEMORY_BOX     0x00000020
typedef void (WINAPI *GET_NATIVE_SYSTEM_INFO_)(SYSTEM_INFO* lpSystemInfo);
typedef void (WINAPI *GET_NATIVE_SYSTEM_INFO_)(SYSTEM_INFO* lpSystemInfo);
typedef int (* RUN_FILE_DLG_)(HWND hwndParent, HICON hIcon, LPCTSTR lpszWorkingDir, LPCTSTR lpszTitle, LPCTSTR lpszPrompt, DWORD dwFlags);

// Temperature helper functions
std::wstring& GetTempString(long double in_temperature);

std::wstring fromCelsius(long double in_celsius);

std::wstring fromKelvin(long double in_kelvin);

std::wstring fromFahrenheit(long double in_fahrenheit);

std::wstring fromRankine(long double in_rankine);

// Runs test routines
bool runTests();

// UI Helper functions

// Returns the executable name, without path
const std::wstring GetExecutableName();

// Gets the version number as human readable wstring.
const std::wstring GetVersionWstring();

// Shows the version info and quits
const int ShowVersionAndExit();

// Closes all sub-windows, then exits app.
void CloseAllWindows(HWND hWnd);

// Confirms whether the user wants to exit.
int ConfirmExit(HWND hWnd);

// Confirms the user wants to clear the controls
bool ConfirmClearControls(HWND hWnd);

// Shows commandline usage and quits
const int ShowHelpAndExit();

// Handles the debug mode command line flag
void HandleDebugMode(const bool debug_mode);

// Checks that input only contains 0 - 9 and optionally
// a decimal point, excluding text and special symbols.
bool IsValidNumericInput(const wchar_t* text);

// Verifies that threads input is valid.
bool IsValidThreadsInput(const wchar_t* text);

// Gets the number of logical CPU threads of the host system.
DWORD GetLogicalProcessorCount();

// Gets the default number of threads to use for stressor: the number of logical CPUs within bounds of
// MIN_THREADS and MAX_THREADS.
unsigned int GetDefaultNumThreads();

// Converts input with decimal point to long double representation
// for feeding into the actual conversion algorithms.
long double ConvertInputToLD(const wchar_t* input);

// Appends lines of text to an edit control
void AppendTextToEditControl(HWND hWnd, const std::wstring line);

// Sets the precision to use for calculations (usually from hPrecisionCombo).
void SetCryoCalcPrecision(unsigned int precision);

// Gets the precision to use for calculations
const unsigned int GetCryoCalcPrecision();

// Simple percent calculator functions
const int GetPercentInt(const int in, const float percent);

const int GetXOffset(const int in, const int offset, const float percent);

const int GetYOffset(const int in, const int offset, const float percent);

// Parses command line arguments and sets bools as necessary
bool ParseCommandLine(int argc, LPWSTR argv[]);

// Returns an HINSTANCE for a given window HWND
HINSTANCE GetInstanceFromHwnd(HWND hWnd);

// Opens the "Run" applet
void OpenRunDialog(HWND hWnd);

// Launcher function to open shell applets
bool RunShellApplet(HWND hWnd, const wchar_t* executable);

// Get default cache multiplier to use, from hCacheSizeCombo
const size_t GetCacheSize();

// Adds a tooltip to a control.
HWND AddTooltip(HWND hWndParent, HWND hWndControl, HINSTANCE hInst, const wchar_t* tooltipText);

// Detaches console to allow attaching a new one. See AttachConsole() in cryocalc.cc
void DetachConsole(HWND hWnd);

// Clears console output without detaching it.
void ClearConsole(HWND hWnd);

// Gets the position to place a window to the right of another window
void GetRightOfWindow(HWND hWnd, int* outX, int* outY);

#endif // CRYOCALC_UTILS_H_
