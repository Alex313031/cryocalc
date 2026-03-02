#include "converters.h"

#include "framework.h"

#include <check.h>
#include <logging.h>

/* functions that return Kelvin */
long double kelvin::fromCelsius(long double in_celsius) {
  long double retval = in_celsius + KELVIN_OFFSET;
  return retval;
}

long double kelvin::fromFahrenheit(long double in_fahrenheit) {
  long double retval = (in_fahrenheit - 32.0L) * 5.0L / 9.0L + KELVIN_OFFSET;
  return retval;
}

long double kelvin::fromRankine(long double in_rankine) {
  long double retval = in_rankine * (5.0L / 9.0L);
  return retval;
}
/* end Kelvin functions */

/* functions that return Celsius */
long double celsius::fromKelvin(long double in_kelvin) {
  long double retval = in_kelvin - KELVIN_OFFSET;
  return retval;
}

long double celsius::fromFahrenheit(long double in_fahrenheit) {
  long double retval = (in_fahrenheit - 32.0L) * 5.0L / 9.0L;
  return retval;
}

long double celsius::fromRankine(long double in_rankine) {
  long double kTempFahrenheit = fahrenheit::fromRankine(in_rankine);
  long double retval          = celsius::fromFahrenheit(kTempFahrenheit);
  return retval;
}
/* end Celsius functions */

/* functions that return Fahrenheit */
long double fahrenheit::fromCelsius(long double in_celsius) {
  long double retval = (in_celsius * 9.0L / 5.0L) + 32.0L;
  return retval;
}

long double fahrenheit::fromKelvin(long double in_kelvin) {
  long double kTempCelsius = celsius::fromKelvin(in_kelvin);
  long double retval       = fahrenheit::fromCelsius(kTempCelsius);
  return retval;
}

long double fahrenheit::fromRankine(long double in_rankine) {
  long double retval = in_rankine - RANKINE_OFFSET;
  return retval;
}
/* end Fahrenheit functions */

/* functions that return Rankine */
long double rankine::fromCelsius(long double in_celsius) {
  long double kTempKelvin = kelvin::fromCelsius(in_celsius);
  long double retval      = rankine::fromKelvin(kTempKelvin);
  return retval;
}

long double rankine::fromKelvin(long double in_kelvin) {
  long double retval = in_kelvin * (9.0L / 5.0L);
  return retval;
}

long double rankine::fromFahrenheit(long double in_fahrenheit) {
  long double retval = in_fahrenheit + RANKINE_OFFSET;
  return retval;
}
/* end Rankine functions */

// Check that input temp makes sense, that it isn't lower than absolute zero or higher than
// what long double can store.
bool CheckInputTempBounds(Scale in_scale, long double in_temp) {
  bool impossible_temp = false;
  if (in_temp > max_temp) {
    LOG(ERROR) << L"Input temp was higher than " << max_temp;
    return false;
  }
  static constexpr long double min_c = -273.16L;
  static constexpr long double min_f = -459.68L;
  static constexpr long double min_abs = 0;
  switch (in_scale) {
    case kScaleCelsius: {
      if (in_temp < min_c) {
        impossible_temp = true;
        LOG(ERROR) << L"Celsius input temp was lower than than " << min_c;
      }
    } break;
    case kScaleKelvin:
      if (in_temp < min_abs) {
        impossible_temp = true;
        LOG(ERROR) << L"Kelvin input temp was lower than than " << min_abs;
      }
      break;
    case kScaleFahrenheit:
      if (in_temp < min_f) {
        impossible_temp = true;
        LOG(ERROR) << L"Fahrenheit input temp was lower than than " << min_f;
      }
      break;
    case kScaleRankine:
      if (in_temp < min_abs) {
        impossible_temp = true;
        LOG(ERROR) << L"Rankine input temp was lower than than " << min_abs;
      }
      break;
    default:
      NOTREACHED();
  }
  return !impossible_temp;
}
