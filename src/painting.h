#ifndef CRYOCALC_PAINTING_H_
#define CRYOCALC_PAINTING_H_

#include "framework.h"
#include "utils.h"

// Creates main hCryoFont
bool CreateMainFont();

// Returns a font from a given font name string
HFONT GetFont(const std::wstring in_font);

// Sets font for a window/control
bool SetFontForControl(HWND hWnd, const std::wstring font);

// Sets a font for all child windows of a given parent Window HWND.
bool SetFontAllControls(HWND hParentWnd, const std::wstring font);

// Loads funny beach ball .ico as a bitmap, to use later for animations
bool LoadBeachBall();

// Easter egg that bounces beach ball around the window
void BounceBeachBall(HBITMAP hBall);

#endif // CRYOCALC_PAINTING_H_
