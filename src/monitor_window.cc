#include "monitor_window.h"

#include <deque>

#include "painting.h"
#include "resource.h"
#include "stress/stress.h"
#include "strings.h"
#include "ui_utils.h"

static HFONT g_monitor_font = nullptr;

const WCHAR* szMonitorWindowClass = WPERF_MAIN_WNDCLASS;

// Handle to monitor window
HWND hMonitorWin       = nullptr;
HWND hMonitorStatusBar = nullptr;

static HINSTANCE this_hinst = nullptr;

// Ring of CPU samples; newest sample is at the back.
// We store up to kHistoryMax entries and display the most recent
// <graph_width> of them right-aligned (oldest on the left, newest on the right).
static constexpr int kHistoryMax = 1024; // more than enough history for any window size
static std::deque<float> g_cpu_history;
static std::deque<float> g_kernel_history;
static std::deque<float> g_ram_history;
static std::deque<float> g_comm_history;
static std::deque<float> g_io_history;

static bool g_show_kernel = true; // Draw kernel-time overlay on CPU graph
static bool g_fill_lines  = true; // Fill area below lines with color

UINT g_update_interval = kSpeedHigh; // Shared; also read by controls.cc on init

// Layout constants
static constexpr int kMovePixH  = 2; // Horizontal pixels to move the line each sample;
static constexpr int kMarginH   = static_cast<int>(PADDING_X); // left/right margin outside the graph box
static constexpr int kMarginTop = static_cast<int>(PADDING_Y); // top margin above the label
static constexpr int kMarginBot = kMarginTop; // bottom margin below the graph box
static constexpr int kLabelH = 16; // height of the label above the graph box, also used for hit-testing

// "Update Speed" popup from the monitor window menu (Settings > index 0).
static HMENU GetMonitorSpeedMenu() {
  HMENU hBar  = GetMenu(hMonitorWin);
  HMENU hOpts = hBar ? GetSubMenu(hBar, 0) : nullptr;
  return hOpts ? GetSubMenu(hOpts, 0) : nullptr;
}

// "Update Speed" popup from the main CryoCalc window menu (Settings=index 2, submenu index 1).
static HMENU GetMainSpeedMenu() {
  HMENU hBar      = GetMenu(hMainWindow);
  HMENU hSettings = hBar ? GetSubMenu(hBar, 2) : nullptr;
  return hSettings ? GetSubMenu(hSettings, 1) : nullptr;
}

// Restart the single monitoring timer on the main window at the new rate and
// radio-check both menus. Safe to call from either window's message handler.
void SetUpdateSpeed(UINT interval, UINT menu_id) {
  g_update_interval = interval;
  KillTimer(hMainWindow, kUpdateTimerId);
  SetTimer(hMainWindow, kUpdateTimerId, g_update_interval, nullptr);

  HMENU hMainSpd = GetMainSpeedMenu();
  if (hMainSpd) {
    CheckMenuRadioItem(hMainSpd, IDM_SPEED_LOW, IDM_SPEED_HIGH, menu_id, MF_BYCOMMAND);
  }
  if (hMonitorWin) {
    HMENU hMonSpd = GetMonitorSpeedMenu();
    if (hMonSpd) {
      CheckMenuRadioItem(hMonSpd, IDM_SPEED_LOW, IDM_SPEED_HIGH, menu_id, MF_BYCOMMAND);
    }
  }
}

