#include "utils.h"

#include "converters.h"
#include "resource.h"
#include "strings.h"

unsigned int g_precision_;

static GET_NATIVE_SYSTEM_INFO_ pfnGetNativeSystemInfo = nullptr;
static RUN_FILE_DLG_ pfnRunFileDlg = nullptr;

std::wstring& GetTempString(long double in_temperature) {
  std::wcout << __FUNC__ << in_temperature << L"\n\n";
  std::wostringstream wostr;
  const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << in_temperature;
  static std::wstring retval;
  retval = wostr.str();
  return retval;
}

std::wstring fromCelsius(long double in_celsius) {
  long double out_kelvin = kelvin::fromCelsius(in_celsius);
  long double out_fahrenheit = fahrenheit::fromCelsius(in_celsius);
  long double out_rankine = rankine::fromCelsius(in_celsius);
  std::wostringstream wostr;
  const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_celsius << L" \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  return wostr.str();
}

std::wstring fromKelvin(long double in_kelvin) {
  long double out_celsius = celsius::fromKelvin(in_kelvin);
  long double out_fahrenheit = fahrenheit::fromKelvin(in_kelvin);
  long double out_rankine = rankine::fromKelvin(in_kelvin);
  std::wostringstream wostr;
  const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_kelvin << L" \u00B0 Kelvin \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  return wostr.str();
}

std::wstring fromFahrenheit(long double in_fahrenheit) {
  long double out_celsius = celsius::fromFahrenheit(in_fahrenheit);
  long double out_kelvin = kelvin::fromFahrenheit(in_fahrenheit);
  long double out_rankine = rankine::fromFahrenheit(in_fahrenheit);
  std::wostringstream wostr;
  const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_fahrenheit << L" \u00B0 Fahrenheit \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_rankine << L" = \u00B0 Rankine \n";
  return wostr.str();
}

std::wstring fromRankine(long double in_rankine) {
  long double out_celsius = celsius::fromRankine(in_rankine);
  long double out_kelvin = kelvin::fromRankine(in_rankine);
  long double out_fahrenheit = fahrenheit::fromRankine(in_rankine);
  std::wostringstream wostr;
  const unsigned int precision = GetCryoCalcPrecision();
  wostr << std::fixed << std::setprecision(precision) << "  " << in_rankine << L" \u00B0 Rankine \n"
        << "  " << out_celsius << L" = \u00B0 Celsius \n"
        << "  " << out_kelvin << L" = \u00B0 Kelvin \n"
        << "  " << out_fahrenheit << L" = \u00B0 Fahrenheit \n";
  return wostr.str();
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
  return wostr.str();
}

const int ShowVersionAndExit() {
  static const std::wstring ver = GetVersionWstring();
  std::wcout << L"\n " << kAppName << " ver. " << ver << std::endl;
  system("pause");
  return 0;
}

void CloseAllWindows(HWND hWnd) {
  // Tell OsInfo Window to close, if it's open.
  if (hOsInfoWin != nullptr) {
    PostMessageW(hOsInfoWin, WM_COMMAND, IDC_CLOSE_OSINFO, 0);
  }
  DetachConsole(hWnd);
  DestroyWindow(hWnd); // Send WM_DESTROY message to close main window. Bad practice.
}

int ConfirmExit(HWND hWnd) {
  int user_response_code =
      MessageBoxW(nullptr, L"Are you sure you want to exit?", L"Confirm Exit",
                  MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON1);
  switch (user_response_code) {
    case IDNO:
    case IDCANCEL:
      break;
    case IDYES:
      CloseAllWindows(hWnd);
      break;
    default:
      break;
  }
  return user_response_code;
}

