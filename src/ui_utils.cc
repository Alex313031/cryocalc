#include "ui_utils.h"

#include "resource.h"

// Declare rects to use for all future window layout
RECT kMainClientRect;
RECT kMainWinRect;

HWND AddTooltip(HWND hWndParent, HWND hWndControl, HINSTANCE hInst, const wchar_t* tooltipText) {
  if (!hWndParent || !hWndControl || !tooltipText) {
    return nullptr;
  }

  // Create the tooltip window
  HWND hTooltip = CreateWindowExW(
      0, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, nullptr, hInst, nullptr);

  if (!hTooltip) {
    return nullptr;
  }

  static const bool can_use_582_controls = IsCommCtrlAtLeast(dwComCtl32TargetVer);
  // Set up the TOOLINFO structure
  TOOLINFOW ti = {0};
  if (can_use_582_controls) {
    ti.cbSize = sizeof(ti);
  } else {
    // MinGW's TOOLINFOW always includes lpReserved, but Windows 2000's
    // comctl32 v5.81 only supports up to V2 (through lParam). Using
    // sizeof(ti) gives V3 size which Win2000's TTM_ADDTOOLW rejects.
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
  }
  ti.uFlags   = TTF_SUBCLASS | TTF_IDISHWND;
  ti.hwnd     = hWndParent;
  ti.uId      = reinterpret_cast<UINT_PTR>(hWndControl);
  ti.lpszText = const_cast<wchar_t*>(tooltipText);

  // Associate the tooltip with the control
  SendMessageW(hTooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));

  return hTooltip;
}

bool SetClientRects(HWND hWnd, HINSTANCE hInst) {
  RECT winRect;
  RECT clientRect;
  // Get rects of Window including titlebar, for positioning other windows relative to it.
  if (!GetWindowRect(hWnd, &winRect)) {
    return false;
  }
  // Get rects inside Window, excluding titlebar, for positioning other windows relative to it.
  if (!GetClientRect(hWnd, &clientRect)) {
    return false;
  }
  kMainWinRect = winRect;
  kMainClientRect = clientRect;
  return true;
}

const RECT GetMainWinRect() {
  return kMainWinRect;
}

const RECT GetMainClientRect() {
  return kMainClientRect;
}

const RECT GetDesktopRect(HINSTANCE hInstance) {
  RECT desktopWinRect;
  if (!hInstance) {
    desktopWinRect.left   = 0;
    desktopWinRect.top    = 0;
    desktopWinRect.right  = 0;
    desktopWinRect.bottom = 0;
  } else {
    BOOL gotWorkArea = SystemParametersInfoW(SPI_GETWORKAREA, 0, &desktopWinRect, 0);
    if (!gotWorkArea) {
      desktopWinRect.left   = 0;
      desktopWinRect.top    = 0;
      desktopWinRect.right  = 0;
      desktopWinRect.bottom = 0;
    }
    LOG(DEBUG) << L"Got desktop window rect: L" << desktopWinRect.left << L" T"
               << desktopWinRect.top << L" B" << desktopWinRect.bottom << L" R"
               << desktopWinRect.right;
  }
  return desktopWinRect;
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
  } else {
    *outX = kDefaultX;
    *outY = kDefaultY;
  }
}

int ConfirmExit(HWND hWnd) {
  int user_response_code = MessageBoxW(hWnd, L"Are you sure you want to exit?", L"Confirm Exit",
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

// Restore parent window to foreground, optional child window HWND to set keyboard focus
bool ResetFocus(HWND foreground, HWND keyb_focus) {
  bool success = false;
  bool set_foreground = false;
  if (foreground == nullptr) {
    return false;
  } else {
    set_foreground = SetForegroundWindow(foreground);
  }
  if (keyb_focus == nullptr) {
    success = set_foreground; // Requested to not set keyb focus
  } else {
    HWND prev_focus = nullptr;
    // Set keyboard focus to specified child window
    prev_focus = SetFocus(keyb_focus); // Returns previously focused window
    success = set_foreground && prev_focus != nullptr;
  }
  return success;
}

int InfoBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && hMainWindow != nullptr) {
    hWndTmp = hMainWindow;
  } else {
    hWndTmp = hWnd;
  }
  return MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

int WarnBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && hMainWindow != nullptr) {
    hWndTmp = hMainWindow;
  } else {
    hWndTmp = hWnd;
  }
  return MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONWARNING);
}

int ErrorBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && hMainWindow != nullptr) {
    hWndTmp = hMainWindow;
  } else {
    hWndTmp = hWnd;
  }
  return MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}
