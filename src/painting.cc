#include "painting.h"

#include "constants.h"
#include "globals.h"

HFONT hCryoFont  = nullptr;
HFONT hInputFont = nullptr;
HFONT hLogFont   = nullptr;

bool CreateMainFont() {
  HDC hdc    = GetDC(nullptr);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = kMainFont;
  hCryoFont =
      CreateFontW(height,                    // Height of each char
                  0,                         // Width of each char, 0 means auto-set
                  0,                         // Angle
                  0,                         // Angle
                  FW_LIGHT,                  // Font weight, can also use int 0-900
                  false,                     // Italic ?
                  false,                     // Underline
                  false,                     // Strikethrough
                  DEFAULT_CHARSET,           // Charset to use TODO update to UTF-8
                  OUT_DEFAULT_PRECIS,        // GDI subpixel precision for chars
                  CLIP_DEFAULT_PRECIS |
                      CLIP_STROKE_PRECIS,    // GDI subpixel precision when partially occluded chars
                  DEFAULT_QUALITY,           // Anti-aliasing quality // ANTIALIASED_QUALITY
                  DEFAULT_PITCH | FF_MODERN, // Horizontal pitch and win32 font family
                  font_to_use);              // Name of font
  return hCryoFont != nullptr;
}

bool CreateEditFont() {
  HDC hdc    = GetDC(nullptr);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = kInputFont;
  hInputFont = CreateFontW(height, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS | CLIP_STROKE_PRECIS,
                           DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, font_to_use);
  return hInputFont != nullptr;
}

bool CreateLogFont(const std::wstring& font) {
  HDC hdc    = GetDC(nullptr);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = font.c_str();
  hLogFont = CreateFontW(height, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS | CLIP_STROKE_PRECIS,
                         DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, font_to_use);
  return hLogFont != nullptr;
}

static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam) {
  HFONT hFont = reinterpret_cast<HFONT>(lParam);
  SendMessageW(hChild, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
  return true;
}

static HFONT GetFont(const std::wstring in_font) {
  HDC hdc    = GetDC(nullptr);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = in_font.c_str();
  HFONT hGetFont = CreateFontW(height, 0, 0, 0, FW_ULTRABOLD, false, false, false, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS | CLIP_STROKE_PRECIS,
                               DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, font_to_use);
  if (hGetFont != nullptr) {
    return hGetFont;
  }
  return nullptr;
}

bool SetFontForControl(HWND hWnd, const std::wstring font) {
  if (!hWnd || font.length() == 0) {
    return false;
  }
  const HFONT font_to_send = GetFont(font);
  SendMessageW(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(font_to_send), TRUE);
  return font_to_send != nullptr;
}

bool SetFontAllControls(HWND hParentWnd) {
  if (hCryoFont == nullptr) {
    return false;
  }
  return EnumChildWindows(hParentWnd, SetFontProc, reinterpret_cast<LPARAM>(hCryoFont));
}
