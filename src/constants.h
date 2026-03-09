#ifndef CRYOCALC_CONSTANTS_H_
#define CRYOCALC_CONSTANTS_H_

#include "framework.h"
#include "resource.h"

// Name of the app
inline const std::wstring kAppName = APP_NAME;

// The title bar text, non-translatable
constexpr const WCHAR* CAPTION_TITLE = APP_NAME;

// File name constants
inline const wchar_t* kOsInfoDll   = L"osinfo.dll";
inline const wchar_t* kShell32Dll  = L"shell32.dll";
inline const wchar_t* kComCtl32Dll = L"comctl32.dll";
inline const wchar_t* kChmHelpFile = L"cryocalc.chm";
inline const wchar_t* kLogFileName = L"cryocalc.log";
inline const wchar_t* kIniFileName = L"cryocalc.ini";
inline const wchar_t* kWinVerExe   = L"winver.exe";
inline const wchar_t* kMsInfo32Exe = L"msinfo32.exe";

// Font and GDI constants
inline const wchar_t* kMainFont = L"MS Shell Dlg 2"; // Font to use for all controls by default
constexpr bool use_antialiasing = false;             // Whether to use ClearType and antialiasing

// Min/Max decimal precision for output.
constexpr unsigned int MIN_PRECISION = 0u;
constexpr unsigned int MAX_PRECISION = 4u;

// Default precision to set to on startup
constexpr unsigned int DEFAULT_PRECISION = 3u;

// Natural Constants needed for temp conversion functions.
inline constexpr long double KELVIN_OFFSET  = 273.150L;
inline constexpr long double RANKINE_OFFSET = 459.670L;

// clang-format off
// Dummy values for feeding unit test functions
constexpr long double kDummyCelsius    = 0.000L;   // Freezing point of water
constexpr long double kDummyKelvin     = 77.000L;  // Boiling point of Liquid Nitrogen
constexpr long double kDummyFahrenheit = 212.000L; // Boiling point of water
constexpr long double kDummyRankine    = 671.641L; // Boiling point of water

// Static values
constexpr unsigned int MAINWIDTH          = 480u; // Width of main window
constexpr unsigned int MAINHEIGHT         = 320u; // Height of main window
constexpr unsigned int STATICLABEL_HEIGHT = 24u;  // Height of static text label controls
constexpr unsigned int EDITCONTROL_HEIGHT = STATICLABEL_HEIGHT; // Height of edit controls

// Padding metrics
constexpr UINT PADDING_X    = 4u;             // Min. pixels to pad all content on the left and right
constexpr UINT PADDING_Y    = 4u;             // Min. pixels to pad all content on the top and bottom
constexpr UINT END_PADDING  = PADDING_Y * 2u; // Min. pixels to pad all content at end diagonally
constexpr INT STATIC_TOP    = 12;             // Min. pixels to pad all content from top
constexpr INT STATIC_LEFT   = 8;              // How many pixels to pad left side before labels
constexpr INT STATIC_RIGHT  = 8;              // How many pixels to pad right side after labels
constexpr INT STATIC_BOTTOM = 12;             // How many pixels to pad bottom

// Control layout metrics
constexpr UINT LABEL_WIDTH   = 80u;             // Width in pixels of a static label
constexpr UINT EDIT_WIDTH    = 80u;             // Width in pixels of an edit control
constexpr UINT BUTTON_Y      = MAINHEIGHT / 2u; // Y pixel units from top of frame to top of buttons
constexpr UINT BUTTON_WIDTH  = 75u;             // Width in pixels buttons
constexpr UINT BUTTON_HEIGHT = 30u;             // Height in pixels of buttons
constexpr UINT COMBO_WIDTH   = 40u;             // Width in pixels of comboboxes
constexpr UINT COMBO_HEIGHT  = 200u;            // Combobox height (must be > edit height for Win2K)
constexpr UINT INTRA_PADDING = 3u;              // Value in pixels of intra-item padding
constexpr UINT BOTTOM_AREA   = MAINHEIGHT / 3u; // Minimum height of bottom buttom controls area
// Progress bar metrics
constexpr UINT PROGBAR_WIDTH = LABEL_WIDTH + (EDIT_WIDTH / 2u) + PADDING_X;
constexpr UINT PROGBAR_HEIGHT = STATICLABEL_HEIGHT;
// clang-format on

// Minimum/Maximum number of threads for the CPU stressor.
constexpr unsigned int MIN_THREADS = 1u;   // Less than one thread doesn't make sense
constexpr unsigned int MAX_THREADS = 128u; // Threadrippers top out at 128

// Window style constants, to not clutter up function declarations
inline constexpr DWORD dwCHILD  = WS_CHILD | WS_VISIBLE;
inline constexpr DWORD dwLABEL  = SS_CENTER | SS_CENTERIMAGE;
inline constexpr DWORD dwBUTTON = BS_CENTER | BS_VCENTER | WS_TABSTOP;
inline constexpr DWORD dwCHECKBOX = BS_VCENTER | BS_CHECKBOX | BS_AUTOCHECKBOX;

// Minimum common controls version for certain functions, used for fallback codepaths
// See https://learn.microsoft.com/en-us/windows/win32/controls/common-control-versions
inline constexpr DWORD dwComCtl32TargetVer =
    _PACKVERSION(static_cast<DWORD>(5u), static_cast<DWORD>(82u));

// Color constants
#define RGB_BLACK     RGB(0, 0, 0)
#define RGB_WHITE     RGB(255, 255, 255)
#define RGB_RED       RGB(255, 0, 0)
#define RGB_GREEN     RGB(0, 255, 0)
#define RGB_BLUE      RGB(0, 0, 255)
#define RGB_YELLOW    RGB(255, 255, 0)
#define RGB_REDISH    RGB(255, 192, 192)
#define RGB_GREENISH  RGB(192, 255, 192)
#define RGB_BLUEISH   RGB(0, 0, 128)
#define RGB_YELLOWISH RGB(192, 192, 0)
#define RGB_CYAN      RGB(192, 255, 255)
#define RGB_DARKGREY  RGB(32, 32, 32)

// Internal temp scale representation
enum Scale {
  kScaleCelsius    = 0,
  kScaleKelvin     = 1,
  kScaleFahrenheit = 2,
  kScaleRankine    = 3,
  kScaleUnknown    = -1,
  kMaxScale        = 4
};

#endif // CRYOCALC_CONSTANTS_H_