bool ConfirmClearControls(HWND hWnd) {
  int user_response_code =
      MessageBoxW(nullptr, L"Clear All Temperature Output?", L"Confirm Clear",
                  MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
  switch (user_response_code) {
    case IDNO:
    case IDCANCEL:
      return false;
    case IDOK:
      return true;
    default:
      return false;
  }
}

const int ShowHelpAndExit() {
  std::wostringstream wostr;
  std::wcout << L"\n " << GetExecutableName()
             << L" Usage: \n" << std::endl;
  wostr << L"   /d | -d | --debug   : Enable debug mode and enable logging\n"
        << L"   /l | -l | --logging : Enable logging in console Window \n"
        << L"   /v | -v | --version : Show version info \n"
        << L"   /? | -h | --help    : Show this Help \n" << std::endl;
  std::wcout << wostr.str();
  system("pause");
  return 0;
}

void HandleDebugMode(const bool debug_mode) {
  std::wostringstream wostr;
  wostr << L"Welcome to " << kAppName << " ver. " << GetVersionWstring();
  if (debug_mode) {
    wostr << L" (Debug Mode)" << std::endl;
  } else {
    wostr << std::endl;
  }
  std::wcout << wostr.str();
}

bool IsValidNumericInput(const wchar_t* text) {
  assert(text);
  bool decimalFound = false;
  bool minusFound = false;
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
    // Same checking, but for minus symbol, for negative temp values.
    if (*p == L'-') {
      if (minusFound) {
        return false;
      }
      minusFound = true;
      continue;
    }
    // Any other character as invalid
    return false;
  }
  return true;
}

// Verifies that threads input is a whole integer
bool IsValidThreadsInput(const wchar_t* text) {
  assert(text);
    // Check that all characters are digits
  for (const wchar_t* p = text; *p != L'\0'; ++p) {
    if (*p < L'0' || *p > L'9') {
      return false;
    }
  }
  // Convert to integer
  wchar_t* end;
  const long value = wcstol(text, &end, 10);
  // Check conversion was successful (end should point to null terminator)
  if (*end != L'\0') {
    return false;
  }
  if (value == 0) {
    std::wcerr << L"Threads input was 0!" << std::endl;
  }
  // Check range [1, 128]
  return (value >= static_cast<long>(MIN_THREADS) && value <= static_cast<long>(MAX_THREADS));
}

DWORD GetLogicalProcessorCount() {
  SYSTEM_INFO sysInfo;
  std::wstring whichfunc;
#if _WIN32_WINNT >= 0x0502 && defined(_WIN64)
  whichfunc =  L"GetNativeSystemInfo";
  GetNativeSystemInfo(&sysInfo); // Directly run GetNativeSystemInfo
#else
  // Note: Do not call FreeLibrary() on GetModuleHandle() results
  HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
  if (!hKernel32) {
    // Fallback if kernel32.dll handle couldn't be obtained
    whichfunc = L"GetSystemInfo";
    GetSystemInfo(&sysInfo);
  } else {
    // Dynamically get GetNativeSystemInfo
    pfnGetNativeSystemInfo = reinterpret_cast<GET_NATIVE_SYSTEM_INFO_>(GetProcAddress(hKernel32, "GetNativeSystemInfo"));
    // Windows 2000 won't have this function, use GetSystemInfo instead
    if (pfnGetNativeSystemInfo) {
      whichfunc = L"pfnGetNativeSystemInfo";
      pfnGetNativeSystemInfo(&sysInfo);
    } else {
      whichfunc = L"GetSystemInfo";
      GetSystemInfo(&sysInfo);
    }
  }
  if (debug_mode) {
    std::wcout << L"Using " << whichfunc << " for " << __FUNC__ << std::endl;
  }
#endif
  return sysInfo.dwNumberOfProcessors;
}

unsigned int GetDefaultNumThreads() {
  unsigned int def_threads = 0;
  def_threads = static_cast<unsigned int>(GetLogicalProcessorCount());
  if (def_threads == 0) {
    return MIN_THREADS;
  } else if (def_threads > MAX_THREADS) {
    return MAX_THREADS;
  }
  return def_threads;
}

long double ConvertInputToLD(const wchar_t* input) {
  wchar_t* endPtr;
  long double retval;
  retval = std::wcstold(input, &endPtr);
  return retval;
}

