#ifndef CRYOCALC_CONSTANTS_H_
#define CRYOCALC_CONSTANTS_H_

#include "framework.h"
#include "resource.h"

// Name of the app
inline const std::wstring kAppName = APP_NAME;

// The title bar text, non-translatable
constexpr const WCHAR* CAPTION_TITLE = APP_NAME;

// File name constants
inline const wchar_t* kOsInfoDll    = L"osinfo.dll";
inline const wchar_t* kShell32Dll   = L"shell32.dll";
inline const wchar_t* kChmHelpFile  = L"cryocalc.chm";
inline const wchar_t* kLogFileName  = L"cryocalc.log";
inline const wchar_t* kIniFileName  = L"cryocalc.ini";
inline const wchar_t* kWinVerExe    = L"winver.exe";
inline const wchar_t* kMsInfo32Exe  = L"msinfo32.exe";

// Min/Max decimal precision for output.
constexpr unsigned int MIN_PRECISION = 0u;
constexpr unsigned int MAX_PRECISION = 4u;

// Default precision to set to on startup
constexpr unsigned int DEFAULT_PRECISION = 3u;

// Natural Constants needed for temp conversion functions.
constexpr long double KELVIN_OFFSET = 273.150L;
constexpr long double RANKINE_OFFSET = 459.670L;

// Dummy values for feeding unit test functions
constexpr long double kDummyCelsius = 0.000L; // Freezing point of water
constexpr long double kDummyKelvin = 77.000L; // Boiling point of Liquid Nitrogen
constexpr long double kDummyFahrenheit = 212.000L; // Boiling point of water
constexpr long double kDummyRankine = 671.641L; // Boiling point of water

// Static values
constexpr unsigned int CW_MAINWIDTH  = 404u; // Width of main window
constexpr unsigned int CW_MAINHEIGHT = 320u; // Height of main window
constexpr unsigned int CW_STATICLABEL_HEIGHT = 24u; // Height of static text label controls
constexpr unsigned int CW_EDITCONTROL_HEIGHT = CW_STATICLABEL_HEIGHT; // Height of edit controls

// Padding metrics
constexpr UINT PADDING_X     = 4u;  // Minimum pixels to pad all content on the left and right
constexpr UINT PADDING_Y     = 4u;  // Minimum pixels to pad all content on the top and bottom
constexpr UINT END_PADDING   = PADDING_Y * 2u; // Minimum pixels to pad all content vertically for resizing
constexpr INT STATIC_TOP     = 12;  // Minimum pixels to pad all content from top
constexpr INT STATIC_LEFT    = 8;  // How many pixels to pad left side before labels
constexpr INT STATIC_RIGHT   = 8;  // How many pixels to pad right side after labels
constexpr INT STATIC_BOTTOM  = 12;  // How many pixels to pad bottom

// Control layout metrics
constexpr UINT LABEL_WIDTH   = 80u;  // Width in pixels of a static label
constexpr UINT EDIT_WIDTH    = 80u;  // Width in pixels of an edit control
constexpr UINT BUTTON_Y      = CW_MAINHEIGHT / 2u; // Y pixel units from top of frame to position buttons
constexpr UINT BUTTON_WIDTH  = 75u;  // Width in pixels buttons
constexpr UINT BUTTON_HEIGHT = 30u;  // Height in pixels of buttons
constexpr UINT COMBO_WIDTH   = 40u;  // Width in pixels of comboboxes
constexpr UINT INTRA_PADDING = 3u;   // Value in pixels of intra-item padding
constexpr UINT BOTTOM_AREA   = CW_MAINHEIGHT / 3u;   // Minimum height of bottom buttom controls area
constexpr UINT PROGBAR_WIDTH = LABEL_WIDTH + (EDIT_WIDTH / 2u) + PADDING_X;
constexpr UINT PROGBAR_HEIGHT = CW_STATICLABEL_HEIGHT;

// Minimum/Maximum number of threads for the CPU stressor.
constexpr unsigned int MIN_THREADS = 1u; // Less than one thread doesn't make sense
constexpr unsigned int MAX_THREADS = 128u; // Threadrippers top out at 128

// Color constants
#define RGB_RED RGB(255, 192, 192)
#define RGB_GREEN RGB(192, 255, 192)
#define RGB_BLUE RGB(0, 0, 128)
#define RGB_CYAN RGB(192, 255, 255)
#define RGB_YELLOW RGB(192, 192, 0)
#define RGB_BLACK RGB(0, 0, 0)
#define RGB_WHITE RGB(255, 255, 255)
#define RGB_GREY RGB(48, 48, 48)

#endif // CRYOCALC_CONSTANTS_H_
