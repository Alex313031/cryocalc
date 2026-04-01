#include "painting.h"

#include "constants.h"
#include "globals.h"
#include "ui_utils.h"

static HINSTANCE ball_hinst = nullptr; // Simply set to main cryocalc HINSTANCE

static HWND BallWindow = nullptr; // Transparent window for ball
HBITMAP hBeachBall     = nullptr; // Our beach ball bitmap, made from ball_icon
static HICON ball_icon = nullptr; // HICON kept for DrawIconEx, should be 32x32

// How often to update ball position
static inline const UINT kBallInterval() {
  // (~60 fps, 1000/60 ≈ 16.667, rounds to 17)
  return static_cast<unsigned int>(std::round(16.667f));
}

// Beach ball bounce state
static UINT_PTR bounce_timer = 0; // Timer for each ball movement

static float ball_x       = 0.0f; // Stores X position of ball
static float ball_y       = 0.0f; // Stores Y position of ball
static float ball_offsetx = 3.0f; // How much to move ball across
static float ball_offsety = 2.25f; // How much to move ball down
static int ball_w         = 32; // Icon is 32 pixels wide
static int ball_h         = 32; // Icon is 32 pixels high

// Window class for ball child window
static const LPCWSTR czBallClass = L"CryoCalcBallClass";
// Whether BallWindow class was registered
static bool g_ball_class_registered = false;

static const BYTE GetFontAntiAliasQuality();
static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam);
static LRESULT CALLBACK BallWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static VOID CALLBACK BallTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

