#include "utils.h"

#include "converters.h"

unsigned int g_precision_;

std::wstring& GetTempString(long double in_temperature) {
  std::wcout << __FUNC__ << in_temperature << L"\n\n";
  static const long double str = in_temperature;
  std::wostringstream wostr;
  static const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << str;
  static std::wstring retval = wostr.str();
  return retval;
}

std::wstring fromCelsius(long double in_celsius) {
  long double out_kelvin = kelvin::fromCelsius(in_celsius);
  long double out_fahrenheit = fahrenheit::fromCelsius(in_celsius);
  long double out_rankine = rankine::fromCelsius(in_celsius);
  std::wostringstream wostr;
  static const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_celsius << L" \u00B0 Celsius \n"
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
  static const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_kelvin << L" \u00B0 Kelvin \n"
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
  static const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_fahrenheit << L" \u00B0 Fahrenheit \n"
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
  static const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_rankine << L" \u00B0 Rankine \n"
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

const std::wstring GetExecutableName() {
  wchar_t path[MAX_PATH];
  DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);
  if (len == 0 || len == MAX_PATH) {
    return L""; // error or truncated
  }
  // Convert to std::wstring
  std::wstring fullPath(path);

  // Extract filename portion
  size_t pos = fullPath.find_last_of(L"\\/");
  if (pos == std::wstring::npos) {
    return fullPath; // no slash found
  }
  const std::wstring exec_name = fullPath.substr(pos + 1);
  return exec_name;
}

const std::wstring GetVersionWstring() {
  std::wostringstream wostr;
  wostr << VERSION_STRING;
  const std::wstring retval = wostr.str();
  return retval;
}

const int ShowVersionAndExit() {
  static const std::wstring ver = GetVersionWstring();
  std::wcout << L"\n CryoCalc ver. " << ver << std::endl;
  system("pause");
  return 0;
}

int ConfirmExit(HWND hWnd) {
  int user_response_code;
  user_response_code =
      MessageBoxW(nullptr, L"Are you sure you want to exit?", L"Confirm Exit",
                  MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON2);
  switch (user_response_code) {
    case IDNO:
    case IDCANCEL:
      break;
    case IDYES:
      DestroyWindow(hWnd);
    default:
      break;
  }
  return user_response_code;
}

bool ConfirmClearControls(HWND hWnd) {
  bool can_clear = false;
  int user_response_code =
      MessageBoxW(nullptr, L"Clear All Controls?", L"Confirm Clear",
                  MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
  switch (user_response_code) {
    case IDNO:
    case IDCANCEL:
      can_clear = false;
      break;
    case IDOK:
      can_clear = true;
      break;
    default:
      break;
  }
  return can_clear;
}

const int ShowHelpAndExit() {
  std::wostringstream wostr;
  std::wcout << L"\n " << GetExecutableName()
             << L" Usage: \n" << std::endl;
  wostr << L"   /d | -d | --debug : Enable Debug Mode \n"
        << L"   /v | -v | --ver   : Show Version Info \n"
        << L"   /? | -h | --help  : Show this Help \n" << std::endl;
  std::wcout << wostr.str();
  system("pause");
  return 0;
}

void HandleDebugMode() {
  std::wcout << L"This is CryoCalc ver. "
  << GetVersionWstring() << L" (Debug Mode)" << std::endl;
}

bool IsValidNumericInput(const wchar_t* text) {
  assert(text);
  bool decimalFound = false;
  for (const wchar_t* p = text; *p != L'\0'; ++p) {
    if (*p >= L'0' && *p <= L'9') {
      continue; // Safe numerals
    }
    if (*p == L'.') {
      if (decimalFound) { // decimal already found, only one allowed
        return false;
      }
      decimalFound = true; // Found a decimal
      continue;
    }
    // Any other character as invalid
    return false;
  }
  return true;
}

long double ConvertInputToLD(const wchar_t* input) {
  wchar_t* endPtr;
  long double retval;
  retval = std::wcstold(input, &endPtr);
  return retval;
}

void SetCryoCalcPrecision(unsigned int precision) {
  g_precision_ = precision;
}

const unsigned int GetCryoCalcPrecision() {
  const unsigned int retval = g_precision_;
  return retval;
}

const int GetPercentInt(const int in, const float percent) {
  if (percent > 1.0f || percent <= 0.0f) {
    std::wcerr << L"Percentage " << percent 
               << L" is too large or zero!" << std::endl;
    return in;    
  } else {
    const int retval = static_cast<int>(in * percent);
    return retval;
  }
}

const int GetXOffset(const int in, const int offset, const float percent) {
  const int init = GetPercentInt(in, percent);
  const int retval = init + offset;
  return retval;
}

const int GetYOffset(const int in, const int offset, const float percent) {
  const int init = GetPercentInt(in, percent);
  const int retval = init + offset;
  return retval;
}
