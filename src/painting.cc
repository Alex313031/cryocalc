#include "painting.h"

#include "globals.h"

HFONT hCryoFont;

bool CreateMainFont() {
  HDC hdc = GetDC(NULL);
  int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(NULL, hdc);

  hCryoFont = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                           DEFAULT_PITCH | FF_SWISS, L"Arial");
  return hCryoFont != NULL;
}