HFONT GetFont(const std::wstring& in_font) {
  HDC hdc = GetDC(nullptr);
  if (!hdc || in_font.empty()) {
    return nullptr;
  }
  const int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
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

bool SetFontForControl(HWND hWnd, const std::wstring& font) {
  if (!hWnd || font.length() == 0) {
    return false;
  }
  const HFONT font_to_send = GetFont(font);
  if (font_to_send != nullptr) {
    SendMessageW(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(font_to_send), TRUE);
    return true;
  } else {
    LOG(ERROR) << L"Failed to set font for HWND " << hWnd;
    return false;
  }
}

bool SetFontAllControls(HWND hParentWnd, const std::wstring& font) {
  if (!hParentWnd || font.length() == 0) {
    return false;
  }
  const HFONT font_to_send = GetFont(font);
  if (font_to_send != nullptr) {
    return EnumChildWindows(hParentWnd, SetFontProc, reinterpret_cast<LPARAM>(font_to_send));
  } else {
    LOG(ERROR) << L"Failed to set font for child windows of HWND " << hParentWnd;
    return false;
  }
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

// Loads global beach ball bitmap, to use below in bouncing animation
bool LoadBeachBall() {
  ball_hinst = GetGlobalHinst(); // Set hinst once for other stuff
  ball_icon = LoadIcon(ball_hinst, MAKEINTRESOURCE(IDI_BALL));
  if (!ball_icon) {
    return false;
  }
  ICONINFO iconInfo = {};
  if (!GetIconInfo(ball_icon, &iconInfo)) {
    return false;
  }

  // hbmColor is the actual color bitmap handle.
  // The mask bitmap (hbmMask) is also available in iconInfo.
  // Note: The caller is responsible for deleting the bitmaps
  // returned in the ICONINFO structure when they are no longer needed.
  if (iconInfo.hbmMask != nullptr) {
    DeleteObject(iconInfo.hbmMask);
  }
  hBeachBall = iconInfo.hbmColor;
  // Read the actual icon dimensions for use in bounce layout calculations.
  if (hBeachBall) {
    BITMAP bm = {};
    if (GetObject(hBeachBall, sizeof(bm), &bm)) {
      if (bm.bmWidth > 0) {
        ball_w = bm.bmWidth;
      }
      if (bm.bmHeight > 0) {
        ball_h = bm.bmHeight;
      }
    }
  }
  return hBeachBall != nullptr;
}

// WndProc for the bouncing ball child window.
// WS_EX_TRANSPARENT ensures it paints after siblings (visually on top)
// and does not steal mouse clicks from controls beneath it.
static LRESULT CALLBACK BallWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      if (hdc && ball_icon) {
        DrawIconEx(hdc, 0, 0, ball_icon, ball_w, ball_h, 0, nullptr, DI_NORMAL);
      }
      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_ERASEBKGND:
      // Suppress background erase; DrawIconEx applies the icon mask for
      // transparency, so the parent's content shows through the corners.
      return 1;
    case WM_DESTROY:
      // Kill the move timer when the ball window is destroyed (covers both
      // explicit toggle-off and parent-window teardown).
      if (bounce_timer) {
        KillTimer(nullptr, bounce_timer);
        bounce_timer = 0;
      }
      BallWindow = nullptr;
      return 0;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

// Timer callback: moves the ball each tick and bounces it off the client edges.
static VOID CALLBACK BallTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
  if (!BallWindow || !hMainWindow) {
    return;
  }
  RECT client;
  if (GetClientRect(hMainWindow, &client)) {
    ball_x += ball_offsetx;
    ball_y += ball_offsety;
    if (ball_x <= 0.0f) {
      ball_x  = 0.0f;
      ball_offsetx = -ball_offsetx;
    } else if (ball_x + static_cast<float>(ball_w) >= static_cast<float>(client.right)) {
      ball_x  = static_cast<float>(client.right - ball_w);
      ball_offsetx = -ball_offsetx;
    }
    if (ball_y <= 0.0f) {
      ball_y  = 0.0f;
      ball_offsety = -ball_offsety;
    } else if (ball_y + static_cast<float>(ball_h) >= static_cast<float>(client.bottom)) {
      ball_y  = static_cast<float>(client.bottom - ball_h);
      ball_offsety = -ball_offsety;
    }

    SetWindowPos(BallWindow, HWND_TOP, static_cast<int>(ball_x), static_cast<int>(ball_y), 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOCOPYBITS);
  }
}

// Bounces the beach ball icon around the client area "DVD player style",
// at the top of the Z-order. This is an easter egg, launch with "Ctrl + B".
// Calling a second time while bouncing stops the animation (toggle).
void BounceBeachBall() {
  if (ball_icon == nullptr || hBeachBall == nullptr || ball_hinst == nullptr) {
    LOG(ERROR) << __FUNC__ << L"() Ball bounce failed!";
    ErrorBox(nullptr, L"Easter Egg Error", L"Ball Bounce Failed!");
    return;
  }

  // Toggle off if already running.
  if (BallWindow != nullptr || bounce_timer != 0) {
    if (BallWindow) {
      DestroyWindow(BallWindow); // timer is killed inside WM_DESTROY
    } else if (bounce_timer) {
      KillTimer(nullptr, bounce_timer);
      bounce_timer = 0;
    }
    LOG(INFO) << L"Stopped ball bounce.";
    return;
  }

  LOG(INFO) << L"Starting ball bounce!";

  // Register the ball window class once for the lifetime of the process.
  if (!g_ball_class_registered) {
    WNDCLASSW wc     = {};
    wc.lpfnWndProc   = BallWndProc;
    wc.hInstance     = ball_hinst;
    wc.lpszClassName = czBallClass;
    RegisterClassW(&wc);
    g_ball_class_registered = true;
  }

  // Pick a pseudo-random starting position within the client area.
  RECT client;
  GetClientRect(hMainWindow, &client);
  const int max_x = client.right > ball_w ? client.right - ball_w : 0;
  const int max_y = client.bottom > ball_h ? client.bottom - ball_h : 0;
  const DWORD tick = GetTickCount();
  ball_x  = max_x > 0 ? static_cast<float>(tick % static_cast<DWORD>(max_x)) : 0.0f;
  ball_y  = max_y > 0 ? static_cast<float>((tick / 7u) % static_cast<DWORD>(max_y)) : 0.0f;
  ball_offsetx = 3.0f;
  ball_offsety = 2.25f;

  // Create a frameless child window sized to the icon.
  // WS_EX_TRANSPARENT: paints after all sibling controls (visually on top)
  // and does not capture mouse input, so controls beneath remain interactive.
  BallWindow = CreateWindowExW(WS_EX_TRANSPARENT, czBallClass, nullptr, WS_CHILD | WS_VISIBLE,
                                static_cast<int>(ball_x), static_cast<int>(ball_y), ball_w,
                                ball_h, hMainWindow, nullptr, ball_hinst, nullptr);
  if (!BallWindow) {
    LOG(ERROR) << __FUNC__ << L"() failed to create ball window!";
    return;
  }

  // Start the move timer. NULL hwnd + non-NULL proc: timer fires
  // via the thread message loop without being tied to any window's lifetime.
  bounce_timer = SetTimer(nullptr, 0, kBallInterval(), BallTimerProc);
}
