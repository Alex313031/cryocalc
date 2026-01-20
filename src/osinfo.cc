#include "osinfo.h"

static int this_width;
static int this_height;

const WCHAR* szOSInfoWindowClass = L"CryoCalcOsInfoClass";

HWND hOsInfoStatusBar;
HWND hOsInfoTextOut;

bool ShowOsInfo(HWND hWnd) {
  bool success = false;
  const HINSTANCE this_hinst = GetGlobalHinst();

  WNDCLASSEXW wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = OsInfoWndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = this_hinst;
  wcex.hIcon          = LoadIcon(this_hinst, MAKEINTRESOURCE(IDI_SMALL));
  wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground  = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
  wcex.lpszMenuName   = nullptr;
  wcex.lpszClassName  = szOSInfoWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  RegisterClassExW(&wcex);
  
  HWND hwnd = CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW,
                              szOSInfoWindowClass,
                              L"OS Info",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX,
                              512,
                              512,
                              300,
                              200,
                              nullptr,
                              nullptr,
                              this_hinst,
                              nullptr);
  // Create text edit control
  hOsInfoTextOut = CreateWindowExW(0, WC_EDIT, nullptr,
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
      6,
      6,
      300,
      200,
      hwnd, nullptr, this_hinst, nullptr);
  hOsInfoStatusBar = CreateWindowExW(
      0, STATUSCLASSNAME, nullptr,
      WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
      0, 0, 0, 0,
      hwnd, nullptr, this_hinst, nullptr
  );

  if (!hwnd) {
    success = false;
  } else {
    OutputOsInfo(hWnd);
    ShowWindow(hwnd, SW_NORMAL);
    success = UpdateWindow(hwnd);
  }

  return success;
}

LRESULT CALLBACK OsInfoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  const HINSTANCE this_hinst = GetGlobalHinst();
  switch (uMsg) {
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    case WM_PAINT: {
    } break;
    case WM_SIZE: {
      this_width = LOWORD(lParam);
      this_height = HIWORD(lParam);
      SendMessageW(hOsInfoStatusBar, WM_SIZE, 0, 0);
      RECT hStatusRect;
      int status_height = 0;
      if (GetWindowRect(hOsInfoStatusBar, &hStatusRect)) {
        status_height = hStatusRect.bottom - hStatusRect.top;
      }
      MoveWindow(hOsInfoTextOut, PADDING_X, PADDING_Y, this_width - END_PADDING, this_height - status_height - END_PADDING, TRUE);
      
    } break;
    case WM_GETMINMAXINFO: {
      LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = 250;
      pMinMaxInfo->ptMinTrackSize.y = 150;
    } break;
    case WM_CLOSE:
    case WM_DESTROY:
      CloseWindow(hWnd);
      DestroyWindow(hWnd);
      UnregisterClassW(szOSInfoWindowClass, this_hinst);
      break;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}

std::wstring GetWinInfo() {
  std::wostringstream wostr;
  wostr << L"(Windows NT " << GetWinVersionW() << L")\r\n"
        << L"OS Name: " << GetOSNameW() << L"\r\n";
  const std::wstring retval = wostr.str();
  return retval;
}

void OutputOsInfo(HWND hWnd) {
  std::wstring textout = GetWinInfo();
  AppendTextToEditControl(hOsInfoTextOut, textout);
  std::wstring kNTVer = L"";
  const unsigned long short_nt_ver = GetShortNTVer();
  std::wostringstream wostr;
  wostr << L"Raw NTVER: "
        << std::fixed << std::setprecision(8) << std::showbase << std::hex
        << short_nt_ver << std::dec << std::endl;
  kNTVer = wostr.str();
  wostr.str(L"");
  wostr.clear();
  std::wcout << kNTVer.c_str();
  SendMessageW(hOsInfoTextOut, EM_SETREADONLY, TRUE, 0);
  if (hStatusBar) {
    SendMessageW(hOsInfoStatusBar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(kNTVer.c_str()));
  }
}

void LogOsInfo() {
  GetOsInfoDllVersionW_t pGetOsInfoDllVersionW =
      reinterpret_cast<GetOsInfoDllVersionW_t>(GetProcAddress(hOsInfoDll, "GetOsInfoDllVersionW"));
  std::wstring OsInfoDllVer;
  if (pGetOsInfoDllVersionW == nullptr) {
    DWORD error = GetLastError();
    std::wcerr << L"Failed to get function address. Error: " << error << std::endl;
  } else {
    OsInfoDllVer = pGetOsInfoDllVersionW();
  }
  std::wcout << L"osinfo.dll ver. " << OsInfoDllVer << std::endl;
  std::wcout << L"Windows Version: " << GetWinVersionW()
             << L" " << GetOSNameW() << std::endl;
  const unsigned long long nt_ver = GetRawNTVer();
  if (debug_mode) {
    std::wcout << std::fixed << std::showbase << std::hex << L"GetRawNTVer result = "
               << nt_ver << std::dec << std::defaultfloat << std::endl;
    TestDllGetVersion();
  }
}

bool TestDllGetVersion() {
  bool success = false;
  DLLGETVERSIONPROC fnDllGetVersion;
  DLLVERSIONINFO dvi = {sizeof(dvi)};
  DLLVERSIONINFO2 dvi2 = {sizeof(dvi2)};
  DWORD dwVersion = 0;
  DWORD dwVersion2 = 0;
  DWORD error;
  if (!hOsInfoDll || hOsInfoDll == nullptr) {
    fnDllGetVersion = nullptr;
    std::wcerr << L"hOsInfoDll was null!" << std::endl;
    return false;
  } else {
    fnDllGetVersion = reinterpret_cast<DLLGETVERSIONPROC>(GetProcAddress(hOsInfoDll, "DllGetVersion"));
  }
  if (fnDllGetVersion == nullptr) {
    error = GetLastError();
    std::wcerr << L"Failed to get DllGetVersion address. Error: " << error << std::endl; 
  } else {
    HRESULT hr = fnDllGetVersion(&dvi);
    HRESULT hr2 = fnDllGetVersion(reinterpret_cast<DLLVERSIONINFO*>(&dvi2));
    if (hr == S_OK && hr2 == S_OK) {
      dwVersion = _PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
      dwVersion2 = _PACKVERSION(dvi2.info1.dwMajorVersion, dvi2.info1.dwMinorVersion);
      success = true;
    } else {
      error = GetLastError();
    }
  }
  if (success) {
    std::wcout << L"DLLVERSIONINFO dwVersion = " << std::showbase << std::hex
               << dwVersion << std::dec << std::defaultfloat << std::endl;
    std::wcout << L"DLLVERSIONINFO dwMajorVersion.dwMinorVersion.dwBuildNumber = " << dvi.dwMajorVersion << L"." << dvi.dwMinorVersion << L"." << dvi.dwBuildNumber << std::endl;
    std::wcout << L"DLLVERSIONINFO2 dwVersion = " << std::showbase << std::hex
               << dwVersion2 << std::dec << std::defaultfloat << std::endl;
    std::wcout << L"DLLVERSIONINFO2 dwMajorVersion.dwMinorVersion.dwBuildNumber = " << dvi2.info1.dwMajorVersion << L"." << dvi2.info1.dwMinorVersion << L"." << dvi2.info1.dwBuildNumber << std::endl;
    std::wcout << L"DLLVERSIONINFO2 ullVersion = " << dvi2.ullVersion << std::endl;
  } else {
    std::wcerr << __FUNC__ << L"() Failed! Error: " << error << std::endl; 
  }
  return success;
}
