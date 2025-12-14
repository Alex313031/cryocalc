#include "cryocalc.h"
#include <os_info_dll.h>
#include "utils.h"

// Global instance
HINSTANCE hInst;

bool debug_mode = false;
bool show_version = false;
bool show_help = false;

bool ParseCommandLine(int argc, LPWSTR argv[]) {
  bool parsed = false;
  bool is_debug_mode =
#ifdef _DEBUG
    true;
#else
    false;
#endif
  if (argv) {
    for (int i = 1; i < argc; ++i) { // start at 1 (skip .exe path)
      wchar_t* arg = argv[i];
      is_debug_mode =
          ((wcscmp(arg, L"--debug") == 0) || (wcscmp(arg, L"-d") == 0) || (wcscmp(arg, L"-debug") == 0)
           || (wcscmp(arg, L"/d") == 0) || (wcscmp(arg, L"/D") == 0));
      const bool is_version_mode =
          ((wcscmp(arg, L"--version") == 0) || (wcscmp(arg, L"-v") == 0) || (wcscmp(arg, L"-ver")) == 0
           || (wcscmp(arg, L"/v") == 0) || (wcscmp(arg, L"/V") == 0));
      const bool is_help_mode =
          ((wcscmp(arg, L"--help") == 0) || (wcscmp(arg, L"-h") == 0) || (wcscmp(arg, L"-?") == 0)
           || (wcscmp(arg, L"/h") == 0) || (wcscmp(arg, L"/H") == 0) || (wcscmp(arg, L"/?") == 0));
      if (is_version_mode && !is_help_mode) {
        show_version = true;
      }
      if (is_help_mode) {
        show_help = true;
      }
    }
    parsed = true;
  } else {
    parsed = false;
  }
  if (is_debug_mode) {
    debug_mode = true;
  }
  return parsed;
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  // Initialize common controls
  InitCommonControls();

  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  if (!ParseCommandLine(argc, argv)) {
    LocalFree(argv);
    return 1;
  } else {
    LocalFree(argv);

    if (show_version || debug_mode) {
      // Allow and allocate conhost for cmd.exe logging window
      if (!AllocConsole()) {
        return 1;
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
    if (debug_mode) {
      HandleDebugMode();
    }
  }
  const unsigned long long nt_ver = 0L;
  std::wcout << std::fixed << std::showbase << std::hex << L"GetOsInfo result = " << nt_ver << std::dec << std::defaultfloat << std::endl;
  std::wcout << L"Windows Version: " << GetWinVersionW()
             << L" " << GetOSNameW() << std::endl;

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

  return (int)msg.wParam;
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
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW); // Choose window client area background color
  wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CRYOCALC); // Attach menu to window
  wcex.lpszClassName  = szWindowClass; // Use our unique window class name
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_CRYOCALC)); // Load titlebar icon

  // Returns a "class atom", a win32 specific data type.
  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  bool success = false;
  hInst = hInstance;
  // Create the main window
  HWND hwnd = CreateWindowExW(WS_EX_WINDOWEDGE,
                              szWindowClass,
                              CAPTION_TITLE,
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              512,
                              512,
                              CW_MAINWIDTH,
                              CW_MAINHEIGHT,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);

  if (!hwnd) {
    success = false;
  } else {
    InitControls(hwnd, hInst);

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    success = true;
  }

  return success;
}

bool LaunchHelp(HWND hWnd) {
  std::wcout << L"Opened help" << std::endl;
  const int retval =
      MessageBoxW(hWnd, L"No Help implemented for CryoCalc yet...", L"Help", MB_OK | MB_ICONINFORMATION);\
  if (retval == 1) {
    return true;
  } else {
    return false;
  }
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
          OnStartButtonClick(hWnd);
          break;
        }
        case IDC_CLEAR_BUTTON: {
          const bool can_clear = ConfirmClearControls(hWnd);
          if (can_clear) {
            ClearControls(hWnd);
          }
        } break;
        case IDC_ABOUT_BUTTON:
          AboutButtonClicked(hWnd);
          break;
        case IDM_ABOUT:
          ShowAboutDialog(hWnd);
          break;
        case IDM_HELP:
          LaunchHelp(hWnd);
          break;
        case IDM_EXIT:
          // Send WM_DESTROY message to close window 
          DestroyWindow(hWnd);
          break;
        case IDM_CEXIT:
          // Confirm before exiting
          ConfirmExit(hWnd);
          break;
        case IDM_CLEAR: {
          const bool can_clear = ConfirmClearControls(hWnd);
          if (can_clear) {
            ClearControls(hWnd);
          }
        } break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    case WM_PAINT:
      SetClientRects(hWnd, paintHinst);
      break;
    case WM_SIZE: {
      SetClientRects(hWnd, paintHinst);
      if (hStatusBar) {
        SendMessageW(hStatusBar, WM_SIZE, 0, 0);
      }
    } break;
    case WM_DESTROY:
      PostQuitMessage(0);
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
