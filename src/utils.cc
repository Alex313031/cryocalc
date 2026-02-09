#include "utils.h"

#include "converters.h"
#include "resource.h"
#include "strings.h"

unsigned int g_precision_;

static GET_NATIVE_SYSTEM_INFO_ pfnGetNativeSystemInfo = nullptr;
static RUN_FILE_DLG_ pfnRunFileDlg = nullptr;

// Declare custom_settings here, to be set later, for all of cryocalc to use
CustomSettings custom_settings;
// To be set if we read .ini file correctly
unsigned int custom_precision;
bool custom_debug_mode;

// Bools to toggle as we go down the line to get our settings
bool got_ini = false;
bool got_settings = false;
bool set_settings = false;

// Handle to our .ini file
HANDLE g_ini_file;
// Declare rects to use for all future window layout
RECT kMainClientRect;
RECT kMainWinRect;

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
  LOG(DEBUG) << "testCelsius: \n" << testCelsius;

  std::wstring testFahrenheit = fromFahrenheit(kDummyFahrenheit);
  LOG(DEBUG) << "testFahrenheit: \n" << testFahrenheit;

  std::wstring testKelvin = fromKelvin(kDummyKelvin);
  LOG(DEBUG) << "testKelvin: \n" << testKelvin;

  std::wstring testRankine = fromRankine(kDummyRankine);
  LOG(DEBUG) << "testRankine: \n" << testRankine;

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

void CloseAllWindows(HWND hWnd) {
  // Tell OsInfo Window to close, if it's open.
  if (hOsInfoWin != nullptr) {
    PostMessageW(hOsInfoWin, WM_COMMAND, IDC_CLOSE_OSINFO, 0);
  }
  logging::DeInitLogging(GetGlobalHinst());
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

bool ConfirmAndClearLog(HWND hWnd) {
  int user_response_code =
      MessageBoxW(nullptr, L"Clear all log file contents?", L"Confirm Clear LogFile",
                  MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON2);
  switch (user_response_code) {
    case IDNO:
    case IDCANCEL:
      return false;
    case IDYES:
      return ClearLogFile(hWnd);
    default:
      return false;
  }
}

const int ShowVersionAndExit() {
  static const std::wstring kVersion = GetVersionWstring();
  std::wcout << L"\n " << kAppName << " ver. "
             << kVersion << L"\n " << std::endl;
  system("pause");
  return 0;
}

const int ShowHelpAndExit() {
  std::wcout << L"\n " << GetExecutableName()
             << L" Usage: \n" << std::flush;
  std::wostringstream wostr;
  wostr << L"   /d | -d | --debug   : Enable debug mode and enable logging\n"
        << L"   /l | -l | --logging : Enable logging in console Window \n"
        << L"   /v | -v | --version : Show version info \n"
        << L"   /? | -h | --help    : Show this Help \n" << std::flush;
  static const std::wstring kHelpMsg = wostr.str();
  std::wcout << kHelpMsg.c_str() << std::endl;
  system("pause");
  return 0;
}

void HandleDebugMode(const bool debug_mode) {
  std::wostringstream wostr;
  wostr << L" ------ " << "Welcome to " << kAppName << " ver. " << GetVersionWstring();
  if (debug_mode) {
    wostr << L" (Debug Mode)";
  }
  wostr << L" ------ " << std::endl;
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
    LOG(WARN) << L"Threads input was 0!";
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
    LOG(DEBUG) << L"Using " << whichfunc << " for " << __FUNC__;
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
    LOG(ERROR) << L"Percentage " << percent
               << L" is too large or zero!";
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
  bool parsed;
  bool is_debug_mode = GetDefaultWantDebug();
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
      pfnRunFileDlg = reinterpret_cast<RUN_FILE_DLG_>(GetProcAddress(hShell32Dll, (LPCSTR)(61)));
      if (pfnRunFileDlg) {
        LOG(INFO) << L"Opening RunFileDlg";
        pfnRunFileDlg(hWnd, kSmallIcon, (LPWSTR)szCurDir, RUN_TITLE, RUN_PROMPT, RFD_USEFULLPATHDIR | RFD_WOW_APP);
      } else {
        LOG(ERROR) << L"Failed to open run dialog.";
      }
    } else {
      LOG(ERROR) << L"Failed to get shell32.dll handle.";
    }
    DestroyIcon(kSmallIcon); // Cleanup icon
  }
}

const std::wstring GetExeDir() {
  wchar_t exe_path[MAX_PATH];
  HMODULE this_app = GetModuleHandleW(nullptr);
  if (!this_app) {
    return std::wstring();
  }
  DWORD got_path = GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
  if (got_path == 0 || got_path >= MAX_PATH) {
    return std::wstring();
  }

  // Find the last backslash to get the directory
  std::wstring fullPath(exe_path);
  size_t lastSlash = fullPath.find_last_of(L"\\/");
  std::wstring retval;
  if (lastSlash != std::wstring::npos) {
    retval = fullPath.substr(0, lastSlash + 1);  // Include trailing slash
  } else {
    retval = fullPath;
  }
  if (debug_mode) {
    LOG(DEBUG) << __func__ << L" = " << retval;
  }
  return retval;
}