void AppendTextToEditControl(HWND hWnd, const std::wstring line) {
  const WCHAR* text = line.c_str();
  int length = GetWindowTextLength(hWnd); // Get current text length
  SendMessageW(hWnd, EM_SETSEL, static_cast<WPARAM>(length), static_cast<LPARAM>(length)); // Set cursor at the end
  SendMessageW(hWnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text)); // Append the text
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
  const int retval = GetPercentInt(in, percent) + offset;
  return retval;
}

const int GetYOffset(const int in, const int offset, const float percent) {
  const int retval = GetPercentInt(in, percent) + offset;
  return retval;
}

bool debug_mode = false;
bool enable_logging = false;
bool show_version = false;
bool show_help = false;

bool ParseCommandLine(int argc, LPWSTR argv[]) {
  bool parsed = false;
  bool is_debug_mode =
#if defined(_DEBUG) || defined(DEBUG)
    true;
#else
    false;
#endif
  bool is_version_mode = false;
  bool is_help_mode = false;
  bool is_log_mode =
#if defined(CRYOCALC_LOGBYDEFAULT)
    true;
#else
    false;
#endif
  if (argv) {
    for (int i = 1; i < argc; ++i) { // start at 1 (skip .exe path)
      wchar_t* arg = argv[i];
      is_debug_mode =
          (((wcscmp(arg, L"--debug") == 0) || (wcscmp(arg, L"-d") == 0) || (wcscmp(arg, L"-debug") == 0)
           || (wcscmp(arg, L"/d") == 0) || (wcscmp(arg, L"/D") == 0)) && !(wcscmp(arg, L"--no-debug") == 0));
      is_log_mode =
          ((wcscmp(arg, L"--logging") == 0) || (wcscmp(arg, L"-l") == 0) || (wcscmp(arg, L"-log") == 0)
           || (wcscmp(arg, L"/l") == 0) || (wcscmp(arg, L"/L") == 0));
      is_version_mode =
          ((wcscmp(arg, L"--version") == 0) || (wcscmp(arg, L"-v") == 0) || (wcscmp(arg, L"-ver")) == 0
           || (wcscmp(arg, L"/v") == 0) || (wcscmp(arg, L"/V") == 0));
      is_help_mode =
          ((wcscmp(arg, L"--help") == 0) || (wcscmp(arg, L"-h") == 0) || (wcscmp(arg, L"-?") == 0)
           || (wcscmp(arg, L"/h") == 0) || (wcscmp(arg, L"/H") == 0) || (wcscmp(arg, L"/?") == 0));
    }
    parsed = true;
  } else {
    parsed = false;
  }
  if (is_version_mode && !is_help_mode) {
    show_version = true;
  }
  if (is_help_mode) {
    show_help = true;
  }
  if (is_debug_mode) {
    debug_mode = true;
  }
  if (is_log_mode || is_debug_mode || is_version_mode || is_help_mode) {
    enable_logging = true;
  }
  return parsed;
}

HINSTANCE GetInstanceFromHwnd(HWND hWnd) {
  // GetWindowLongPtr is the recommended function for 64-bit compatibility
  LONG_PTR hInstancePtr = GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
  // Cast the result to HINSTANCE
  HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(hInstancePtr);

  return hInstance;
}

// Opens the "Run" shell dialog from shell32.dll
void OpenRunDialog(HWND hWnd) {
  static HICON kSmallIcon = LoadIcon(GetInstanceFromHwnd(hWnd), MAKEINTRESOURCE(IDI_WINFLAG));
  if (kSmallIcon) {
    wchar_t szCurDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, szCurDir);
    // Open "Run"
    HMODULE hShell32Dll = GetModuleHandleW(kShell32Dll);
    if (hShell32Dll) {
      pfnRunFileDlg = reinterpret_cast<RUN_FILE_DLG_>(GetProcAddress(hShell32Dll, (LPCSTR)61));
      if (pfnRunFileDlg) {
        pfnRunFileDlg(hWnd, kSmallIcon, (LPWSTR)szCurDir, RUN_TITLE, RUN_PROMPT, RFD_USEFULLPATHDIR | RFD_WOW_APP);
      } else {
        std::wcerr << L"Failed to open run dialog." << std::endl;
      }
    } else {
      std::wcerr << L"Failed to get shell32.dll handle." << std::endl;
    }
    DestroyIcon(kSmallIcon); // Cleanup icon
  }
}

