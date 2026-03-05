#include "ui_utils.h"

#include "resource.h"

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
