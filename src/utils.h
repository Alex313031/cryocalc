#ifndef CRYOCALC_UTILS_H_
#define CRYOCALC_UTILS_H_

#include <logging.h>

#include <cassert>

#include "constants.h"
#include "converters.h"
#include "framework.h"
#include "globals.h"

// Whether to force using std::random
static constexpr bool force_rand_s = false;

// Global precision to use
extern unsigned int g_precision_;

// Bools set by parsing commandline to control further program behavior
extern bool debug_mode;
extern bool enable_logging;
extern bool show_version;
extern bool show_help;

// For Run file dialog
#define RFD_NOBROWSE        0x00000001
#define RFD_NODEFFILE       0x00000002
#define RFD_USEFULLPATHDIR  0x00000004
#define RFD_NOSHOWOPEN      0x00000008
#define RFD_WOW_APP         0x00000010
#define RFD_NOSEPMEMORY_BOX 0x00000020

struct CustomSettings {
  bool set_debug_mode;
  unsigned int default_precision;
};

extern CustomSettings custom_settings;

extern bool got_settings;
extern bool set_settings;

extern unsigned int custom_precision;
extern bool custom_debug_mode;

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

// Closes all sub-windows, then exits app.
void CloseAllWindows(HWND hWnd);

// Confirms whether the user wants to exit.
int ConfirmExit(HWND hWnd);

// Confirms whether to clear the log file, and clears it if "Yes" is selected.
bool ConfirmAndClearLog(HWND hWnd);

// Shows the version info and quits
const int ShowVersionAndExit();

// Shows commandline usage and quits
const int ShowHelpAndExit();

// Handles the debug mode command line flag and DCHECK
void HandleDebugMode(const bool debug_mode, const bool dcheck_mode);

// Checks that input only contains 0 - 9 and optionally
// a decimal point, excluding text and special symbols.
bool IsValidNumericInput(const wchar_t* text);

// Verifies that threads input is valid.
bool IsValidThreadsInput(const wchar_t* text);

// Gets the default number of threads to use for stressor: the number of logical CPUs within bounds
// of MIN_THREADS and MAX_THREADS.
unsigned int GetDefaultNumThreads();

// Converts input with decimal point to long double representation
// for feeding into the actual conversion algorithms.
long double ConvertInputToLD(const wchar_t* input);

// Appends lines of text to an edit control
void AppendTextToEditControl(HWND hWnd, const std::wstring& line);

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

// Returns path to the main .exe, for finding files side by side with it.
const std::wstring GetExeDir();

// Opens log file
bool OpenLogFile(HWND hWnd, const std::wstring& file_path);

// Attaches console to window, only one allowed per process.
bool AttachConsole();

// Detaches console to allow attaching a new one.
bool DetachConsole();

// Clears console output without detaching it.
void ClearConsole(HWND hWnd);

// Clears log file, if any.
bool ClearLogFile(HWND hWnd);

// Gets, and verifies custom settings from a user created .ini file
bool GetCustomSettings();

// Sets our custom settings from .ini if we got them
bool SetCustomSettings();

// Opens an .ini file in read only mode to get settings
bool OpenIniFileForReading(const std::wstring& ini_file);

// Gets the default precision to use to set the intial state of the Precision combobox
const unsigned int GetDefaultPrecision();

// Gets if the user set debug mode in .ini file, takes precedence over command line flag.
const bool GetDefaultWantDebug();

// Compares whether the installed comctl32.dll is at least the supplied version
// Use DWORD to_compare = _PACKVERSION(major, minor)
const bool IsCommCtrlAtLeast(const DWORD to_compare);

// Gets the installed comctl32.dll version
DWORD GetCommCtrlVersion();

// Returns if this program is running under WINE, in which case certain assumptions are naught
bool IsRunningOnWine();

// Returns a random unsigned integer, using Win2K compatible functions if necessary
UINT RandomUint();

#endif // CRYOCALC_UTILS_H_