// Run any shell app
bool RunShellApplet(HWND hWnd, const wchar_t* executable) {
  bool success = false;
  std::wcout << L"Running " << executable << std::endl;
  HINSTANCE result = ShellExecuteW(hWnd, L"open", executable, nullptr, nullptr, SW_NORMAL);
  std::wostringstream wostr;
  if (reinterpret_cast<INT_PTR>(result) <= 32) {
    DWORD error = GetLastError();
    wostr << L"Opening " << executable << " failed! \n";
    if (error == ERROR_FILE_NOT_FOUND) {
      wostr << executable << L" could not be found." << std::endl;
    } else {
      wostr << L"Error = " << std::showbase << std::hex << error
            << std::dec << std::defaultfloat << std::endl;
    }
    const std::wstring warn = wostr.str();
    std::wcerr << warn;
    MessageBoxW(hWnd, warn.c_str(), L"Error", MB_OK | MB_ICONERROR);
    success = false;
  } else {
    success = true;
  }
  wostr.str(L"");
  wostr.clear();
  return success;
}

const size_t GetCacheSize() {
  size_t cache_size = 1024u;
  DWORD dwCacheComboSize = GetWindowTextLength(hCacheSizeCombo);
  std::wstring cachesz_buff(dwCacheComboSize + 1, L'\0');
  GetWindowTextW(hCacheSizeCombo, &cachesz_buff[0], dwCacheComboSize + 1);
  if ((wcscmp(cachesz_buff.c_str(), L"1MB") == 0)) {
    cache_size = 1024u;
  } else if ((wcscmp(cachesz_buff.c_str(), L"2MB") == 0)) {
    cache_size = 2048u;
  } else if ((wcscmp(cachesz_buff.c_str(), L"3MB") == 0)) {
    cache_size = 3072u;
  } else if ((wcscmp(cachesz_buff.c_str(), L"4MB") == 0)) {
    cache_size = 4096u;
  } else {
    std::wcerr << L"Default cache size was used!" << std::endl;
    cache_size = 1024u;
  }
  return cache_size;
}

HWND AddTooltip(HWND hWndParent, HWND hWndControl, HINSTANCE hInst, const wchar_t* tooltipText) {
  if (!hWndParent || !hWndControl || !tooltipText) {
    return nullptr;
  }

  // Create the tooltip window
  HWND hTooltip = CreateWindowExW(
      0, TOOLTIPS_CLASS, nullptr,
      WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      hWndParent, nullptr, hInst, nullptr);

  if (!hTooltip) {
    return nullptr;
  }

  // Set up the TOOLINFO structure
  TOOLINFOW ti = {0};
  ti.cbSize = sizeof(ti);
  ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
  ti.hwnd = hWndParent;
  ti.uId = reinterpret_cast<UINT_PTR>(hWndControl);
  ti.lpszText = const_cast<wchar_t*>(tooltipText);

  // Associate the tooltip with the control
  SendMessageW(hTooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
  // Tooltip must be topmost to appear above other windows
  SetWindowPos(hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  // Finally, activate it to show it.
  SendMessageW(hTooltip, TTM_ACTIVATE, TRUE, 0);

  return hTooltip;
}

void DetachConsole(HWND hWnd) {
  if (!hWnd) {
    __debugbreak();
    return;
  }
  FreeConsole();
}

void GetRightOfWindow(HWND hWnd, int* outX, int* outY) {
  // Default position if we can't get the main window rect
  const int kDefaultX = 512;
  const int kDefaultY = 512;

  if (!outX || !outY) {
    return;
  }

  if (!hWnd) {
    *outX = kDefaultX;
    *outY = kDefaultY;
    return;
  }

  RECT thisRect;
  if (GetWindowRect(hWnd, &thisRect)) {
    // Position at the right edge of the main window, aligned with its top
    *outX = thisRect.right;
    *outY = thisRect.top;
  } else {
    *outX = kDefaultX;
    *outY = kDefaultY;
  }
}
