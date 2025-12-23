#include "osinfo.h"

#include <os_info_dll.h>

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
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
  wcex.lpszMenuName   = nullptr;
  wcex.lpszClassName  = szOSInfoWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  RegisterClassExW(&wcex);
  
  HWND hwnd = CreateWindowExW(WS_EX_WINDOWEDGE,
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
    case WM_CREATE: {
      OutputOsInfo(hWnd);
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
      LPMINMAXINFO pMinMaxInfo = (LPMINMAXINFO)lParam;
      pMinMaxInfo->ptMinTrackSize.x = 250;
      pMinMaxInfo->ptMinTrackSize.y = 150;
      pMinMaxInfo->ptMaxTrackSize.x = 350;
      pMinMaxInfo->ptMaxTrackSize.y = 250;
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
  static std::wstring textout = GetWinInfo();
  AppendTextToEditControl(hOsInfoTextOut, textout);
  std::wstring kNTVer = L"";
  const unsigned long short_nt_ver = GetShortNTVer();
  std::wostringstream wostr;
  wostr << L"Raw NTVER: "
        << std::fixed << std::setprecision(8) << std::showbase << std::hex
        << short_nt_ver << std::dec << std::endl;
  kNTVer = wostr.str();
  std::wcout << kNTVer.c_str();
  if (hStatusBar) {
    SendMessageW(hOsInfoStatusBar, SB_SETTEXT, 0, (LPARAM)kNTVer.c_str());
  }
}

void LogOsInfo() {
  std::wcout << L"osinfo.dll ver. " << GetOsInfoDllVersionW() << std::endl;
  std::wcout << L"Windows Version: " << GetWinVersionW()
             << L" " << GetOSNameW() << std::endl;
  const unsigned long long nt_ver = GetRawNTVer();
  std::wcout << std::fixed << std::showbase << std::hex << L"GetRawNTVer result = "
             << nt_ver << std::dec << std::defaultfloat << std::endl;
}
