#include "monitor_window.h"

#include <windowsx.h>

#include <deque>

#include "stress/cpu.h"
#include "strings.h"
#include "ui_utils.h"

static HFONT g_monitor_font = nullptr;

const WCHAR* szMonitorWindowClass = WPERF_MAIN_WNDCLASS;

// Handle to monitor window
HWND hMonitorWin = nullptr;
HWND hMonitorStatusBar = nullptr;

static HINSTANCE this_hinst = nullptr;

// Ring of CPU samples; newest sample is at the back.
// We store up to kHistoryMax entries and display the most recent
// <graph_width> of them right-aligned (oldest on the left, newest on the right).
static constexpr int kHistoryMax = 1024; // more than enough history for any window size
static std::deque<int> g_cpu_history;
static std::deque<int> g_ram_history;
static std::deque<int> g_comm_history;
static std::deque<int> g_io_history;

static UINT g_update_interval = kSpeedHigh;

// Layout constants
static inline constexpr int kMarginH   = 4;  // left/right margin outside the graph box
static inline constexpr int kMarginTop = 4;  // top margin above the label
static inline constexpr int kMarginBot = 4;  // bottom margin below the graph box
static inline constexpr int kLabelH    = 16; // height of the label above the graph box, also used for hit-testing

// Returns the HMENU for the "Update Speed" popup so we can radio-check it.
static HMENU GetSpeedMenu(HWND hWnd) {
  HMENU hBar  = GetMenu(hWnd);
  if (!hBar) {
    return nullptr;
  }
  HMENU hOpts = GetSubMenu(hBar, 0); // "Settings" is index 0
  if (!hOpts) {
    return nullptr;
  }
  return GetSubMenu(hOpts, 0); // "Update Speed" is index 0 within "Settings"
}

// Apply a new update interval: kill the old timer, start a new one, and
// update the radio-button checkmark on the speed submenu.
static void SetUpdateSpeed(HWND hWnd, UINT interval, UINT menu_id) {
  g_update_interval = interval;
  KillTimer(hWnd, kUpdateTimerId);
  SetTimer(hWnd, kUpdateTimerId, g_update_interval, nullptr);

  HMENU hSpd = GetSpeedMenu(hWnd);
  if (hSpd) {
    CheckMenuRadioItem(hSpd, IDM_SPEED_LOW, IDM_SPEED_HIGH, menu_id, MF_BYCOMMAND);
  }
}

unsigned int GetUpdateSpeed() {
  return g_update_interval;
}