bool OpenMonitorWindow(HWND hWnd) {
  bool success = false;
  this_hinst   = GetGlobalHinst();

  // Seed performance counters before the window opens.
  if (!IsPerfDataInitialized() || !this_hinst) {
    ErrorBox(hWnd, L"Monitor Window Error", L"Perf data not initialized!");
    return false;
  }

  // If the monitor window is already open, bring it to the foreground.
  if (hMonitorWin != nullptr) {
    LOG(INFO) << L"Restoring System Monitor window to foreground";
    return SetForegroundWindow(hMonitorWin);
  } else {
    LOG(INFO) << L"Opening System Monitor window";
  }

  WNDCLASSEXW wcex;
  wcex.cbSize        = sizeof(WNDCLASSEXW);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = MonitorWindowProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = this_hinst;
  wcex.hIcon         = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_SYSMON));
  wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
  wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_WPERF);
  wcex.lpszClassName = szMonitorWindowClass;
  wcex.hIconSm       = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_WPERF));
  RegisterClassExW(&wcex);

  // Compute the outer window size that gives us the desired client area.
  DWORD style =
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
  DWORD ex_style = 0;

  /* RECT adj = {0, 0, kDesiredClientW, kDesiredClientH};
  AdjustWindowRectEx(&adj, style, true, ex_style);
  int outer_w = adj.right  - adj.left;
  int outer_h = adj.bottom - adj.top; */

  // Put to right of main window
  int left, top;
  GetRightOfWindow(hMainWindow, &left, &top);
  left = left - kDesiredClientW / 3u;

  hMonitorWin =
      CreateWindowExW(ex_style, szMonitorWindowClass, MON_TITLE, style, left, top, kDesiredClientW,
                      kDesiredClientH, nullptr, nullptr, this_hinst, nullptr);

  if (!hMonitorWin) {
    return false;
  }

  ShowWindow(hMonitorWin, SW_NORMAL);
  success = UpdateWindow(hMonitorWin);
  return success;
}

void HandleMonitorWindowResize(HWND hWnd) {
  if (!hWnd) {
    return;
  }
  if (hMonitorStatusBar) {
    // Resize the status bar, graph auto-sizes during paint!
    SendMessageW(hMonitorStatusBar, WM_SIZE, 0, 0);
  }
}

