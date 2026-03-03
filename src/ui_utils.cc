#include "utils.h"

#include "constants.h"
#include "globals.h"
#include "resource.h"
#include "strings.h"

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