bool OpenMonitorWindow(HWND hWnd) {
  bool success               = false;
  this_hinst = GetGlobalHinst();

  // Seed performance counters before the window opens.
  if (!IsPerfDataInitialized() || !this_hinst) {
    ErrorBox(hWnd, L"Monitor Window Error", L"Perf data not initialized!");
    return false;
  }

  // If the monitor window is already open, bring it to the foreground.
  if (hMonitorWin != nullptr) {
    LOG(INFO) << L"Restoring Sys Monitor window to foreground";
    return SetForegroundWindow(hMonitorWin);
  } else {
    LOG(INFO) << L"Opening Sys Monitor window";
  }

  WNDCLASSEXW wcex;
  wcex.cbSize        = sizeof(WNDCLASSEXW);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = MonitorWindowProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = this_hinst;
  wcex.hIcon         = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_WPERF));
  wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
  wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_WPERF);
  wcex.lpszClassName = szMonitorWindowClass;
  wcex.hIconSm       = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_WPERF));
  RegisterClassExW(&wcex);

  // Compute the outer window size that gives us the desired client area.
  DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
                | WS_MAXIMIZEBOX | WS_THICKFRAME;
  DWORD ex_style = 0;

  /* RECT adj = {0, 0, kDesiredClientW, kDesiredClientH};
  AdjustWindowRectEx(&adj, style, true, ex_style);
  int outer_w = adj.right  - adj.left;
  int outer_h = adj.bottom - adj.top; */

  // Put to right of main window
  int left, top;
  GetRightOfWindow(hMainWindow, &left, &top);

  hMonitorWin = CreateWindowExW(
      ex_style,
      szMonitorWindowClass,
      MON_TITLE,
      style,
      left, top, kDesiredClientW, kDesiredClientH,
      nullptr, nullptr, this_hinst, nullptr);

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
      // Initialize anything we need to
      InitMeters(hWnd);

      // Start the update timer at the default interval.
      SetTimer(hWnd, kUpdateTimerId, GetUpdateSpeed(), nullptr);

      // Radio check the default speed item.
      HMENU hSpd = GetSpeedMenu(hWnd);
      if (hSpd) {
        CheckMenuRadioItem(hSpd, IDM_SPEED_LOW, IDM_SPEED_HIGH,
                           IDM_SPEED_HIGH, MF_BYCOMMAND);
      }
    } break;
    // Periodic update
    case WM_TIMER:
      if (wParam == kUpdateTimerId) {
        UpdatePerfData();
        const PerfSnapshot snapshot = GetPerfSnapshot();
        PushSamples(snapshot.cpu_percent, static_cast<int>(snapshot.ram_percent),
                    static_cast<int>(snapshot.comm_percent), snapshot.io_percent);
        if (hMonitorStatusBar) {
          wchar_t sb_text[32];
          wsprintfW(sb_text, L" CPU: %d%%", GetPerfSnapshot().cpu_percent);
          SendMessageW(hMonitorStatusBar, SB_SETTEXT, 0,
                       reinterpret_cast<LPARAM>(sb_text));
        }
        // FALSE = do not erase background (we paint it fully in WM_PAINT).
        InvalidateRect(hWnd, nullptr, FALSE);
      }
      break;
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
      int dw = draw_area.right;
      int dh = draw_area.bottom;

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
    case WM_SIZE:
      HandleMonitorWindowResize(hWnd);
      break;
    case WM_GETMINMAXINFO: {
      LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = kMinOuterW;
      pMinMaxInfo->ptMinTrackSize.y = kMinOuterH;
      pMinMaxInfo->ptMaxTrackSize.x = MAXWIDTH;
      pMinMaxInfo->ptMaxTrackSize.y = MAXHEIGHT;
    } break;
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDM_CLOSE_MON:
          PostMessageW(hWnd, WM_CLOSE, 0, 0); // Will be picked up on next MonitorWindowProc loop
          break;
        case IDM_ABOUT_MON:
          ShowAboutDialog(hWnd);
          break;
        case IDM_SPEED_LOW:
          SetUpdateSpeed(hWnd, kSpeedLow, IDM_SPEED_LOW);
          break;
        case IDM_SPEED_MED:
          SetUpdateSpeed(hWnd, kSpeedMed, IDM_SPEED_MED);
          break;
        case IDM_SPEED_HIGH:
          SetUpdateSpeed(hWnd, kSpeedHigh, IDM_SPEED_HIGH);
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
        // -1/-1 means keyboard invocation, in that case centre in this window.
        if (screen_x == -1 && screen_y == -1) {
          RECT wr;
          GetWindowRect(hWnd, &wr);
          screen_x = (wr.left + wr.right)  / 2;
          screen_y = (wr.top  + wr.bottom) / 2;
        }
        TrackPopupMenu(hOpts, TPM_RIGHTBUTTON, screen_x, screen_y, 0, hWnd, nullptr);
      }
    } break;
    // Stop monitoring on shutdown, before CryoCalc close
    case WM_QUERYENDSESSION:
      KillTimer(hWnd, kUpdateTimerId);
      DestroyWindow(hWnd);
      break;
    // Left-click on the graph area drags the whole window,
    // mimicking Task Manager's mini-graph behaviour.
    case WM_LBUTTONDOWN: {
      POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      RECT cr;
      GetClientRect(hWnd, &cr);
      // Only initiate drag when the click is within the graph area (below the label).
      if (pt.y >= kMarginTop + kLabelH) {
        ClientToScreen(hWnd, &pt);
        ReleaseCapture();
        SendMessageW(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
      }
    } break;
    case WM_CLOSE:
      KillTimer(hWnd, kUpdateTimerId);
      if (!ResetFocus(hMainWindow, nullptr)) {
        LOG(ERROR) << L"ResetFocus failed!";
      }
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY:
      KillTimer(hWnd, kUpdateTimerId);
      CleanupMeters();
      // Do NOT call CleanupPerfData() here: PerfData is shared with the MonitorCPU
      // thread (hCPUBar) which may still be running after this window closes.
      // CleanupPerfData() should be called at application shutdown.
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

void InitMeters(HWND hWnd) {
  /* g_monitor_font = CreateFontW(-11, 0, 0, 0, FW_NORMAL,
                       FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH | FF_SWISS,
                       L"MS Shell Dlg"); */
  if (!g_monitor_font) {
    g_monitor_font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
  }
  // Create the status bar.
  hMonitorStatusBar = CreateWindowExW(
      0, STATUSCLASSNAME, nullptr,
      WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
      0, 0, 0, 0,
      hWnd, nullptr, this_hinst, nullptr);
  if (hMonitorStatusBar) {
    SendMessageW(hMonitorStatusBar, WM_SIZE, 0, 0);
    SendMessageW(hMonitorStatusBar, SB_SETTEXT, 0,
                 reinterpret_cast<LPARAM>(L" CPU: 0%"));
  }
}

void PushSamples(int cpu_percent, int ram_percent, int comm_percent, int io_percent) {
  g_cpu_history.push_back(cpu_percent);
  g_ram_history.push_back(ram_percent);
  g_comm_history.push_back(comm_percent);
  g_io_history.push_back(io_percent);
  while (static_cast<int>(g_cpu_history.size()) > kHistoryMax) {
    g_cpu_history.pop_front();
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
  int iw = inner.right - inner.left;
  int ih = inner.bottom - inner.top;
  bool make_grid = false;
  if (iw <= 0 || ih <= 0) {
    return;
  } else {
    make_grid = true;
  }

  // Select the correct history ring based on graph type.
  const std::deque<int>* history = nullptr;
  switch (type) {
    case RAM_TYPE:  history = &g_ram_history;  break;
    case COMM_TYPE: history = &g_comm_history; break;
    case IO_TYPE:   history = &g_io_history;   break;
    case CPU_TYPE:
    default:        history = &g_cpu_history;  break;
  }

  // How many history entries fit in the available pixel width
  int n_valid = static_cast<int>(history->size());
  if (n_valid > iw) {
    n_valid = iw;
  }

  // Data is right-aligned: most recent sample appears at the far right.
  int start_x      = iw - n_valid;
  int hist_offset  = static_cast<int>(history->size()) - n_valid;

  // Grid lines - 9 horizontal + 9 vertical, dividing the area into 10 equal sized cells
  if (make_grid) {
    HPEN grid_pen  = CreatePen(PS_SOLID, 1, RGB(0, 0, 128)); // Blue grid lines
    HPEN saved_pen = static_cast<HPEN>(SelectObject(hdc, grid_pen));
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
  }

  if (n_valid < 2) {
    return; // need at least 2 points for a line
  }

  // Build top-contour point array
  std::vector<POINT> pts;
  pts.reserve(static_cast<size_t>(n_valid));
  for (int i = 0; i < n_valid; i++) {
    int pct = (*history)[static_cast<size_t>(hist_offset + i)];
    int y   = inner.top + ih - 1 - (pct * (ih - 1) / 100);
    pts.push_back({inner.left + start_x + i, y});
  }

  // Filled area below the line (dark green)
  if (make_grid) {
    std::vector<POINT> poly;
    poly.reserve(pts.size() + 2);
    poly.insert(poly.end(), pts.begin(), pts.end());
    poly.push_back({pts.back().x,  inner.bottom - 1}); // bottom-right corner
    poly.push_back({pts.front().x, inner.bottom - 1}); // bottom-left corner

    HPEN null_pen     = static_cast<HPEN>(GetStockObject(NULL_PEN));
    HBRUSH fill_brush = CreateSolidBrush(RGB(0, 96, 0)); // Dark green
    HPEN saved_pen    = static_cast<HPEN>(SelectObject(hdc, null_pen));
    HBRUSH saved_br   = static_cast<HBRUSH>(SelectObject(hdc, fill_brush));
    Polygon(hdc, poly.data(), static_cast<int>(poly.size()));
    SelectObject(hdc, saved_pen);
    SelectObject(hdc, saved_br);
    DeleteObject(fill_brush);
  }

  // Bright green line along the top contour
  if (make_grid) {
    HPEN line_pen  = CreatePen(PS_SOLID, 1, RGB(0, 255, 0)); // Full bright green
    HPEN saved_pen = static_cast<HPEN>(SelectObject(hdc, line_pen));
    Polyline(hdc, pts.data(), static_cast<int>(pts.size()));
    SelectObject(hdc, saved_pen);
    DeleteObject(line_pen);
  }
}

void DrawMeter(HDC hdc, const RECT& area) {
  int cw = (area.right - area.left);
  int ch = (area.bottom - area.top);

  // Regular dialog gray color
  FillRect(hdc, &area, reinterpret_cast<HBRUSH>(static_cast<LONG_PTR>(COLOR_3DSHADOW)));

  // Label centered at the top
  RECT label_rect = {0, kMarginTop, cw, kMarginTop + kLabelH};
  SelectObject(hdc, g_monitor_font);
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
  // const std::wstring label = 
  DrawTextW(hdc, CPU_LABEL, -1, &label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

  // Graph box rect, fills most of the rest of monitor window
  RECT graph_rect = {kMarginH, kMarginTop + kLabelH, cw - kMarginH, ch - kMarginBot};

  // Sunken "3D" border, similar to SS_SUNKEN. BF_ADJUST shrinks graph_rect to the drawable inner area
  DrawEdge(hdc, &graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  // Fill background (do this before calling DrawGraph) TODO: CHANGE COLOR
  FillRect(hdc, &graph_rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

  // Scrolling line graph
  DrawGraph(hdc, graph_rect, CPU_TYPE);
}

void DrawMeters(HDC hdc, const RECT& area) {
  // Regular dialog gray color
  FillRect(hdc, &area, reinterpret_cast<HBRUSH>(static_cast<LONG_PTR>(COLOR_3DSHADOW)));

  // Layout constants for the 4 graphs
  const int halfwidth  = (area.right  - area.left) / 2;
  const int halfheight = (area.bottom - area.top) / 2;
  const int graph_width = halfwidth - kMarginH;
  const int graph_height = halfheight - kMarginBot;

  SelectObject(hdc, g_monitor_font); // Set font for this
  SetBkMode(hdc, TRANSPARENT); // Background opacity
  SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // Text color
  // Labels centered at the top
  RECT cpu_label_rect = {kMarginH, // x-coordinate of the upper-left corner
                         kMarginTop, // y-coordinate of the upper-left corner
                         graph_width, // x-coordinate of the lower-right corner
                         kMarginTop + kLabelH}; // y-coordinate of the lower-right corner
  RECT ram_label_rect = {graph_width + kMarginH, kMarginTop, graph_width * 2, kMarginTop + kLabelH};
  RECT comm_label_rect = {kMarginH, graph_height + kMarginTop, graph_width, kMarginTop + kLabelH + graph_height};
  RECT io_label_rect = {graph_width + kMarginH, graph_height + kMarginTop, graph_width * 2, kMarginTop + kLabelH + graph_height};

  DrawTextW(hdc, CPU_LABEL, -1, &cpu_label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above CPU Graph
  DrawTextW(hdc, RAM_LABEL, -1, &ram_label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above RAM Graph
  DrawTextW(hdc, COMM_LABEL, -1, &comm_label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above Commit Charge graph
  DrawTextW(hdc, IO_LABEL, -1, &io_label_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER); // Label above I/O graph

  // Graph box rects, one for each quadrant
  RECT cpu_graph_rect = {kMarginH, kMarginTop + kLabelH, graph_width, graph_height};
  RECT ram_graph_rect = {graph_width + kMarginH, kMarginTop + kLabelH, graph_width * 2, graph_height};
  RECT comm_graph_rect = {kMarginH, graph_height + kMarginTop + kLabelH, graph_width, graph_height * 2};
  RECT io_graph_rect = {graph_width + kMarginH, graph_height + kMarginTop + kLabelH, graph_width * 2, graph_height * 2};

  //DrawEdge(hdc, &cpu_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &cpu_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  //DrawEdge(hdc, &ram_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &ram_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  //DrawEdge(hdc, &comm_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
  DrawEdge(hdc, &comm_graph_rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  //DrawEdge(hdc, &io_label_rect, EDGE_ETCHED, BF_RECT | BF_ADJUST);
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
  if (g_monitor_font && g_monitor_font != GetStockObject(DEFAULT_GUI_FONT)) {
    DeleteObject(g_monitor_font);
  }
  g_monitor_font = nullptr;
  g_cpu_history.clear();
  g_ram_history.clear();
  g_comm_history.clear();
  g_io_history.clear();
}