LRESULT CALLBACK MonitorWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE: {
      InitMeters(hWnd);
      InitMonMenuItems(hWnd);
      SetFontAllControls(hWnd, kMainFont);
    } break;
    // Painting
    case WM_ERASEBKGND:
      // Suppress default erase; WM_PAINT handles the full background.
      return 1;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      RECT cr;
      GetClientRect(hWnd, &cr);

      // Shrink the drawable area to exclude the status bar at the bottom.
      int sb_h = 0;
      if (hMonitorStatusBar) {
        RECT sbr;
        GetClientRect(hMonitorStatusBar, &sbr);
        sb_h = sbr.bottom;
      }
      RECT draw_area = {0, 0, cr.right, cr.bottom - sb_h};
      int dw         = draw_area.right;
      int dh         = draw_area.bottom;

      // Double-buffer to help eliminate flicker.
      HDC hmem         = CreateCompatibleDC(hdc);
      HBITMAP hbmp     = CreateCompatibleBitmap(hdc, dw, dh);
      HBITMAP hbmp_old = static_cast<HBITMAP>(SelectObject(hmem, hbmp));

      DrawMeters(hmem, draw_area);
      BitBlt(hdc, 0, 0, dw, dh, hmem, 0, 0, SRCCOPY);

      SelectObject(hmem, hbmp_old);
      DeleteObject(hbmp);
      DeleteDC(hmem);

      EndPaint(hWnd, &ps);
    } break;
    case WM_SIZE: {
      if (wParam != SIZE_MINIMIZED) {
        HandleMonitorWindowResize(hWnd);
      }
    } break;
    case WM_GETMINMAXINFO: {
      LPMINMAXINFO pMinMaxInfo      = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = MONWIN_MINWIDTH;
      pMinMaxInfo->ptMinTrackSize.y = MONWIN_MINHEIGHT;
      pMinMaxInfo->ptMaxTrackSize.x = MONWIN_MAXWIDTH;
      pMinMaxInfo->ptMaxTrackSize.y = MONWIN_MAXHEIGHT;
    } break;
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDM_SHOW_KERNEL: {
          ShowKernelLines(hWnd);
        } break;
        case IDM_FILL_LINES: {
          ShowFillLines(hWnd);
        } break;
        case IDM_CLOSE_MON:
          PostMessageW(hWnd, WM_CLOSE, 0, 0); // Will be picked up on next MonitorWindowProc loop
          break;
        case IDM_ABOUT_MON:
          ShowAboutDialog(hWnd);
          break;
        case IDM_SPEED_LOW:
          SetUpdateSpeed(kSpeedLow, IDM_SPEED_LOW);
          break;
        case IDM_SPEED_MED:
          SetUpdateSpeed(kSpeedMed, IDM_SPEED_MED);
          break;
        case IDM_SPEED_HIGH:
          SetUpdateSpeed(kSpeedHigh, IDM_SPEED_HIGH);
          break;
        default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
    } break;
    // Right-click context menu (mirrors the "Settings" popup)
    case WM_CONTEXTMENU: {
      HMENU hBar  = GetMenu(hWnd);
      HMENU hOpts = hBar ? GetSubMenu(hBar, 0) : nullptr;
      if (hOpts) {
        int screen_x = GET_X_LPARAM(lParam);
        int screen_y = GET_Y_LPARAM(lParam);
        // -1/-1 means keyboard invocation; show at current cursor position.
        if (screen_x == -1 && screen_y == -1) {
          POINT pt;
          GetCursorPos(&pt);
          screen_x = pt.x;
          screen_y = pt.y;
        }
        TrackPopupMenu(hOpts, TPM_RIGHTBUTTON, screen_x, screen_y, 0, hWnd, nullptr);
      }
    } break;
    case WM_QUERYENDSESSION:
      LOG(DEBUG) << L"Stopping monitoring, shutting down...";
      DestroyWindow(hWnd);
      break;
    // Left-click on the graph area drags the whole window,
    // mimicking Task Manager's mini-graph behaviour.
    case WM_LBUTTONDOWN: {
      POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      RECT cr;
      GetClientRect(hWnd, &cr);
      // Only initiate drag when the click is within the graph area (below the margin).
      if (pt.y >= kMarginTop) {
        ClientToScreen(hWnd, &pt);
        ReleaseCapture();
        SendMessageW(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
      }
    } break;
    case WM_CLOSE:
      if (!ResetFocus(hMainWindow, nullptr)) {
        LOG(ERROR) << L"ResetFocus failed!";
      }
      DestroyWindow(hWnd);
      LOG(DEBUG) << L"Closed System Monitor Window";
      break;
    case WM_DESTROY:
      CleanupMeters();
      // Do NOT call CleanupPerfData() here; it is owned by the main window
      // and cleaned up via StopMonitoring() at application shutdown.
      if (UnregisterClassW(szMonitorWindowClass, this_hinst)) {
        hMonitorWin = nullptr;
      }
      break;
    case WM_NCDESTROY:
      hMonitorWin = nullptr;
      break;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}