// Run any shell app
bool RunShellApplet(HWND hWnd, const wchar_t* executable) {
  bool success = false;
  LOG(INFO) << L"Running " << executable;
  HINSTANCE result = ShellExecuteW(hWnd, L"open", executable, nullptr, nullptr, SW_NORMAL);
  std::wostringstream wostr;
  if (reinterpret_cast<INT_PTR>(result) <= 32) {
    DWORD error = GetLastError();
    wostr << L"Opening " << executable << " failed! \n";
    bool treat_as_error = true;
    if (error == ERROR_FILE_NOT_FOUND) {
      wostr << executable << L" could not be found.";
      treat_as_error = false;
    } else {
      wostr << L"Error = " << std::showbase << std::hex << error
            << std::dec << std::defaultfloat;
    }
    const std::wstring message = wostr.str();
    if (!treat_as_error) {
      LOG(WARN) << message;
    } else {
      LOG(ERROR) << message;
    }
    MessageBoxW(hWnd, message.c_str(), treat_as_error ? L"Error" : L"Warning", MB_OK | MB_ICONSTOP);
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
    LOG(ERROR) << L"Default cache size was used!";
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
               SWP_NOMOVE | SWP_NOSIZE);
  // Finally, activate it to show it.
  //SendMessageW(hTooltip, TTM_ACTIVATE, TRUE, 0);

  return hTooltip;
}

bool AttachConsole() {
  return logging::AttachConsoleImpl();
}

bool DetachConsole() {
  return logging::DetachConsoleImpl();
}

void ClearConsole(HWND hWnd) {
  if (!hWnd || !logging::GetIsConsoleAttached()) {
    return;
  }

  // Open CONOUT$ directly since GetStdHandle may return the original
  // (invalid) handle after freopen redirected the C runtime streams.
  HANDLE hConsole = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                0, nullptr);
  if (hConsole == INVALID_HANDLE_VALUE) {
    MessageBoxW(nullptr, L"Failed to open CONOUT$", L"Clear Console Error", MB_OK | MB_ICONERROR);
    return;
  }

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    unsigned int err = GetLastError();
    std::wstring msg = L"Failed to get screen buffer info. \nError: " + std::to_wstring(err);
    MessageBoxW(nullptr, msg.c_str(), L"Clear Console Error", MB_OK | MB_ICONERROR);
    CloseHandle(hConsole);
    return;
  }

  // Calculate visible window size
  SHORT windowWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  SHORT windowHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  // Shrink buffer to window size - this clears scrollback history
  COORD newSize = {windowWidth, windowHeight};
  SetConsoleScreenBufferSize(hConsole, newSize);

  // Clear the visible area
  DWORD visibleSize = windowWidth * windowHeight;
  COORD topLeft = {0, 0};
  DWORD charsWritten;
  FillConsoleOutputCharacterW(hConsole, L' ', visibleSize, topLeft, &charsWritten);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, visibleSize, topLeft, &charsWritten);

  // Move cursor to top-left
  SetConsoleCursorPosition(hConsole, topLeft);

  CloseHandle(hConsole);
}

bool ClearLogFile(HWND hWnd) {
  if (!hWnd || !logging::ClearFileContents()) {
    MessageBoxW(nullptr, L"Failed to clear log file!", L"Logging Error", MB_OK | MB_ICONERROR);
    return false;
  } else {
    return true;
  }
}

bool SetClientRects(HWND hWnd, HINSTANCE hInst) {
  RECT winRect;
  RECT clientRect;
  // Get rect size of window including titlebar, for setting other Window's positions relative to this window
  if (!GetWindowRect(hMainWindow, &winRect)) {
    return false;
  }
  // Get internal rects inside Window, excluding titlebar, for setting control positions inside Window
  if (!GetClientRect(hWnd, &clientRect)) {
    return false;
  }
  kMainClientRect = winRect;
  kMainClientRect = clientRect;
  return true;
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
    LOG(DEBUG) << L"thisRect.right = " << thisRect.right << L" \n"
               << L"thisRect.top = " << thisRect.top << L" ";
    LOG(DEBUG) << L"kMainWinRect.right = " << kMainWinRect.right << L" \n"
               << L"kMainWinRect.top = " << kMainWinRect.top << L" ";
  } else {
    *outX = kDefaultX;
    *outY = kDefaultY;
  }
}

