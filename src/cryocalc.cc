#include "cryocalc.h"

#include "ui_utils.h"

// Global instance
HINSTANCE hInst;
// Main Window handle
HWND hMainWindow;

// Set during WM_SIZE main window message, used to calculate rects of all other controls.
unsigned int current_width;
unsigned int current_height;

// The main window class name
WCHAR szWindowClass[MAX_LOADSTRING];

// Window procedure function
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HMODULE hOsInfoDll = nullptr; // Module handle to osinfo.dll

// Set to true to early attach console before InitLogging
static constexpr bool early_attach_console = true;

static constexpr bool hold_console_for_testing = false; // Don't detach console even if non logging

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  // Initialize common controls
  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC  = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS;
  InitCommonControlsEx(&icex);

  // Check that we can load osinfo.dll and run init function.
#if _WIN32_WINNT >= 0x0501
  hOsInfoDll = LoadLibraryExW(kOsInfoDll, nullptr, static_cast<DWORD>(0x00000000));
#else
  hOsInfoDll = LoadLibraryW(kOsInfoDll);
#endif
  if (!hOsInfoDll || hOsInfoDll == nullptr) {
    ErrorBox(nullptr, L"Error loading DLL", L"osinfo.dll init failed!");
    return -1;
  }

  if (early_attach_console) {
    if (!AttachConsole()) {
      return 1;
    }
  }

  int argc     = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  // Get our custom settings from .ini file (if any)
  GetCustomSettings();

  const bool parsed_cmdline = ParseCommandLine(argc, argv);
  LocalFree(argv);
  if (!parsed_cmdline) {
    return 1;
  } else {
    if (!enable_logging && !hold_console_for_testing) {
      if (!DetachConsole()) {
        return 1;
      }
    }
    if (show_help) {
      return ShowHelpAndExit();
    }
    if (show_version) {
      return ShowVersionAndExit();
    }
  }

  // Set up our logging using mini_logger library.
  const logging::LogDest kLogSink = enable_logging ? logging::LOG_TO_ALL : logging::LOG_NONE;
  const std::wstring kLogFile(kLogFileName);

  logging::LogInitSettings LoggingSettings;
  LoggingSettings.log_sink = kLogSink;
  LoggingSettings.logfile_name = kLogFile;
  LoggingSettings.app_name = kAppName;
  LoggingSettings.show_func_sigs = false;
  LoggingSettings.show_line_numbers = false;
  LoggingSettings.show_time = false;
  LoggingSettings.full_prefix_level = LOG_ERROR;
  const bool init_logging = logging::InitLogging(hInstance, LoggingSettings);
  if (init_logging) {
    logging::SetIsDCheck(is_dcheck);
    HandleDebugMode(debug_mode ? debug_mode
                               : is_dcheck); // Handle setting debug mode and welcome message
    LogOsInfo();                             // Log Windows version info
  } else {
    ErrorBox(nullptr, L"Logging Initialization Failure", L"InitLogging failed!");
    return 1;
  }

  LoadStringW(hInstance, IDC_CRYOCALC, szWindowClass, MAX_LOADSTRING);

  // Register the window class
  RegisterWndClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow)) {
    std::wcerr << L"InitInstance() failed!" << std::endl;
    return 2;
  }

  // Load keyboard accelerators.
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CRYOCALC));

  // Message loop
  MSG msg;
  // Main window message loop:
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      if (!IsDialogMessage(hMainWindow, &msg) &&
          !(hOsInfoWin && IsDialogMessage(hOsInfoWin, &msg))) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }

  return static_cast<int>(msg.wParam);
}

ATOM RegisterWndClass(HINSTANCE hInstance) {
  // Declare and set size of this window class struct.
  WNDCLASSEXW wcex;
  wcex.cbSize                  = sizeof(WNDCLASSEX);
  static const HICON main_icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRYOCALC));
  // Set styles, icons, and window message handling function.
  wcex.style       = CS_HREDRAW | CS_VREDRAW; // Drawing style
  wcex.lpfnWndProc = WindowProc;              // Window Procedure function
  wcex.cbClsExtra  = 0;                       // Extra bytes to add to end of this window class
  wcex.cbWndExtra  = 0;                       // Extra bytes to add to end hInstance
  wcex.hInstance   = hInstance;               // This instance
  wcex.hIcon       = nullptr;                 // Load our main app icon
  wcex.hCursor     = LoadCursor(nullptr, IDC_ARROW);   // Choose default cursor style to show
  wcex.hbrBackground =
      reinterpret_cast<HBRUSH>(COLOR_WINDOW);          // Choose window client area background color
  wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_CRYOCALC); // Attach menu to window
  wcex.lpszClassName = szWindowClass;                  // Use our unique window class name
  wcex.hIconSm       = main_icon;                      // Load titlebar icon

  // Returns a "class atom", a win32 specific data type.
  return RegisterClassExW(&wcex);
}

