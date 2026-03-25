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
constexpr unsigned int MAINWIDTH  = 640u; // Default width of main window
constexpr unsigned int MAINHEIGHT = 480u; // Default height of main window
constexpr unsigned int MINWIDTH   = 480u; // Min width of main window
constexpr unsigned int MINHEIGHT  = 384u; // Min height of main window
constexpr unsigned int MAXWIDTH   = 868u; // Max width of main window
constexpr unsigned int MAXHEIGHT  = 680u; // Max height of main window

constexpr unsigned int OSINFO_WIDTH     = 400u; // Default width of OS Info Window
constexpr unsigned int OSINFO_HEIGHT    = 320u; // Default height of OS Info window
constexpr unsigned int OSINFO_MINWIDTH  = 340u; // Min width of OS Info Window
constexpr unsigned int OSINFO_MINHEIGHT = 300u; // Min height of OS Info window
constexpr unsigned int OSINFO_MAXWIDTH  = MAINWIDTH; // Max width of OS Info Window
constexpr unsigned int OSINFO_MAXHEIGHT = MAXHEIGHT; // Max height of OS Info window

constexpr unsigned int MONWIN_WIDTH     = MAINWIDTH;  // Default width of monitor Window
constexpr unsigned int MONWIN_HEIGHT    = MAINHEIGHT; // Default height of monitor window
constexpr unsigned int MONWIN_MINWIDTH  = 200u; // Min width of monitor window
constexpr unsigned int MONWIN_MINHEIGHT = 180u; // Min height of monitor window
constexpr unsigned int MONWIN_MAXWIDTH  = (MAXWIDTH * 2u); // Max. width of monitor window
constexpr unsigned int MONWIN_MAXHEIGHT = (MAXHEIGHT * 2u); // Max. height of monitor window

constexpr unsigned int STATICLABEL_HEIGHT = 24u;  // Height of static text label controls
constexpr int SMALLLABEL_HEIGHT  = 16;  // Height of static text in small labels
constexpr unsigned int EDITCONTROL_HEIGHT = STATICLABEL_HEIGHT; // Height of edit controls

// Padding metrics
constexpr inline UINT PADDING_X     = 4u; // Min. pixels to pad all content on the left and right
constexpr inline UINT PADDING_Y     = 4u; // Min. pixels to pad all content on the top and bottom
constexpr inline UINT INTRA_PADDING = 4u; // Value in pixels of intra-item padding
constexpr inline UINT END_PADDING   = 8u; // Min. pixels to pad all content at end diagonally
constexpr inline INT STATIC_TOP     = 12; // Min. pixels to pad all content from top
constexpr inline INT STATIC_LEFT    = 8;  // How many pixels to pad left side before labels
constexpr inline INT STATIC_RIGHT   = 8;  // How many pixels to pad right side after labels
constexpr inline INT STATIC_BOTTOM  = 12; // How many pixels to pad bottom
constexpr INT ADJ_WINDOW_PADDING = 2; // How many pixels to pad between adjacent parent windows

// Control layout metrics
constexpr UINT LABEL_WIDTH   = 80u;             // Width in pixels of a static label
constexpr UINT EDIT_WIDTH    = 80u;             // Width in pixels of an edit control
constexpr UINT BUTTON_Y      = MAINHEIGHT / 2u; // Y pixel units from top of frame to top of buttons
constexpr UINT BUTTON_WIDTH  = 75u;             // Width in pixels buttons
constexpr UINT BUTTON_HEIGHT = 30u;             // Height in pixels of buttons
constexpr UINT COMBO_WIDTH   = 40u;             // Width in pixels of comboboxes
constexpr UINT COMBO_HEIGHT  = 200u;            // Combobox height (must be > edit height for Win2K)
constexpr UINT BOTTOM_AREA   = MAINHEIGHT / 3u; // Minimum height of bottom buttom controls area
// Progress bar metrics
constexpr UINT PROGBAR_WIDTH  = LABEL_WIDTH + (EDIT_WIDTH / 2u) + PADDING_X;
constexpr UINT PROGBAR_HEIGHT = STATICLABEL_HEIGHT;
// Invert values for vertical progress bars (i.e. cpu bars)
constexpr UINT CPUBAR_HEIGHT  = PROGBAR_WIDTH;
constexpr UINT CPUBAR_WIDTH   = PROGBAR_HEIGHT + 4;
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
#define RGB_DARKRED   RGB(96, 0, 0)
#define RGB_DARKGREEN RGB(0, 96, 0)
#define RGB_DARKBLUE  RGB(0, 0, 96)

// Internal temp scale representation
enum Scale {
  kScaleCelsius    = 0,
  kScaleKelvin     = 1,
  kScaleFahrenheit = 2,
  kScaleRankine    = 3,
  kScaleUnknown    = -1,
  kMaxScale        = 4
};

// Posted (not sent) to hMainWindow to open the CPU monitor window.
// Using PostMessage avoids re-entrancy: on Windows 2000/XP, PROGRESS_CLASS has
// internal child windows whose clicks bubble multiple synchronous WM_PARENTNOTIFY
// messages before OpenMonitorWindow can assign hMonitorWin.
inline constexpr UINT WM_OPEN_MONITOR_WIN = WM_APP + 1;

#endif // CRYOCALC_CONSTANTS_H_
