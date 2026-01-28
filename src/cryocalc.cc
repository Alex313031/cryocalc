#include "cryocalc.h"
#include "utils.h"

// Global instance
HINSTANCE hInst;
// Main Window handle
HWND hMainWindow;

// Set during WM_SIZE main window message, used to calculate rects of all other controls.
unsigned int current_width;
unsigned int current_height;

// Dummy file output for conhost
static FILE* fDummyFile;

// The main window class name
WCHAR szWindowClass[MAX_LOADSTRING];

// Window procedure function
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HMODULE hOsInfoDll = nullptr; // Module handle to osinfo.dll

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  // Initialize common controls
  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS;
  InitCommonControlsEx(&icex);

  // Check that we can load osinfo.dll and run init function.
  hOsInfoDll = LoadLibraryW(kOsInfoDll);
  if (!hOsInfoDll || hOsInfoDll == nullptr) {
    MessageBoxW(nullptr, L"osinfo.dll init failed!", L"Error loading DLL", MB_OK | MB_ICONERROR);
    return -1;
  }

  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  if (!ParseCommandLine(argc, argv)) {
    LocalFree(argv);
    return 1;
  } else {
    LocalFree(argv);

    if (enable_logging) {
      if (!AttachConsole()) {
        return 1;
      }
    }

    static const std::wstring ver = GetVersionWstring();
    if (show_help) {
      return ShowHelpAndExit();
    }
    if (show_version) {
      return ShowVersionAndExit();
    }
    HandleDebugMode(debug_mode);
  }
  LogOsInfo();

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
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return static_cast<int>(msg.wParam);
}

ATOM RegisterWndClass(HINSTANCE hInstance) {
  // Declare and set size of this window class struct.
  WNDCLASSEXW wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);

  // Set styles, icons, and window message handling function.  
  wcex.style          = CS_HREDRAW | CS_VREDRAW; // Drawing style
  wcex.lpfnWndProc    = WindowProc; // Window Procedure function
  wcex.cbClsExtra     = 0; // Extra bytes to add to end of this window class
  wcex.cbWndExtra     = 0; // Extra bytes to add to end hInstance
  wcex.hInstance      = hInstance; // This instance
  wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRYOCALC)); // Load our main app icon
  wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW); // Choose default cursor style to show
  wcex.hbrBackground  = reinterpret_cast<HBRUSH>(COLOR_WINDOW); // Choose window client area background color
  wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CRYOCALC); // Attach menu to window
  wcex.lpszClassName  = szWindowClass; // Use our unique window class name
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_CRYOCALC)); // Load titlebar icon

  // Returns a "class atom", a win32 specific data type.
  return RegisterClassExW(&wcex);
}

bool InitInstance(HINSTANCE hInstance, int nCmdShow) {
  bool success = false;
  hInst = hInstance;
  // Create the main window
  hMainWindow = CreateWindowExW(WS_EX_WINDOWEDGE,
                                szWindowClass,
                                CAPTION_TITLE,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX,
                                512,
                                512,
                                CW_MAINWIDTH,
                                CW_MAINHEIGHT,
                                nullptr,
                                nullptr,
                                hInstance,
                                nullptr);

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
  std::wcout << L"Opening chm help" << std::endl;
  HINSTANCE chm_result = ShellExecuteW(hWnd, L"open", kChmHelpFile, nullptr, nullptr, SW_NORMAL);
  std::wostringstream wostr;
  if (reinterpret_cast<INT_PTR>(chm_result) <= 32) {
    DWORD error = GetLastError();
    wostr << L"Opening Help failed! \n";
    if (error == ERROR_FILE_NOT_FOUND) {
      wostr << kChmHelpFile << L" could not be found." << std::endl;
    } else {
      wostr << L"Error = " << std::showbase << std::hex << error
            << std::dec << std::defaultfloat << std::endl;
    }
    const std::wstring warn = wostr.str();
    std::wcerr << warn;
    MessageBoxW(hWnd, warn.c_str(), L"Help Error", MB_OK | MB_ICONERROR);
    success = false;
  } else {
    success = true;
  }
  wostr.str(L"");
  wostr.clear();
  return success;
}

bool LaunchHelpEx(HWND hWnd) {
  std::wcout << L"Opening online help" << std::endl;
  return LaunchHelp(hWnd);
}

