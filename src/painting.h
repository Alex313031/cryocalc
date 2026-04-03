#ifndef CRYOCALC_PAINTING_H_
#define CRYOCALC_PAINTING_H_

#include "framework.h"
#include "utils.h"

// Returns a font from a given font name string
HFONT GetFont(const std::wstring& in_font);

// Sets font for a window/control
bool SetFontForControl(HWND hWnd, const std::wstring& font);

// Sets a font for all child windows of a given parent Window HWND.
bool SetFontAllControls(HWND hParentWnd, const std::wstring& font);

// Loads funny beach ball .ico as a bitmap, to use later for animations
bool LoadBeachBall();

// Easter egg that bounces beach ball around hWnd's client area.
// Calling again with the same window stops the animation (toggle).
// Calling with a different window stops the current bounce and starts a new one.
void BounceBeachBall(HWND hWnd);

#endif // CRYOCALC_PAINTING_H_
