#ifndef CRYOCALC_UTILS_H_
#define CRYOCALC_UTILS_H_

#include "constants.h"
#include "converters.h"
#include "framework.h"

std::wstring& GetTempString(long double in_temperature);

std::wstring fromCelsius(long double in_celsius);

std::wstring fromKelvin(long double in_kelvin);

std::wstring fromFahrenheit(long double in_fahrenheit);

std::wstring fromRankine(long double in_rankine);

bool runTests();

#endif // CRYOCALC_UTILS_H_
