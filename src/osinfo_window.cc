#include "osinfo_window.h"

#include "painting.h"

static int this_width;
static int this_height;

const WCHAR* szOSInfoWindowClass = CRYOCALC_OSINFO_WNDCLASS;

HWND hOsInfoWin;

HWND hOsInfoTextOut;
HWND hOsInfoStatusBar;
HWND hWinVerButton;
HWND hMsInfoButton;
HWND hRunAppButton;
HWND hCloseOSInfoButton;

static const unsigned int kOsInfoButtonWidth = (BUTTON_WIDTH * 2u);

bool ShowOsInfo(HWND hWnd) {
  bool success               = false;
  const HINSTANCE this_hinst = GetGlobalHinst();

  // Check if Window already exists: Don't open two OS Info Windows
  if (hOsInfoWin != nullptr) {
    LOG(INFO) << L"Restoring OS Info Window to foreground";
    return SetForegroundWindow(hOsInfoWin);
  } else {
    LOG(INFO) << L"Showed OS Info Window";
  }

  WNDCLASSEXW wcex;
  wcex.cbSize        = sizeof(WNDCLASSEX);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = OsInfoWndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = this_hinst;
  wcex.hIcon         = LoadIcon(this_hinst, MAKEINTRESOURCE(IDI_SMALL));
  wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
  wcex.lpszMenuName  = nullptr;
  wcex.lpszClassName = szOSInfoWindowClass;
  wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  RegisterClassExW(&wcex);

  // Position OS Info window to the right of the main window
  int osInfoX, osInfoY;
  GetRightOfWindow(hMainWindow, &osInfoX, &osInfoY);
  osInfoX = osInfoX + 2; // 2 pixels padding between windows
  osInfoY = osInfoY + 2;

  hOsInfoWin = CreateWindowExW(
      WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW, szOSInfoWindowClass, OSINFO_TITLE,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX, osInfoX, osInfoY,
      OSINFO_WIDTH, OSINFO_HEIGHT, nullptr, nullptr, this_hinst, nullptr);

  if (!hOsInfoWin) {
    success = false;
  } else {
    ShowWindow(hOsInfoWin, SW_NORMAL);
    success = UpdateWindow(hOsInfoWin);
  }

  return success;
}

void InitOsInfoControls(HWND hWnd, HINSTANCE hInst) {
  // Create text edit control
  hOsInfoTextOut = CreateWindowExW(
      0, WC_EDIT, nullptr, dwCHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
      PADDING_X, PADDING_Y, OSINFO_WIDTH - END_PADDING,
      GetYOffset(OSINFO_HEIGHT, 0, 0.60f) - (END_PADDING * 2), hWnd, (HMENU)IDC_OSINFO_OUT, hInst,
      nullptr);
  hOsInfoStatusBar =
      CreateWindowExW(0, STATUSCLASSNAME, nullptr, dwCHILD | SBARS_SIZEGRIP | SBARS_TOOLTIPS,
                      CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hWnd, nullptr, hInst, nullptr);
  hWinVerButton =
      CreateWindowExW(0, WC_BUTTON, WINVER_BUTTON, dwCHILD | dwBUTTON | SS_NOTIFY, PADDING_X,
                      GetYOffset(OSINFO_HEIGHT, 0, 0.60f) - END_PADDING, kOsInfoButtonWidth,
                      BUTTON_HEIGHT, hWnd, (HMENU)IDC_WINVER_BUTTON, hInst, nullptr);
  hMsInfoButton =
      CreateWindowExW(0, WC_BUTTON, MSINFO_BUTTON, dwCHILD | dwBUTTON | SS_NOTIFY,
                      PADDING_X + kOsInfoButtonWidth + INTRA_PADDING,
                      GetYOffset(OSINFO_HEIGHT, 0, 0.60f) - END_PADDING, kOsInfoButtonWidth,
                      BUTTON_HEIGHT, hWnd, (HMENU)IDC_MSINFO_BUTTON, hInst, nullptr);
  hRunAppButton = CreateWindowExW(
      0, WC_BUTTON, RUN_BUTTON, dwCHILD | dwBUTTON | SS_NOTIFY, PADDING_X,
      GetYOffset(OSINFO_HEIGHT, 0, 0.60f) + BUTTON_HEIGHT + INTRA_PADDING - END_PADDING,
      kOsInfoButtonWidth, BUTTON_HEIGHT, hWnd, (HMENU)IDC_RUNAPP_BUTTON, hInst, nullptr);
  hCloseOSInfoButton = CreateWindowExW(
      0, WC_BUTTON, CLOSE_OI_BUTT, dwCHILD | dwBUTTON | SS_NOTIFY,
      PADDING_X + kOsInfoButtonWidth + INTRA_PADDING,
      GetYOffset(OSINFO_HEIGHT, 0, 0.60f) + BUTTON_HEIGHT + INTRA_PADDING - END_PADDING,
      kOsInfoButtonWidth, BUTTON_HEIGHT, hWnd, (HMENU)IDC_CLOSE_OSINFO_BUTTON, hInst, nullptr);
  SetFontAllControls(hWnd, kMainFont);
  AddTooltip(hWnd, hOsInfoTextOut, hInst, L"Windows System Info Log");
  AddTooltip(hWnd, hWinVerButton, hInst, L"Open Windows Version shell dialog");
  AddTooltip(hWnd, hMsInfoButton, hInst, L"Open \"System Information\" Utility");
  AddTooltip(hWnd, hRunAppButton, hInst, L"Open \"Run\" Dialog");
  AddTooltip(hWnd, hCloseOSInfoButton, hInst, L"Close this OS Info Utilities Window");
  OutputOsInfo(hWnd);
}