bool InitMonMenuItems(HWND hWnd) {
  if (!hWnd) {
    return false;
  }
  // hMonitorWin is not yet assigned at WM_CREATE time, so use hWnd directly.
  HMENU hMenu  = GetMenu(hWnd);
  HMENU hOpts = hMenu ? GetSubMenu(hMenu, 0) : nullptr;
  HMENU hSpd  = hOpts ? GetSubMenu(hOpts, 0) : nullptr;
  if (hSpd) {
    UINT cur_id = IDM_SPEED_HIGH;
    if (g_update_interval == kSpeedLow) {
      cur_id = IDM_SPEED_LOW;
    } else if (g_update_interval == kSpeedMed) {
      cur_id = IDM_SPEED_MED;
    } else if (g_update_interval == kSpeedHigh) {
      cur_id = IDM_SPEED_HIGH;
    }
    CheckMenuRadioItem(hSpd, IDM_SPEED_LOW, IDM_SPEED_HIGH, cur_id, MF_BYCOMMAND);
  }
  if (hMenu) {
    CheckMenuItem(hMenu, IDM_SHOW_KERNEL,
                  MF_BYCOMMAND | (g_show_kernel ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_FILL_LINES,
                  MF_BYCOMMAND | (g_fill_lines ? MF_CHECKED : MF_UNCHECKED));
  }
  return hMenu != nullptr;
}

void ShowKernelLines(HWND hWnd) {
  if (!hWnd) {
    return;
  }
  g_show_kernel = !g_show_kernel;
  HMENU hMenu   = GetMenu(hWnd);
  if (hMenu) {
    CheckMenuItem(hMenu, IDM_SHOW_KERNEL,
                  MF_BYCOMMAND | (g_show_kernel ? MF_CHECKED : MF_UNCHECKED));
  }
  InvalidateRect(hWnd, nullptr, false);
}

void ShowFillLines(HWND hWnd) {
  if (!hWnd) {
    return;
  }
  g_fill_lines = !g_fill_lines;
  HMENU hMenu  = GetMenu(hWnd);
  if (hMenu) {
    CheckMenuItem(hMenu, IDM_FILL_LINES,
                  MF_BYCOMMAND | (g_fill_lines ? MF_CHECKED : MF_UNCHECKED));
  }
  InvalidateRect(hWnd, nullptr, false);
}

void InitMeters(HWND hWnd) {
  if (!g_monitor_font) {
    g_monitor_font = GetFont(kMainFont);
  }
  // Create the status bar.
  hMonitorStatusBar =
      CreateWindowExW(0, STATUSCLASSNAME, nullptr, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0,
                      0, hWnd, nullptr, this_hinst, nullptr);
  if (hMonitorStatusBar) {
    SendMessageW(hMonitorStatusBar, WM_SIZE, 0, 0);
    static constexpr LPCWSTR init_status =
         L" CPU: 0%%   RAM: OMB/OMB   Commit: 0MB/0MB   I/O: O%% ";
    SendMessageW(hMonitorStatusBar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(init_status));
  }
  //AddTooltip(hWnd, hCPUGraph, this_hinst, L"");
}

void PushSamples(float cpu_percent,
                 float kernel_percent,
                 float ram_percent,
                 float comm_percent,
                 float io_percent) {
  // On the very first push after the window opens, seed all histories with 0
  // so the line starts from the bottom rather than jumping to the current
  // value (classic Task Manager behavior).
  if (g_cpu_history.empty()) {
    g_cpu_history.push_back(0.0f);
    g_kernel_history.push_back(0.0f);
    g_ram_history.push_back(0.0f);
    g_comm_history.push_back(0.0f);
    g_io_history.push_back(0.0f);
  }
  g_cpu_history.push_back(cpu_percent);
  g_kernel_history.push_back(kernel_percent);
  g_ram_history.push_back(ram_percent);
  g_comm_history.push_back(comm_percent);
  g_io_history.push_back(io_percent);
  while (static_cast<int>(g_cpu_history.size()) > kHistoryMax) {
    g_cpu_history.pop_front();
  }
  while (static_cast<int>(g_kernel_history.size()) > kHistoryMax) {
    g_kernel_history.pop_front();
  }
  while (static_cast<int>(g_ram_history.size()) > kHistoryMax) {
    g_ram_history.pop_front();
  }
  while (static_cast<int>(g_comm_history.size()) > kHistoryMax) {
    g_comm_history.pop_front();
  }
  while (static_cast<int>(g_io_history.size()) > kHistoryMax) {
    g_io_history.pop_front();
  }
}

// Renders the scrolling line graph into the already-clipped
// inner RECT, (background should already be filled by the caller).
// kMonType selects which performance counter to use, for the four different graphs
static void DrawGraph(HDC hdc, const RECT& inner, kMonType type) {
  int iw          = inner.right - inner.left;
  int ih          = inner.bottom - inner.top;
  bool make_graph = false;
  if (iw <= 0 || ih <= 0) {
    return;
  } else {
    make_graph = true;
  }

  // Select the correct history ring based on graph type.
  const std::deque<float>* history = nullptr;
  static const HPEN null_pen = static_cast<HPEN>(GetStockObject(NULL_PEN));
  static const HBRUSH null_brush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
  HBRUSH fill_brush = null_brush;
  HPEN line_pen = null_pen;
  switch (type) {
    case CPU_TYPE:
      line_pen = CreatePen(PS_SOLID, 1, RGB_GREEN);
      fill_brush = CreateSolidBrush(RGB_DARKGREEN);
      history = &g_cpu_history;
      break;
    case RAM_TYPE:
      line_pen = CreatePen(PS_SOLID, 1, RGB_YELLOW);
      fill_brush = CreateSolidBrush(RGB_DARKYELLOW);
      history = &g_ram_history;
      break;
    case COMM_TYPE:
      line_pen = CreatePen(PS_SOLID, 1, RGB_CYAN);
      fill_brush = CreateSolidBrush(RGB_DARKCYAN);
      history = &g_comm_history;
      break;
    case IO_TYPE:
      line_pen = CreatePen(PS_SOLID, 1, RGB_MAGENTA);
      fill_brush = CreateSolidBrush(RGB_DARKMAGENTA);
      history = &g_io_history;
      break;
    default:
      line_pen = null_pen;
      fill_brush = null_brush;
      LOG(FATAL) << L"Unhandled kMonType type!";
      break;
  }

  // How many history entries fit in the available pixel width
  int n_valid = static_cast<int>(history->size());
  if (n_valid > (iw / kMovePixH)) {
    n_valid = (iw / kMovePixH);
  }

  // Data is right-aligned: most recent sample appears at the far right.
  int start_x     = iw - (n_valid * kMovePixH);
  int hist_offset = static_cast<int>(history->size()) - n_valid;

  if (n_valid < 2) {
    make_graph = false; // need at least 2 points for a line
  }

  // Grid lines: 9 x + 9 y, dividing the area into 10 equal sized cells, always draw this
  const HPEN grid_pen = CreatePen(PS_SOLID, 1, RGB_BLUEISH); // Dark Blue grid lines
  HPEN saved_pen      = static_cast<HPEN>(SelectObject(hdc, grid_pen));
  for (int i = 1; i <= 9; i++) {
    // Horizontal lines evenly spaced in Y
    int gy = inner.top + (ih * i) / 10;
    MoveToEx(hdc, inner.left, gy, nullptr);
    LineTo(hdc, inner.right, gy);
    // Vertical lines evenly spaced in X
    int gx = inner.left + (iw * i) / 10;
    MoveToEx(hdc, gx, inner.top, nullptr);
    LineTo(hdc, gx, inner.bottom);
  }
  SelectObject(hdc, saved_pen);
  DeleteObject(grid_pen);

  if (!make_graph) {
    return; // Don't do anything if make_graph is false for whatever reason
  } else {
    // Build line top-contour point array
    std::vector<POINT> pts;
    pts.reserve(static_cast<size_t>(n_valid));
    for (int i = 0; i < n_valid; i++) {
      float pct = (*history)[static_cast<size_t>(hist_offset + i)];
      int y     = inner.top + ih - 1 - static_cast<int>(pct * static_cast<float>(ih - 1) / 100.0f);
      pts.push_back({inner.left + start_x + (i * kMovePixH), y});
    }

    // Compute lower filled area below the line
    std::vector<POINT> poly;
    poly.reserve(pts.size() + 2);
    poly.insert(poly.end(), pts.begin(), pts.end());
    poly.push_back({pts.back().x, inner.bottom - 1});  // bottom-right corner
    poly.push_back({pts.front().x, inner.bottom - 1}); // bottom-left corner

    // Build kernel line top-contour point array (when enabled)
    std::vector<POINT> kpts;
    std::vector<POINT> kpoly;
    if (type == CPU_TYPE && g_show_kernel && n_valid >= 2) {
      kpts.reserve(static_cast<size_t>(n_valid));
      const int k_offset = static_cast<int>(g_kernel_history.size()) - n_valid;
      for (int i = 0; i < n_valid; i++) {
        float pct = g_kernel_history[static_cast<size_t>(k_offset + i)];
        int y = inner.top + ih - 1 - static_cast<int>(pct * static_cast<float>(ih - 1) / 100.0f);
        kpts.push_back({inner.left + start_x + (i * kMovePixH), y});
      }
      kpoly.reserve(kpts.size() + 2);
      kpoly.insert(kpoly.end(), kpts.begin(), kpts.end());
      kpoly.push_back({kpts.back().x, inner.bottom - 1});
      kpoly.push_back({kpts.front().x, inner.bottom - 1});
    }

    // Draw fills first, then lines on top (painter's algorithm)
    if (g_fill_lines) {
      // Fill under line with darkened color of line
      if (!poly.empty()) {
        HPEN saved_pen    = static_cast<HPEN>(SelectObject(hdc, null_pen));
        HBRUSH saved_br   = static_cast<HBRUSH>(SelectObject(hdc, fill_brush));
        Polygon(hdc, poly.data(), static_cast<int>(poly.size()));
        SelectObject(hdc, saved_pen);
        SelectObject(hdc, saved_br);
        DeleteObject(fill_brush);
      }

      if (!kpoly.empty()) {
        // Fill under kernel line with dark red
        HBRUSH kfill_brush = CreateSolidBrush(RGB_DARKRED); // Dark red
        HPEN saved_pen     = static_cast<HPEN>(SelectObject(hdc, null_pen));
        HBRUSH saved_br    = static_cast<HBRUSH>(SelectObject(hdc, kfill_brush));
        Polygon(hdc, kpoly.data(), static_cast<int>(kpoly.size()));
        SelectObject(hdc, saved_pen);
        SelectObject(hdc, saved_br);
        DeleteObject(kfill_brush);
      }
    }

    // Bright line along the top contour
    saved_pen     = static_cast<HPEN>(SelectObject(hdc, line_pen));
    Polyline(hdc, pts.data(), static_cast<int>(pts.size()));
    SelectObject(hdc, saved_pen);
    DeleteObject(line_pen);

    if (!kpts.empty()) {
      // Bright red line along the top kernel contour
      HPEN kern_pen  = CreatePen(PS_SOLID, 1, RGB_RED);
      HPEN saved_pen = static_cast<HPEN>(SelectObject(hdc, kern_pen));
      Polyline(hdc, kpts.data(), static_cast<int>(kpts.size()));
      SelectObject(hdc, saved_pen);
      DeleteObject(kern_pen);
    }
  }
}

void DrawMeter(HDC hdc, const RECT& area) {
  int cw = (area.right - area.left);
  int ch = (area.bottom - area.top);

  // Regular dialog gray color
  FillRect(hdc, &area, reinterpret_cast<HBRUSH>(static_cast<LONG_PTR>(COLOR_3DFACE + 1)));

  // Label centered at the top
  RECT label_rect = {0, kMarginTop, cw, kMarginTop + kLabelH};
  SelectObject(hdc, g_monitor_font);
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
  // const std::wstring label =
  DrawTextW(hdc, CPU_LABEL, -1, &label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

  // Graph box rect, fills most of the rest of monitor window
  RECT graph_rect = {kMarginH, kMarginTop + kLabelH, cw - kMarginH, ch - kMarginBot};

  // Sunken "3D" border, similar to SS_SUNKEN. BF_ADJUST shrinks graph_rect to the drawable inner
  // area
  DrawEdge(hdc, &graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // Fill background (do this before calling DrawGraph) TODO: CHANGE COLOR
  FillRect(hdc, &graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

  // Scrolling line graph
  DrawGraph(hdc, graph_rect, CPU_TYPE);
}

void DrawMeters(HDC hdc, const RECT& area) {
  // Regular dialog gray color
  FillRect(hdc, &area, reinterpret_cast<HBRUSH>(static_cast<LONG_PTR>(COLOR_3DFACE + 1)));

  // Layout constants for the 4 graphs
  const int halfwidth    = (area.right - area.left) / 2;
  const int halfheight   = (area.bottom - area.top) / 2;
  const int graph_width  = halfwidth - kMarginH;
  const int graph_height = halfheight - kMarginBot;

  SelectObject(hdc, g_monitor_font);                // Set font for this
  SetBkMode(hdc, TRANSPARENT);                      // Background opacity
  SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // Text color
  // Labels centered at the top
  RECT cpu_label_rect = {kMarginH,              // x-coordinate of the upper-left corner
                         kMarginTop,            // y-coordinate of the upper-left corner
                         graph_width,           // x-coordinate of the lower-right corner
                         kMarginTop + kLabelH}; // y-coordinate of the lower-right corner
  RECT ram_label_rect = {graph_width + kMarginH, kMarginTop, graph_width * 2, kMarginTop + kLabelH};
  RECT comm_label_rect = {kMarginH, graph_height + kMarginTop, graph_width,
                          kMarginTop + kLabelH + graph_height};
  RECT io_label_rect   = {graph_width + kMarginH, graph_height + kMarginTop, graph_width * 2,
                          kMarginTop + kLabelH + graph_height};

  DrawTextW(hdc, CPU_LABEL, -1, &cpu_label_rect,
            DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above CPU Graph
  DrawTextW(hdc, RAM_LABEL, -1, &ram_label_rect,
            DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above RAM Graph
  DrawTextW(hdc, COMM_LABEL, -1, &comm_label_rect,
            DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above Commit Charge graph
  DrawTextW(hdc, IO_LABEL, -1, &io_label_rect,
            DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above I/O graph

  // Graph box rects, one for each quadrant
  RECT cpu_graph_rect  = {kMarginH, kMarginTop + kLabelH, graph_width, graph_height};
  RECT ram_graph_rect  = {graph_width + kMarginH, kMarginTop + kLabelH, graph_width * 2,
                          graph_height};
  RECT comm_graph_rect = {kMarginH, graph_height + kMarginTop + kLabelH, graph_width,
                          graph_height * 2};
  RECT io_graph_rect   = {graph_width + kMarginH, graph_height + kMarginTop + kLabelH,
                          graph_width * 2, graph_height * 2};

  // DrawEdge(hdc, &cpu_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &cpu_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // DrawEdge(hdc, &ram_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &ram_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // DrawEdge(hdc, &comm_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &comm_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // DrawEdge(hdc, &io_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &io_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // Fill backgrounds (do this before calling DrawGraph) TODO: CHANGE COLOR
  FillRect(hdc, &cpu_graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
  FillRect(hdc, &ram_graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
  FillRect(hdc, &comm_graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
  FillRect(hdc, &io_graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

  // Scrolling line graphs
  DrawGraph(hdc, cpu_graph_rect, CPU_TYPE);
  DrawGraph(hdc, ram_graph_rect, RAM_TYPE);
  DrawGraph(hdc, comm_graph_rect, COMM_TYPE);
  DrawGraph(hdc, io_graph_rect, IO_TYPE);
}

// TODO Rename
void CleanupMeters() {
  if (g_monitor_font) {
    DeleteObject(g_monitor_font);
  }
  g_monitor_font = nullptr;
  g_cpu_history.clear();
  g_kernel_history.clear();
  g_ram_history.clear();
  g_comm_history.clear();
  g_io_history.clear();
}