// Window procedure for handling messages
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  const HINSTANCE paintHinst = GetGlobalHinst();
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
            std::wstring msg = isChecked ? L"SSE2 checkbox was checked." : L"SSE2 checkbox was empty.";
            std::wcout << msg.c_str() << std::endl;
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
          // Confirm before exiting
          ConfirmExit(hWnd);
          break;
        case IDM_RUN:
          // Open run dialog
          OpenRunDialog(hWnd);
          break;
        case IDM_ATTACH_CON: {
          // Atach new console
          if (!AttachConsole()) {
            return 1;
          }
        } break;
        case IDM_DETACH_CON:
          // Detach console
          DetachConsole(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    // When window is shown
    case WM_CREATE:
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
        SetBkMode(winDC, TRANSPARENT);
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
      HDC hdc = reinterpret_cast<HDC>(wParam);
      HWND hThisEditControl = reinterpret_cast<HWND>(lParam);

      // Create brushes once and reuse them (static prevents GDI handle leaks)
      static HBRUSH hBrushDefault = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      static HBRUSH hBrushRed = CreateSolidBrush(RGB_RED);
      static HBRUSH hBrushGreen = CreateSolidBrush(RGB_GREEN);
      static HBRUSH hBrushCyan = CreateSolidBrush(RGB_CYAN);

      HBRUSH hBrushToUse = hBrushDefault;
      COLORREF bgColor = GetSysColor(COLOR_WINDOW);
      bool set_color = true;

      switch (GetDlgCtrlID(hThisEditControl)) {
        case IDC_LABEL_C:
        case IDC_LABEL_K:
        case IDC_LABEL_F:
        case IDC_LABEL_R:
          bgColor = RGB_RED;
          hBrushToUse = hBrushRed;
          break;
        case IDC_LABEL_INPUT:
        case IDC_LABEL_PREC:
        case IDC_LABEL_THREADS:
        case IDC_LABEL_CACHE:
          bgColor = RGB_GREEN;
          hBrushToUse = hBrushGreen;
          break;
        case IDC_INPUT:
        case IDC_THREADS:
          bgColor = RGB_CYAN;
          hBrushToUse = hBrushCyan;
          break;
        default:
          set_color = false;
          break;
      }
      if (set_color) {
        SetBkColor(hdc, bgColor);
        return reinterpret_cast<LRESULT>(hBrushToUse);
      }
    } break;
    // Handle resize events
    case WM_SIZE: {
      current_width = LOWORD(lParam);
      current_height = HIWORD(lParam);
      SetClientRects(hWnd, paintHinst);
      HandleResize(hWnd);
    } break;
    // Set/get min/max window size
    case WM_GETMINMAXINFO: {
      // Set the minimum size for the window
      LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = 390;
      pMinMaxInfo->ptMinTrackSize.y = 320;
      pMinMaxInfo->ptMaxTrackSize.x = 800;
      pMinMaxInfo->ptMaxTrackSize.y = 600;
    } break;
    case WM_QUERYENDSESSION: {
      std::wcout << L"Windows is shutting down now!" << std::endl;
      StopAllThreads();
      const uint64_t reason = static_cast<uint64_t>(lParam);
      std::wcout << L"Reason = " << reason << std::endl;
      CloseAllWindows(hWnd);
    } break;
    case WM_LBUTTONDOWN: {
    } break;
    // When close button is pressed
    case WM_CLOSE:
      CloseAllWindows(hWnd);
      FreeLibrary(hOsInfoDll);
      break;
    // Handle destroy message
    case WM_DESTROY:
      FreeLibrary(hOsInfoDll);
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HINSTANCE GetGlobalHinst() {
  if (hInst) {
    return hInst;
  } else {
    __debugbreak();
    return nullptr;
  }
}

bool AttachConsole() {
  // Allow and allocate conhost for cmd.exe logging window
  if (!AllocConsole()) {
    return false;
  }
  // File handler pointer to a dummy file, possibly an actual logfile
  FILE* fNonExistFile = fDummyFile;
#ifndef __MINGW32__
  freopen_s(&fNonExistFile, "CONOUT$", "w", stdout); // Standard error
  freopen_s(&fNonExistFile, "CONOUT$", "w", stderr); // Standard out
#else
  // freopen_s doesn't exist in MinGW...
  fNonExistFile = freopen("CONOUT$", "w", stdout); // Standard error
  fNonExistFile = freopen("CONOUT$", "w", stderr); // Standard out
#endif // __MINGW32__
  if (!fNonExistFile) {
    return false;
  } else {
    return true;
  }
}
