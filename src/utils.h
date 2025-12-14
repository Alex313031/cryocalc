#ifndef CRYOCALC_UTILS_H_
#define CRYOCALC_UTILS_H_

#include <cassert>

#include "constants.h"
#include "converters.h"
#include "framework.h"

extern unsigned int g_precision_;

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

// Confirms whether the user wants to exit.
int ConfirmExit(HWND hWnd);

// Confirms the user wants to clear the controls
bool ConfirmClearControls(HWND hWnd);

// Shows commandline usage and quits
const int ShowHelpAndExit();

// Handles the debug mode command line flag
void HandleDebugMode();

// Checks that input only contains 0 - 9 and optionally
// a decimal point, excluding text and special symbols.
bool IsValidNumericInput(const wchar_t* text);

// Converts input with decimal point to long double representation
// for feeding into the actual conversion algorithms.
long double ConvertInputToLD(const wchar_t* input);

void SetCryoCalcPrecision(unsigned int precision);

const unsigned int GetCryoCalcPrecision();

const int GetPercentInt(const int in, const float percent);

const int GetXOffset(const int in, const int offset, const float percent);

const int GetYOffset(const int in, const int offset, const float percent);

#endif // CRYOCALC_UTILS_H_
