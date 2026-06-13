#include "ui_utils.h"

#include "painting.h"
#include "resource.h"
#include "strings.h"
#include "utils.h"

// Declare rects to use for all future window layout
RECT kMainClientRect;
RECT kMainWinRect;

static RUN_FILE_DLG_ pfnRunFileDlg = nullptr;

static bool _about_handled = false;

// WH_MSGFILTER hook that gives the About dialog a chance to run TranslateAccelerator
// before IsDialogMessage consumes keyboard messages. Installed on WM_INITDIALOG,
// removed when the dialog exits. DialogBox has no user-controlled message loop, so
// this is the approved Win32 way to support accelerator tables in modal dialogs.
static HHOOK  g_about_hook  = nullptr;
static HWND   g_about_hwnd  = nullptr;
static HACCEL g_about_accel = nullptr;

static LRESULT CALLBACK AboutMsgFilterHook(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == MSGF_DIALOGBOX && g_about_hwnd && g_about_accel) {
    MSG* pMsg = reinterpret_cast<MSG*>(lParam);
    if (TranslateAccelerator(g_about_hwnd, g_about_accel, pMsg)) {
      return 1; // consumed; IsDialogMessage will not process it further
    }
  }
  return CallNextHookEx(g_about_hook, nCode, wParam, lParam);
}

static void InstallAboutAccelHook(HWND hDlg) {
  g_about_accel = LoadAccelerators(GetInstanceFromHwnd(hDlg), MAKEINTRESOURCE(IDD_ABOUTBOX));
  g_about_hook  = SetWindowsHookExW(WH_MSGFILTER, AboutMsgFilterHook, nullptr,
                                    GetCurrentThreadId());
}

static void RemoveAboutAccelHook() {
  if (g_about_hook) {
    UnhookWindowsHookEx(g_about_hook);
    g_about_hook = nullptr;
  }
  if (g_about_accel) {
    DestroyAcceleratorTable(g_about_accel);
    g_about_accel = nullptr;
  }
  g_about_hwnd = nullptr;
}

HWND AddTooltip(HWND hWndParent, HWND hWndControl, HINSTANCE hInst, const wchar_t* tooltipText) {
  if (!hWndParent || !hWndControl || !tooltipText) {
    return nullptr;
  }

  // Create the tooltip window
  HWND hTooltip = CreateWindowExW(
      WS_EX_NOACTIVATE, TOOLTIPS_CLASS, nullptr, TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, nullptr, hInst, nullptr);

  if (!hTooltip) {
    return nullptr;
  }

  // Set up the TOOLINFO structure
  TOOLINFOW ti = {0};
  static const bool can_use_582_controls = IsCommCtrlAtLeast(dwComCtl32TargetVer);
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
  kMainWinRect    = winRect;
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

void InitMenus(HWND hWnd) {
  // We should have a hWnd, but hMainWindow shouldn't be defined yet.
  if (!hWnd || hMainWindow != nullptr) {
    NOTREACHED();
  }
  HMENU hBar      = GetMenu(hWnd);
  HMENU hSettings = hBar ? GetSubMenu(hBar, 2) : nullptr;
  HMENU hSpeed    = hSettings ? GetSubMenu(hSettings, 0) : nullptr;
  if (hSpeed) {
    CheckMenuRadioItem(hSpeed, IDM_SPEED_LOW, IDM_SPEED_HIGH, IDM_SPEED_HIGH, MF_BYCOMMAND);
  }
  if (hBar) {
    const HWND console = logging::GetCurrentConsole();
    const bool con_visible = console && IsWindowVisible(console);
    CheckMenuItem(hBar, IDM_TOGGLE_CON,
                  MF_BYCOMMAND | (con_visible ? MF_CHECKED : MF_UNCHECKED));
  }
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
  } else {
    *outX = kDefaultX;
    *outY = kDefaultY;
  }
}

