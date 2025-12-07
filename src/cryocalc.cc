#include "cryocalc.h"
#include "utils.h"

// Global instance
HINSTANCE hInst;

// Edit instances
HWND hCelsiusEdit;
HWND hKelvinEdit;
HWND hFahrenheitEdit;
HWND hRankineEdit;

// Static labels
static HWND hCelsiusLabel;  
static HWND hKelvinLabel;
static HWND hFahrenheitLabel;
static HWND hRankineLabel;
// Convert button handle
static HWND hConvButton;

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  // Initialize common controls
  InitCommonControls();

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

  LoadStringW(hInstance, IDC_CRYOCALC, szWindowClass, MAX_LOADSTRING);

  // Register the window class
  RegisterWndClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow)) {
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
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1); // Choose window client area background color
  wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CRYOCALC); // Attach menu to window
  wcex.lpszClassName  = szWindowClass; // Use our unique window class name
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)); // Load titlebar icon

  // Returns a "class atom", a win32 specific data type.
  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;
  // Create the main window
  HWND hwnd = CreateWindowExW(WS_EX_WINDOWEDGE,
                              szWindowClass,
                              CAPTION_TITLE,
                              WS_OVERLAPPEDWINDOW,
                              512,
                              512,
                              CW_MAINWIDTH,
                              CW_MAINHEIGHT,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);

  if (!hwnd) {
    return false;
  } else {
    // Label (Static Control)
    hCelsiusLabel = CreateWindowEx(
        0,                      // Optional window styles
        L"STATIC",              // Predefined class: Static
        L"Celsius:",            // Text
        WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles
        15,                     // x position
        25,                     // y position
        50,                     // Width
        20,                     // Height
        hwnd,                   // Parent window
        (HMENU)IDC_LABEL_C,     // No menu, but control ID
        hInst,                  // This window instance
        nullptr                 // Additional app data
    );
    // Create the Edit controls
    hCelsiusEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE, L"EDIT", L"77",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        85,
        25,
        75,
        20,
        hwnd, (HMENU)IDC_CELSIUS, hInst, nullptr
    );
    hKelvinLabel = CreateWindowEx(
        0,
        L"STATIC",
        L"Kelvin:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        15,
        50,
        50,
        20,
        hwnd,
        (HMENU)IDC_LABEL_K,
        hInst,
        nullptr
    );
    hKelvinEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        85,
        50,
        75,
        20,
        hwnd, (HMENU)IDC_KELVIN, hInst, nullptr
    );
    hFahrenheitLabel = CreateWindowEx(
        0,
        L"STATIC",
        L"Fahren:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        15,
        75,
        50,
        20,
        hwnd,
        (HMENU)IDC_LABEL_F,
        hInst,
        nullptr
    );
    hFahrenheitEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        85,
        75,
        75,
        20,
        hwnd, (HMENU)IDC_FAHRENHEIT, hInst, nullptr
    );

    // Create the Start Button control
    hConvButton = CreateWindow(
        L"BUTTON", CONV_BUTTON,
        WS_CHILD | WS_VISIBLE,
        62,
        300,
        75,
        30,
        hwnd, (HMENU)IDC_BUTTON1, hInst, nullptr
    );

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return true;
  }
}

// Window procedure for handling messages
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDC_CELSIUS:
          break;
        case IDC_KELVIN:
          break;
        case IDC_BUTTON1:
          OnStartButtonClick(hWnd);
          break;
        case IDM_ABOUT:
          // Show "About" dialog box
          DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
          break;
        case IDM_EXIT:
          // Send WM_DESTROY message to close window 
          DestroyWindow(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

bool OnStartButtonClick(HWND hWnd) {
  bool success = false;
  // Get the length of the text in the edit control
  DWORD dwSize = GetWindowTextLength(hCelsiusEdit);
  std::wstring out;
  if (dwSize > 0) {
    int celsius = 0;
    // Create a buffer to store the text
    wchar_t* buffer = new wchar_t[dwSize + 1]; // Allocate memory for the text

    // Get the text from the edit control
    GetWindowTextW(hCelsiusEdit, buffer, dwSize + 1);
    std::wcout << L"hCelsiusEdit content = " << buffer << std::endl;
    celsius = std::stoi(buffer);
    std::wcout << L"celsius = " << celsius << std::endl;
    long double testkelvin = kelvin::fromCelsius(celsius);
    std::wcout << L"fromCelsius = " << testkelvin << std::endl;
    out = std::to_wstring(testkelvin);
    success = true;
  } else {
    MessageBoxW(hWnd, L"No text entered!", L"Error", MB_OK | MB_ICONERROR);
    success = false;
  }
  if (success) {
    std::wcout << L"out = " << out << std::endl;
    SetWindowTextW(hKelvinEdit, out.c_str());
  }
  return success;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);

  bool AboutHandled = false; // Stores status of whether dialog has been handled user-wise.
  switch (message) {
    case WM_INITDIALOG:
      // Showed the dialog
      AboutHandled = true;
      break;
    case WM_COMMAND:
      // Exit the dialog
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        if (EndDialog(hDlg, LOWORD(wParam))) {
          AboutHandled = true;
          return (INT_PTR)AboutHandled;
        } else {
          AboutHandled = false;
          break;
        }
      } break;
    default:
      AboutHandled = false;
      break;
  }

  // About dialog failed
  return (INT_PTR)AboutHandled;
}
