#include "converters.h"
#include "framework.h"

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
  long double retval = celsius::fromFahrenheit(kTempFahrenheit);
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
  long double retval = fahrenheit::fromCelsius(kTempCelsius);
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
  long double retval = rankine::fromKelvin(kTempKelvin);
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
