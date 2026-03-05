#include "painting.h"

#include "constants.h"
#include "globals.h"

HFONT hCryoFont = nullptr;

static const BYTE GetFontAntiAliasQuality();
static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam);

bool CreateMainFont() {
  hCryoFont = GetFont(kMainFont);
  return hCryoFont != nullptr;
}

HFONT GetFont(const std::wstring in_font) {
  HDC hdc    = GetDC(nullptr);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = in_font.c_str();
  HFONT hGetFont =
      CreateFontW(height,                    // Height of each char
                  0,                         // Width of each char, 0 means auto-set
                  0,                         // Angle
                  0,                         // Angle
                  FW_NORMAL,                 // Font weight, can also use int 0-900
                  false,                     // Italic ?
                  false,                     // Underline
                  false,                     // Strikethrough
                  DEFAULT_CHARSET,           // Charset to use TODO update to UTF-8
                  OUT_DEFAULT_PRECIS,        // GDI subpixel precision for chars
                  CLIP_DEFAULT_PRECIS,       // GDI subpixel precision when partially occluded chars
                  GetFontAntiAliasQuality(), // Anti-aliasing quality
                  DEFAULT_PITCH | FF_MODERN, // Horizontal pitch and win32 font family
                  font_to_use);              // Name of font
  if (hGetFont == nullptr) {
    LOG(ERROR) << __func__ << L" failed to get " << in_font << L" font!";
    return nullptr;
  }
  return hGetFont;
}

bool SetFontForControl(HWND hWnd, const std::wstring font) {
  if (!hWnd || font.length() == 0) {
    return false;
  }
  const HFONT font_to_send = GetFont(font);
  SendMessageW(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(font_to_send), TRUE);
  return font_to_send != nullptr;
}

bool SetFontAllControls(HWND hParentWnd, const std::wstring font) {
  if (!hParentWnd || font.length() == 0) {
    return false;
  }
  const HFONT font_to_send = GetFont(font);
  return EnumChildWindows(hParentWnd, SetFontProc, reinterpret_cast<LPARAM>(font_to_send));
}

// Default font quality to use
static const BYTE GetFontAntiAliasQuality() {
  // May also use ANTIALIASED_QUALITY or PROOF_QUALITY
  const BYTE font_quality = use_antialiasing ? CLEARTYPE_QUALITY : DEFAULT_QUALITY;
  return font_quality;
}

static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam) {
  HFONT hFont = reinterpret_cast<HFONT>(lParam);
  // Don't set font on OS Info text edit control
  if (hChild != hOsInfoTextOut) {
    SendMessageW(hChild, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
  }
  return true;
}
