#include "utils.h"

#include "converters.h"

std::wstring& GetTempString(long double in_temperature) {
  std::wcout << __FUNC__ << in_temperature << L"\n\n";
  static const long double str = in_temperature;
  std::wostringstream wostr;
  wostr << std::fixed << std::setprecision(8) << str;
  static std::wstring retval = wostr.str();
  return retval;
}

std::wstring fromCelsius(long double in_celsius) {
  long double out_kelvin = kelvin::fromCelsius(in_celsius);
  long double out_fahrenheit = fahrenheit::fromCelsius(in_celsius);
  long double out_rankine = rankine::fromCelsius(in_celsius);
  std::wostringstream wostr;
  wostr << std::fixed << std::setprecision(8) << "  " << in_celsius << L" \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  static std::wstring retval = wostr.str();
  return retval;
}

std::wstring fromKelvin(long double in_kelvin) {
  long double out_celsius = celsius::fromKelvin(in_kelvin);
  long double out_fahrenheit = fahrenheit::fromKelvin(in_kelvin);
  long double out_rankine = rankine::fromKelvin(in_kelvin);
  std::wostringstream wostr;
  wostr << std::fixed << std::setprecision(8) << "  " << in_kelvin << L" \u00B0 Kelvin \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  static std::wstring retval = wostr.str();
  return retval;
}

std::wstring fromFahrenheit(long double in_fahrenheit) {
  long double out_celsius = celsius::fromFahrenheit(in_fahrenheit);
  long double out_kelvin = kelvin::fromFahrenheit(in_fahrenheit);
  long double out_rankine = rankine::fromFahrenheit(in_fahrenheit);
  std::wostringstream wostr;
  wostr << std::fixed << std::setprecision(8) << "  " << in_fahrenheit << L" \u00B0 Fahrenheit \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  static std::wstring retval = wostr.str();
  return retval;
}

std::wstring fromRankine(long double in_rankine) {
  long double out_celsius = celsius::fromRankine(in_rankine);
  long double out_kelvin = kelvin::fromRankine(in_rankine);
  long double out_fahrenheit = fahrenheit::fromRankine(in_rankine);
  std::wostringstream wostr;
  wostr << std::fixed << std::setprecision(8) << "  " << in_rankine << L" \u00B0 Rankine \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n";
  static std::wstring retval = wostr.str();
  return retval;
}

bool runTests() {
  std::wstring testCelsius = fromCelsius(kDummyCelsius);
  std::wcout << "testCelsius: \n" << testCelsius << std::endl;

  std::wstring testFahrenheit = fromFahrenheit(kDummyFahrenheit);
  std::wcout << "testFahrenheit: \n" << testFahrenheit << std::endl;

  std::wstring testKelvin = fromKelvin(kDummyKelvin);
  std::wcout << "testKelvin: \n" << testKelvin << std::endl;

  std::wstring testRankine = fromRankine(kDummyRankine);
  std::wcout << "testRankine: \n" << testRankine << std::endl;

  return true;
}