bool InitInstance(HINSTANCE hInstance, int nCmdShow) {
  bool success = false;
  hInst        = hInstance;

  static const RECT kThisDesktop = GetDesktopRect(hInstance);
  const unsigned int top_position =
      (static_cast<unsigned int>(kThisDesktop.bottom) / 2u) - (MAINHEIGHT / 2u);
  const unsigned int left_position =
      (static_cast<unsigned int>(kThisDesktop.right) / 2u) - (MAINWIDTH / 2u);
  // Create the main window
  hMainWindow = CreateWindowExW(
      WS_EX_WINDOWEDGE, szWindowClass, CAPTION_TITLE,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX, left_position,
      top_position, MAINWIDTH, MAINHEIGHT, nullptr, nullptr, hInstance, nullptr);

  if (!hMainWindow) {
    success = false;
  } else {
    // Show the window
    ShowWindow(hMainWindow, nCmdShow);
    success = UpdateWindow(hMainWindow);
  }

  return success;
}

bool LaunchHelp(HWND hWnd) {
  bool success = false;
  LOG(INFO) << L"Opening chm help";
  std::wstring help_file = GetExeDir() + kChmHelpFile;
  HINSTANCE chm_result =
      ShellExecuteW(hWnd, L"open", help_file.c_str(), nullptr, nullptr, SW_NORMAL);
  std::wostringstream wostr;
  if (reinterpret_cast<INT_PTR>(chm_result) <= 32) {
    DWORD error = GetLastError();
    wostr << L"Opening Help failed! \n";
    bool treat_as_error = true;
    if (error == ERROR_FILE_NOT_FOUND) {
      treat_as_error = false;
      wostr << kChmHelpFile << L" could not be found." << std::endl;
    } else {
      wostr << L"Error = " << std::showbase << std::hex << error << std::dec << std::defaultfloat
            << std::endl;
    }
    const std::wstring message = wostr.str();
    if (!treat_as_error) {
      LOG(WARN) << message;
    } else {
      LOG(ERROR) << message;
    }
    ErrorBox(hWnd, L"Help Error", message);
    success = false;
  } else {
    success = true;
  }
  wostr.str(L"");
  wostr.clear();
  return success;
}

bool LaunchHelpEx(HWND hWnd) {
  LOG(INFO) << L"Opening online help";
  return LaunchHelp(hWnd);
}