void HandleOsInfoResize(HWND hWnd) {
  // Status bar self-sizes via WM_SIZE, always keep it in sync.
  SendMessageW(hOsInfoStatusBar, WM_SIZE, 0, 0);

  // Skip layout computation if dimensions haven't actually changed.
  static int last_width  = -1;
  static int last_height = -1;
  if (this_width == last_width && this_height == last_height) {
    return;
  }
  last_width  = this_width;
  last_height = this_height;

  RECT hStatusRect;
  unsigned int status_height = 0;
  if (GetWindowRect(hOsInfoStatusBar, &hStatusRect)) {
    int height    = hStatusRect.bottom - hStatusRect.top;
    status_height = static_cast<unsigned int>(height);
  }
  const unsigned int kOsOutputWidth = this_width - END_PADDING;
  const unsigned int kOsOutputHeight =
      GetYOffset(this_height, 0, 0.75f) - status_height - END_PADDING;
  const unsigned int kButtonTop  = kOsOutputHeight + (PADDING_Y * 2u);
  const unsigned int kButton2Top = kButtonTop + BUTTON_HEIGHT + INTRA_PADDING;
  const int total_buttons_width =
      static_cast<int>((kOsInfoButtonWidth * 2u) + (INTRA_PADDING * 2u) + PADDING_X);
  const bool is_compact           = this_width <= total_buttons_width;
  const unsigned int kButtonWidth = is_compact ? kOsInfoButtonWidth - 16u : kOsInfoButtonWidth;
  const unsigned int kButtonLeft =
      is_compact ? PADDING_X : GetXOffset(this_width, 0, 0.50f) - kButtonWidth;

  // Move all controls atomically to avoid intermediate redraws between each move.
  HDWP hdwp = BeginDeferWindowPos(5);
  if (hdwp) {
    hdwp = DeferWindowPos(hdwp, hOsInfoTextOut, nullptr,
                          PADDING_X, PADDING_Y, kOsOutputWidth, kOsOutputHeight,
                          SWP_NOZORDER | SWP_NOACTIVATE);
  }
  if (hdwp) {
    hdwp = DeferWindowPos(hdwp, hWinVerButton, nullptr,
                          kButtonLeft, kButtonTop, kButtonWidth, BUTTON_HEIGHT,
                          SWP_NOZORDER | SWP_NOACTIVATE);
  }
  if (hdwp) {
    hdwp = DeferWindowPos(hdwp, hRunAppButton, nullptr,
                          kButtonLeft, kButton2Top, kButtonWidth, BUTTON_HEIGHT,
                          SWP_NOZORDER | SWP_NOACTIVATE);
  }
  if (hdwp) {
    hdwp = DeferWindowPos(hdwp, hMsInfoButton, nullptr,
                          kButtonLeft + kButtonWidth + INTRA_PADDING, kButtonTop,
                          kButtonWidth, BUTTON_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
  }
  if (hdwp) {
    hdwp = DeferWindowPos(hdwp, hCloseOSInfoButton, nullptr,
                          kButtonLeft + kButtonWidth + INTRA_PADDING, kButton2Top,
                          kButtonWidth, BUTTON_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
  }
  if (hdwp) {
    EndDeferWindowPos(hdwp);
  }
}

LRESULT CALLBACK OsInfoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  const HINSTANCE this_hinst = GetGlobalHinst();
  switch (uMsg) {
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDC_WINVER_BUTTON:
          RunShellApplet(hWnd, kWinVerExe);
          break;
        case IDC_MSINFO_BUTTON:
          RunShellApplet(hWnd, kMsInfo32Exe);
          break;
        case IDC_RUNAPP_BUTTON:
          OpenRunDialog(hWnd);
          break;
        case IDC_CLOSE_OSINFO:
        case IDC_CLOSE_OSINFO_BUTTON:
          CloseWindow(hWnd);
          DestroyWindow(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    case WM_CREATE:
      InitOsInfoControls(hWnd, this_hinst);
      break;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
      HDC hdc                              = reinterpret_cast<HDC>(wParam);
      HWND hThisEditControl                = reinterpret_cast<HWND>(lParam);
      static const COLORREF bgColorDefault = GetSysColor(COLOR_WINDOW);
      static const HBRUSH hBrushDefault    = CreateSolidBrush(bgColorDefault);
      static const COLORREF kTextColor     = RGB_GREEN;
      bool set_color                       = false;
      COLORREF kWindowBackground           = bgColorDefault;
      HBRUSH hBrushToUse                   = hBrushDefault;
      // Check which edit control sent the message
      switch (GetDlgCtrlID(hThisEditControl)) {
        case IDC_OSINFO_OUT: {
          kWindowBackground = RGB_DARKGREY;
          hBrushToUse       = CreateSolidBrush(kWindowBackground);
          set_color         = true;
        } break;
        default:
          break;
      }
      if (set_color) {
        SetBkColor(hdc, kWindowBackground);
        SetTextColor(hdc, kTextColor);
        return reinterpret_cast<LRESULT>(hBrushToUse);
      } else {
        return reinterpret_cast<LRESULT>(hBrushDefault);
      }
    } break;
    case WM_SIZE: {
      this_width  = LOWORD(lParam);
      this_height = HIWORD(lParam);
      HandleOsInfoResize(hWnd);
    } break;
    case WM_GETMINMAXINFO: {
      LPMINMAXINFO pMinMaxInfo      = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = 320;
      pMinMaxInfo->ptMinTrackSize.y = 280;
      pMinMaxInfo->ptMaxTrackSize.x = 640;
      pMinMaxInfo->ptMaxTrackSize.y = 480;
    } break;
    case WM_CLOSE:
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY: {
      if (UnregisterClassW(szOSInfoWindowClass, this_hinst)) {
        hOsInfoWin = nullptr;
      }
    } break;
    case WM_NCDESTROY:
      hOsInfoWin = nullptr;
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
  std::wstring kNTVer              = L"";
  const unsigned long short_nt_ver = GetShortNTVer();
  std::wostringstream wostr;
  wostr << L"Raw NTVER: " << std::fixed << std::setprecision(8) << std::showbase << std::hex
        << short_nt_ver << std::noshowbase << std::dec;
  kNTVer = wostr.str();
  wostr.str(L"");
  wostr.clear();
  LOG(DEBUG) << kNTVer.c_str();
  SendMessageW(hOsInfoTextOut, EM_SETREADONLY, TRUE, 0);
  if (hStatusBar) {
    SendMessageW(hOsInfoStatusBar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(kNTVer.c_str()));
  }
}

void LogOsInfo() {
  pGetOsInfoDllVersionW GetOsInfoDllVersionW_t =
      reinterpret_cast<pGetOsInfoDllVersionW>(GetProcAddress(hOsInfoDll, "GetOsInfoDllVersionW"));
  std::wstring OsInfoDllVer;
  DWORD error;
  if (GetOsInfoDllVersionW_t == nullptr) {
    error = GetLastError();
    LOG(ERROR) << L"Failed to get GetOsInfoDllVersionW function address. Error: " << error;
  } else {
    OsInfoDllVer = GetOsInfoDllVersionW_t();
  }
  LOG(INFO) << kOsInfoDll << L" ver. " << OsInfoDllVer;
  LOG(INFO) << L"Windows Version: " << GetWinVersionW() << L" " << GetOSNameW();
  const unsigned long long nt_ver = GetRawNTVer();
  if (debug_mode) {
    LOG(DEBUG) << std::fixed << std::showbase << std::hex << L"GetRawNTVer result = " << nt_ver
               << std::noshowbase << std::dec << std::defaultfloat;
    TestDllGetVersion();
    pIsWoW64 IsWoW64_t = reinterpret_cast<pIsWoW64>(GetProcAddress(hOsInfoDll, "IsWoW64"));
    bool is_wow64;
    if (IsWoW64_t == nullptr) {
      error = GetLastError();
      LOG(ERROR) << L"Failed to get IsWoW64 function address. Error: " << error;
      is_wow64 = false;
    } else {
      is_wow64 = IsWoW64_t();
    }
    static std::wstring result = is_wow64 ? L"TRUE" : L"FALSE";
    LOG(DEBUG) << L"Running under WoW64: " << result;
  }
}

bool TestDllGetVersion() {
  bool success = false;
  DLLGETVERSIONPROC fnDllGetVersion;
  DLLVERSIONINFO dvi   = {sizeof(dvi)};
  DLLVERSIONINFO2 dvi2 = {sizeof(dvi2)};
  DWORD dwVersion      = 0;
  DWORD dwVersion2     = 0;
  DWORD error;
  if (!hOsInfoDll || hOsInfoDll == nullptr) {
    error = GetLastError();
    LOG(ERROR) << L"hOsInfoDll was null! Error: " << error;
    fnDllGetVersion = nullptr;
    return false;
  } else {
    fnDllGetVersion =
        reinterpret_cast<DLLGETVERSIONPROC>(GetProcAddress(hOsInfoDll, "DllGetVersion"));
  }
  if (fnDllGetVersion == nullptr) {
    error = GetLastError();
    LOG(ERROR) << L"Failed to get DllGetVersion address. Error: " << error;
  } else {
    HRESULT hr  = fnDllGetVersion(&dvi);
    HRESULT hr2 = fnDllGetVersion(reinterpret_cast<DLLVERSIONINFO*>(&dvi2));
    if (hr == S_OK && hr2 == S_OK) {
      dwVersion  = _PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
      dwVersion2 = _PACKVERSION(dvi2.info1.dwMajorVersion, dvi2.info1.dwMinorVersion);
      success    = true;
    } else {
      error = GetLastError();
      LOG(ERROR) << L"Failed to run DllGetVersion. Error: " << error;
    }
  }
  if (success) {
    LOG(DEBUG) << L"DLLVERSIONINFO dwVersion = " << std::showbase << std::hex << dwVersion
               << std::dec << std::defaultfloat;
    LOG(DEBUG) << L"DLLVERSIONINFO dwMajorVersion.dwMinorVersion.dwBuildNumber = "
               << dvi.dwMajorVersion << L"." << dvi.dwMinorVersion << L"." << dvi.dwBuildNumber;
    LOG(DEBUG) << L"DLLVERSIONINFO2 dwVersion = " << std::showbase << std::hex << dwVersion2
               << std::dec << std::defaultfloat;
    LOG(DEBUG) << L"DLLVERSIONINFO2 dwMajorVersion.dwMinorVersion.dwBuildNumber = "
               << dvi2.info1.dwMajorVersion << L"." << dvi2.info1.dwMinorVersion << L"."
               << dvi2.info1.dwBuildNumber;
    LOG(DEBUG) << L"DLLVERSIONINFO2 ullVersion = " << dvi2.ullVersion;
  } else {
    LOG(ERROR) << __FUNC__ << L"() Failed! Error: " << error;
  }

  return success;
}
