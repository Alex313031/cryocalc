#ifndef CRYOCALC_PAINTING_H_
#define CRYOCALC_PAINTING_H_

#include "framework.h"
#include "utils.h"

// Creates global font hCryoFont
bool CreateMainFont();

// Creates font for input controls
bool CreateEditFont();

// Creates font for logging windows (like the OS Info Window)
bool CreateLogFont(const std::wstring &font);

// Sets font for a window/control
bool SetFontForControl(HWND hWnd, const std::wstring font);

// Sets a font for all child windows of a given parent Window HWND.
bool SetFontAllControls(HWND hParentWnd);

#endif // CRYOCALC_PAINTING_H_
