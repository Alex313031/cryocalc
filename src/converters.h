#ifndef CRYOCALC_CONVERTERS_H_
#define CRYOCALC_CONVERTERS_H_

#include <cfloat>

#include "constants.h"

// The max temp we can handle as input. Lower than Planck temp.
static inline constexpr long long max_temp = LLONG_MAX;

namespace kelvin {
  // Convert Celsius to Kelvin
  long double fromCelsius(long double in_celsius);

  // Convert Fahrenheit to Kelvin
  long double fromFahrenheit(long double in_fahrenheit);

  // Convert Rankine to Kelvin
  long double fromRankine(long double in_rankine);
} // namespace kelvin

namespace celsius {
  // Convert Kelvin to Celsius
  long double fromKelvin(long double in_kelvin);

  // Convert Fahrenheit to Celsius
  long double fromFahrenheit(long double in_fahrenheit);

  // Convert Rankine to Celsius
  long double fromRankine(long double in_rankine);
} // namespace celsius

namespace fahrenheit {
  // Convert Celsius to Fahrenheit
  long double fromCelsius(long double in_celsius);

  // Convert Kelvin to Fahrenheit
  long double fromKelvin(long double in_kelvin);

  // Convert Rankine to Fahrenheit
  long double fromRankine(long double in_rankine);
} // namespace fahrenheit

namespace rankine {
  // Convert Celsius to Rankine
  long double fromCelsius(long double in_celsius);

  // Convert Kelvin to Rankine
  long double fromKelvin(long double in_kelvin);

  // Convert Fahrenheit to Rankine
  long double fromFahrenheit(long double in_fahrenheit);
} // namespace rankine

// Check that input temp is within real world physical bounds
bool CheckInputTempBounds(Scale in_scale, long double in_temp);

#endif // CRYOCALC_CONVERTERS_H_