// Window procedure for handling messages
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDC_INPUT:
          InputEntered(hWnd);
          break;
        case IDC_CONVERT: {
          HandleConvert(hWnd);
          break;
        }
        case IDC_CONVERT_BUTTON: {
          OnConvertButtonClick(hWnd);
          break;
        }
        case IDC_ALLOC_MEM: {
          // Allocate Memory
          static const size_t kAllocBytes = 104857600; // 100MB
          AllocateMemory(kAllocBytes);
        } break;
        case IDC_CLEAR_BUTTON:
        case IDM_CLEAR: {
          const bool can_clear = ConfirmClearControls(hWnd);
          if (can_clear) {
            ClearControls(hWnd);
          }
        } break;
        case IDC_START_BUTTON: {
          BOOL isChecked = (BOOL)SendMessageW(hSSE2Checkbox, BM_GETCHECK, 0, 0);
          set_use_sse2(isChecked);
          OnStartButtonClick(hWnd);
        } break;
        case IDC_STOP_BUTTON:
          OnStopButtonClick(hWnd);
          break;
        case IDC_SSE2_CHECKBOX: {
          BOOL isChecked = (BOOL)SendMessageW(hSSE2Checkbox, BM_GETCHECK, 0, 0);
          if (HIWORD(wParam) == BN_CLICKED) {
            std::wstring msg =
                isChecked ? L"SSE2 checkbox was checked." : L"SSE2 checkbox was empty.";
            LOG(INFO) << msg.c_str();
          }
          set_use_sse2(isChecked);
        } break;
        case IDC_ABOUT_BUTTON:
          AboutButtonClicked(hWnd);
          break;
        case IDM_ABOUT:
          ShowAboutDialog(hWnd);
          break;
        case IDM_OSINFO:
        case IDC_OSINFO_BUTTON:
          ShowOsInfo(hWnd);
          break;
        case IDM_PASTE:
          HandlePaste(hWnd);
          break;
        case IDM_HELPEX:
          LaunchHelpEx(hWnd);
          break;
        case IDM_HELP:
          LaunchHelp(hWnd);
          break;
        case IDM_EXIT:
          CloseAllWindows(hWnd);
          break;
        case IDM_CEXIT:
          ConfirmExit(hWnd); // Confirm before exiting
          break;
        case IDM_RUN:
          OpenRunDialog(hWnd); // Open run dialog
          break;
        case IDM_ATTACH_CON: {
          // Atach new console
          if (!AttachConsole()) {
            ErrorBox(hWnd, L"Console Error", L"Failed to attach console");
          }
        } break;
        case IDM_DETACH_CON: {
          // Detach console
          if (!DetachConsole()) {
            ErrorBox(hWnd, L"Console Error", L"Failed to detach console");
          }
        } break;
        case IDM_CLEAR_CON: {
          ClearConsole(hWnd);
          FLOG(WARN) << L"Cleared console";
        } break;
        case IDM_CLEAR_LOG: {
          bool clear_log = ConfirmAndClearLog(hWnd);
          if (clear_log) {
            CLOG(WARN) << L"Cleared contents of log file.";
          }
        } break;
        case IDM_TEST_LOG: {
          logging::TestLogging();
        } break;
        case IDM_OPEN_LOG: {
          const std::wstring cryocalc_logfile_path = GetExeDir() + kLogFileName;
          OpenLogFile(hWnd, cryocalc_logfile_path);
        } break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    // When window is shown
    case WM_CREATE:
      SetClientRects(hWnd, hInst);
      InitControls(hWnd, hInst);
      break;
    // Start painting
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // TODO: Add any drawing code that uses hdc here...
      if (hdc) {
        const HDC winDC = GetWindowDC(hWnd);
        // Set color of window and text
        SetBkColor(winDC, COLOR_WINDOW);
        SetTextColor(winDC, COLOR_WINDOWTEXT);
        // Set window background painting behavior
        SetBkMode(winDC, OPAQUE);
        ReleaseDC(hWnd, winDC);
      }
      EndPaint(hWnd, &ps);
    } break;
    // Handle F1 key
    case WM_HELP:
      LaunchHelp(hWnd);
      break;
    // Set Paint colors of child windows
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
      HDC hdc               = reinterpret_cast<HDC>(wParam);
      HWND hThisEditControl = reinterpret_cast<HWND>(lParam);
      // Create brushes once and reuse them (static prevents GDI handle leaks)
      static const COLORREF bgColorDefault = GetSysColor(COLOR_WINDOW);
      static const HBRUSH hBrushDefault    = CreateSolidBrush(bgColorDefault);
      static HBRUSH hBrushRed              = CreateSolidBrush(RGB_REDISH);
      static HBRUSH hBrushGreen            = CreateSolidBrush(RGB_GREENISH);
      static HBRUSH hBrushCyan             = CreateSolidBrush(RGB_CYAN);
      // Initially set to default color
      COLORREF bgColor   = bgColorDefault;
      HBRUSH hBrushToUse = hBrushDefault;
      bool set_color     = false;
      switch (GetDlgCtrlID(hThisEditControl)) {
        case IDC_LABEL_C:
        case IDC_LABEL_K:
        case IDC_LABEL_F:
        case IDC_LABEL_R:
          bgColor     = RGB_REDISH;
          hBrushToUse = hBrushRed;
          set_color   = true;
          break;
        case IDC_LABEL_INPUT:
        case IDC_LABEL_PREC:
        case IDC_LABEL_THREADS:
        case IDC_LABEL_CACHE:
          bgColor     = RGB_GREENISH;
          hBrushToUse = hBrushGreen;
          set_color   = true;
          break;
        case IDC_INPUT:
        case IDC_THREADS:
          bgColor     = RGB_CYAN;
          hBrushToUse = hBrushCyan;
          set_color   = true;
          break;
        default:
          break;
      }
      if (set_color) {
        SetBkColor(hdc, bgColor);
        return reinterpret_cast<LRESULT>(hBrushToUse);
      }
    } break;
    // Handle resize events
    case WM_SIZE: {
      current_width  = LOWORD(lParam);
      current_height = HIWORD(lParam);
      SetClientRects(hWnd, hInst);
      HandleResize(hWnd);
    } break;
    case WM_GETMINMAXINFO: {
      // Set the minimum size for the window
      LPMINMAXINFO pMinMaxInfo      = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = MINWIDTH;
      pMinMaxInfo->ptMinTrackSize.y = MINHEIGHT;
      pMinMaxInfo->ptMaxTrackSize.x = MAXWIDTH;
      pMinMaxInfo->ptMaxTrackSize.y = MAXHEIGHT;
    } break;
    case WM_QUERYENDSESSION: {
      LOG(WARN) << L"Windows is shutting down now!";
      StopAllThreads();
      const uint64_t reason = static_cast<uint64_t>(lParam);
      LOG(WARN) << L"Reason = " << reason;
      CloseAllWindows(hWnd);
    } break;
    case WM_LBUTTONDOWN: {
    } break;
    // When close button is pressed
    case WM_CLOSE:
      CloseAllWindows(hWnd);
      break;
    // Handle destroy message
    case WM_DESTROY:
      FreeLibrary(hOsInfoDll); // Release osinfo.dll
      PostQuitMessage(0);
      break;
    // "After destroy" message.
    case WM_NCDESTROY:
      hMainWindow = nullptr; // Should already be nullptr, but make sure it is.
      break;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}

HINSTANCE GetGlobalHinst() {
  if (hInst) {
    return hInst;
  } else {
    __debugbreak();
    return nullptr;
  }
}