bool GetCustomSettings() {
  // Open cryocalc.ini
  const std::wstring cryocalc_ini = GetExeDir() + kIniFileName;
  if (!OpenIniFileForReading(cryocalc_ini)) {
    return false;
  }

  if (!got_ini) {
    return false;
  }

  // Read file contents
  DWORD file_size = GetFileSize(g_ini_file, nullptr);
  if (file_size == INVALID_FILE_SIZE || file_size == 0) {
    CloseHandle(g_ini_file);
    g_ini_file = INVALID_HANDLE_VALUE;
    return false;
  }

  // Read the file into a buffer
  std::string buffer(file_size, '\0');
  DWORD bytes_read = 0;
  if (!ReadFile(g_ini_file, &buffer[0], file_size, &bytes_read, nullptr)) {
    std::wstring msg = L"Failed to read " + logging::ToWide(kIniFileName);
    OutputDebugStringW(msg.c_str());
    CloseHandle(g_ini_file);
    g_ini_file = INVALID_HANDLE_VALUE;
    return false;
  }

  // Close file handle now that we're through with it
  CloseHandle(g_ini_file);
  g_ini_file = INVALID_HANDLE_VALUE;

  // Parse the file line by line
  // Format: key=value (e.g., default_precision=4, debug_mode=1)
  std::istringstream stream(buffer);
  std::string line;
  while (std::getline(stream, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#' || line[0] == ';') {
      continue;
    }

    // Find the '=' delimiter
    size_t eq_pos = line.find('=');
    if (eq_pos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, eq_pos);
    std::string value = line.substr(eq_pos + 1);

    // Trim whitespace from key and value
    while (!key.empty() && (key.back() == ' ' || key.back() == '\r')) {
      key.pop_back();
    }
    while (!value.empty() && (value.back() == ' ' || value.back() == '\r')) {
      value.pop_back();
    }

    // Parse known settings
    bool precisionkey = (key == "default_precision");
    bool debugkey = (key == "debug_mode");
    if (precisionkey) {
      const int precision = std::atoi(value.c_str());
      if (precision >= static_cast<int>(MIN_PRECISION) &&
          precision <= static_cast<int>(MAX_PRECISION)) {
        const int prec_val = static_cast<unsigned int>(precision);
        custom_settings.default_precision = prec_val;
      } else {
        std::wcerr << L"INI: default_precision=" << precision
                   << L" out of range (" << MIN_PRECISION << L"-" << MAX_PRECISION << L")!" << std::endl;
        custom_settings.default_precision = DEFAULT_PRECISION;
      }
    } else {
      custom_settings.default_precision = DEFAULT_PRECISION;
    }
    if (debugkey) {
      const int debug_val = std::atoi(value.c_str());
      if (debug_val == 0) {
        custom_settings.set_debug_mode = false;
      } else if (debug_val == 1) {
        custom_settings.set_debug_mode = true;
      } else {
        custom_settings.set_debug_mode = false;
      }
    } else {
      custom_settings.set_debug_mode = false;
    }
  }

  got_settings = true;
  return SetCustomSettings();
}

bool SetCustomSettings() {
  if (!got_settings) {
    return false;
  }
  unsigned int want_prec = custom_settings.default_precision;
  bool want_debug = custom_settings.set_debug_mode;
  std::wcout << L"Got custom settings: " << L"Prec=" << want_prec
             << L" Debug=" << static_cast<int>(want_debug) << std::endl;
  custom_precision = want_prec;
  custom_debug_mode = want_debug;
  set_settings = true;
  return true;
}

bool OpenIniFileForReading(const std::wstring ini_file) {
  if (ini_file.length() >= MAX_PATH) {
    return false;
  }

  // Try to open the .ini file in read only mode.
  g_ini_file = CreateFileW(
      ini_file.c_str(),
      GENERIC_READ, // Only allow reading
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,        // Default security
      OPEN_EXISTING, // Only open if .ini file exists
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (g_ini_file == INVALID_HANDLE_VALUE) {
    got_ini = false;
    DWORD err = GetLastError();
    if (err == ERROR_FILE_NOT_FOUND) {
      LOG(WARN) << kIniFileName << " not found!";
    } else {
      LOG(ERROR) << __FUNC__ << L" Failed. Error = " << err;
    }
    return false;
  }

  got_ini = true;
  LOG(DEBUG) << L"Successfully opened " << ini_file;
  return true;
}

// Uses custom precision from .ini file, otherwise DEFAULT_PRECISION
const unsigned int GetDefaultPrecision() {
  if (!set_settings) {
    return DEFAULT_PRECISION;
  } else {
    return custom_precision;
  }
}

// Returns true if debug_mode=1 in .ini file
const bool GetDefaultWantDebug() {
  if (!set_settings) {
    return false;
  } else {
#if defined(_DEBUG) || defined(DEBUG)
    return true;
#else
    return custom_debug_mode;
#endif
  }
}

errno_t AllocateMemory(const size_t num_bytes) {
  LPVOID pMemory = VirtualAlloc(nullptr, num_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (pMemory == NULL) {
    MessageBoxW(nullptr, L"Failed to allocate memory", L"VirtualAlloc Error", MB_OK | MB_ICONERROR);
    return 12; // ENOMEM
  } else {
    LOG(DEBUG) << static_cast<int>(num_bytes / 1048576u) << " Megabytes memory allocated at address: " << std::showbase << std::hex << reinterpret_cast<unsigned long long>(pMemory) << std::dec << std::noshowbase;
    return 0;
  }
}