bool ConfirmClearControls(HWND hWnd) {
  int user_response_code = MessageBoxW(nullptr, L"Clear All Temperature Output?", L"Confirm Clear",
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

// Restore parent window to foreground, optional child window HWND to set keyboard focus
bool ResetFocus(HWND foreground, HWND keyb_focus) {
  bool success        = false;
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
    success    = set_foreground && prev_focus != nullptr;
  }
  return success;
}

// Opens the "Run" shell dialog from shell32.dll
void OpenRunDialog(HWND hWnd) {
  static HICON kSmallIcon = LoadIcon(GetInstanceFromHwnd(hWnd), MAKEINTRESOURCE(IDI_WINFLAG));
  if (kSmallIcon) {
    wchar_t szCurDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, szCurDir);
    // Open "Run"
    HMODULE hShell32Dll = GetModuleHandleW(L"shell32.dll");
    if (hShell32Dll) {
      pfnRunFileDlg = reinterpret_cast<RUN_FILE_DLG_>(GetProcAddress(hShell32Dll, (LPCSTR)(61)));
      if (pfnRunFileDlg) {
        LOG(INFO) << L"Opening RunFileDlg";
        pfnRunFileDlg(hWnd, kSmallIcon, (LPWSTR)szCurDir, RUN_TITLE, RUN_PROMPT,
                      RFD_USEFULLPATHDIR | RFD_WOW_APP);
      } else {
        LOG(ERROR) << L"Failed to open run dialog.";
      }
    } else {
      LOG(ERROR) << L"Failed to get shell32.dll handle.";
    }
    DestroyIcon(kSmallIcon); // Cleanup icon
  }
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
      wostr << L"Error = " << std::showbase << std::hex << error << std::dec << std::defaultfloat;
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
  size_t cache_size = 1024u; // Default cache size
  // Set cache size based on selection in combobox
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
    LOG(ERROR) << L"Default cache size was used!" << cache_size;
  }
  return cache_size;
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

bool HandleChildClick(HWND parent, HWND child) {
  if (!parent || !child) {
    return false;
  }
  if (child == hCPUBar || child == hCPUPercent || child == hMemBar || child == hMemPercent ||
      child == hCommitBar || child == hCommitPercent || child == hIOBar || child == hIOPercent) {
    return PostMessageW(parent, WM_OPEN_MONITOR_WIN, 0, 0);
  }
  return true;
}

void SetAboutHandled(bool handled) {
  _about_handled = handled;
}

bool GetAboutHandledState() {
  return _about_handled;
}

bool ShowAboutDialog(HWND hWnd) {
  const HINSTANCE gHinst = GetGlobalHinst();
  // Show "About" dialog box
  PlaySoundW(L"SystemNotification", nullptr, SND_ALIAS | SND_ASYNC);
  DialogBoxW(gHinst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
  bool handled_dialog = GetAboutHandledState();
  if (handled_dialog) {
    LOG(INFO) << L"Showed about dialog.";
  } else {
    LOG(ERROR) << L"About dialog failed!";
  }
  return handled_dialog;
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);

  if (g_about_hwnd == nullptr) {
    g_about_hwnd = hDlg; // Set as early as possible, before any message is processed.
  }

  bool AboutHandled = false; // Stores status of whether dialog has been handled user-wise.
  switch (message) {
    case WM_INITDIALOG: {
      // Set icon in titlebar of about dialog
      static const HICON kAboutIcon = LoadIcon(GetInstanceFromHwnd(hDlg), MAKEINTRESOURCE(IDI_ABOUT));
      SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)kAboutIcon);
      SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)kAboutIcon);
      InstallAboutAccelHook(hDlg);
      AboutHandled = true;
      SetAboutHandled(AboutHandled);
    } break;
    case WM_DESTROY:
      RemoveAboutAccelHook();
      break;
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDM_EASTEREGG:
          BounceBeachBall(hDlg);
          break;
        case IDOK:
        case IDCANCEL: {
          // Exit the dialog
           if (EndDialog(hDlg, LOWORD(wParam))) {
             AboutHandled = true;
            SetAboutHandled(AboutHandled);
            return (INT_PTR)AboutHandled;
          } else {
            AboutHandled = false;
            SetAboutHandled(AboutHandled);
            break;
          }
       } break;
       default:
         break;
      }
    } break;
    default:
      SetAboutHandled(true);
      break;
  }

  // About dialog failed
  return (INT_PTR)AboutHandled;
}
