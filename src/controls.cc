#include "controls.h"

// Static labels
static HWND hInputLabel;  
static HWND hCelsiusLabel;  
static HWND hKelvinLabel;
static HWND hFahrenheitLabel;
static HWND hRankineLabel;
static HWND hFrameLabel;
static HWND hPrecisionLabel;

// Controls/buttons forward decl
HWND hInputEdit;
HWND hTempSelectEdit;
HWND hPrecisionEdit;
HWND hCelsiusEdit;
HWND hKelvinEdit;
HWND hFahrenheitEdit;
HWND hRankineEdit;
HWND hConvButton;
HWND hAboutButton;
HWND hClearButton;
HWND hStatusBar;

// For CPU Stressor
HWND hThreadsEdit;
HWND hStartStresButton;
HWND hStopStresButton;

// Temp select titles
static const wchar_t* kBlank = L"";
static const std::wstring kTempC = L"C";
static const std::wstring kTempK = L"K";
static const std::wstring kTempF = L"F";
static const std::wstring kTempR = L"R";
static const std::wstring kDummyScale = L"U";

// Button titles
static const std::wstring CONV_BUTTON  = L"Convert";
static const std::wstring CLEAR_BUTTON = L"Clear";
static const std::wstring START_BUTTON = L"Start";
static const std::wstring STOP_BUTTON  = L"Stop";
static const std::wstring ABOUT_BUTTON = L"About";

bool _about_handled = false;

Scale parseScale(const std::wstring& wscale) {
  if (wscale == kTempC) return kScaleCelsius;
  if (wscale == kTempK) return kScaleKelvin;
  if (wscale == kTempF) return kScaleFahrenheit;
  if (wscale == kTempR) return kScaleRankine;
  if (wscale == kDummyScale) {
    std::wcout << L"Using dummy unknown scale \"" << kDummyScale << L"\" for testing" << std::endl;
    return kScaleUnknown;
  }
  throw std::invalid_argument("Unknown scale!");
}

bool AboutButtonClicked(HWND hWnd) {
  if (ShowAboutDialog(hWnd)) {
    return true;
  } else {
    std::wcerr << L"About Button failed" << std::endl;
    return false;
  }
}

bool OnStartButtonClick(HWND hWnd) {
  return HandleConvert(hWnd);
}

bool HandleConvert(HWND hWnd) {
  bool success = false;
  static std::wstring kCelsius, kKelvin, kFahrenheit, kRankine;
  long double convCelsius, convKelvin, convFahrenheit, convRankine;
  long double input = 0.0L;
  // Get the length of the text in the edit control
  DWORD dwInputSize = GetWindowTextLength(hInputEdit);
  DWORD dwScaleSize = GetWindowTextLength(hTempSelectEdit);
  DWORD dwPrecisionSize = GetWindowTextLength(hPrecisionEdit);
  const bool is_empty = (BOOL)(dwInputSize == 0);
  bool is_invalid = is_empty; // Initial value is is_empty, since that's invalid too
  const bool bad_temp_scale = (BOOL)(dwScaleSize == 0);
  if (is_empty || bad_temp_scale) {
    if (bad_temp_scale) {
      MessageBoxW(hWnd, L"Invalid Temp scale!", L"Error!", MB_OK | MB_ICONERROR);
    }
    if (is_empty) {
      MessageBoxW(hWnd, L"No text entered!", L"Empty Input", MB_OK | MB_ICONWARNING);
    }
    success = false;
  } else {
    std::wostringstream wostr;
    // Create a buffer to store the text
    wchar_t* input_buff = new wchar_t[dwInputSize + 1]; // Allocate memory for the text
    wchar_t* scale_buff = new wchar_t[dwScaleSize + 1];
    wchar_t* preci_buff = new wchar_t[dwPrecisionSize + 1];

    // Get the text from the edit control
    GetWindowTextW(hInputEdit, input_buff, dwInputSize + 1);
    // Check that it isn't invalid input, like text characters
    if (!IsValidNumericInput(input_buff)) {
      is_invalid = true; // Bad
    }

    if (is_invalid) {
       MessageBoxW(hWnd, L"Invalid Input!", L"Error!", MB_OK | MB_ICONWARNING);
       success = false;
       ClearInput(hWnd);
       return success; // Fail on invalid input.
    } else {
      input = ConvertInputToLD(input_buff);
      std::wcout << L"Input = " << std::setprecision(MAX_PRECISION) << input << std::endl;
    }

    GetWindowTextW(hTempSelectEdit, scale_buff, dwScaleSize + 1);
    GetWindowTextW(hPrecisionEdit, preci_buff, dwPrecisionSize + 1);

    std::wstring scale(scale_buff);
    std::wstring preci(preci_buff);

    unsigned int precision_ = std::stoi(preci);
    if (precision_ >= MIN_PRECISION && precision_ <= MAX_PRECISION) {
      std::wcout << L"Precision = " << precision_ << std::endl;
    } else {
      std::wcerr << L"Precision out of range " << MIN_PRECISION 
                 << L" - " << MAX_PRECISION << L" Setting precision to max: " 
                 << MAX_PRECISION << std::endl;
      precision_ = MAX_PRECISION;
    }
    SetCryoCalcPrecision(precision_);

    Scale selected_scale = parseScale(scale);
    assert(((selected_scale >= kScaleCelsius && selected_scale < kMaxScale) || (selected_scale == kScaleUnknown)) && "Scale out of range");
    std::wstring kChosenScale;
    switch (selected_scale) {
      case kScaleCelsius: {
        convCelsius = input;
        convKelvin = kelvin::fromCelsius(input);
        convFahrenheit = fahrenheit::fromCelsius(input);
        convRankine = rankine::fromCelsius(input);
        kChosenScale = L"Celsius";
      } break;
      case kScaleKelvin:
        convCelsius = celsius::fromKelvin(input);
        convKelvin = input;
        convFahrenheit = fahrenheit::fromKelvin(input);
        convRankine = rankine::fromKelvin(input);
        kChosenScale = L"Kelvin";
        break;
      case kScaleFahrenheit:
        convCelsius = celsius::fromFahrenheit(input);
        convKelvin = kelvin::fromFahrenheit(input);
        convFahrenheit = input;
        convRankine = rankine::fromFahrenheit(input);
        kChosenScale = L"Fahrenheit";
        break;
      case kScaleRankine:
        convCelsius = celsius::fromRankine(input);
        convKelvin = kelvin::fromRankine(input);
        convFahrenheit = fahrenheit::fromRankine(input);
        convRankine = input;
        kChosenScale = L"Rankine";
        break;
      default:
        convCelsius = static_cast<long double>(INT_MAX);
        convKelvin = static_cast<long double>(INT_MAX);
        convFahrenheit = static_cast<long double>(INT_MAX);
        convRankine = static_cast<long double>(INT_MAX);
        kChosenScale = L"Unknown";
        MessageBoxW(hWnd, L"Temp scale not handled", L"Error", MB_OK | MB_ICONERROR);
    }
    std::wcout << L"Scale = " << kChosenScale << std::endl;
    const unsigned int precision = GetCryoCalcPrecision();
    wostr << std::fixed << std::setprecision(precision) << convCelsius;
    kCelsius = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(precision) << convKelvin;
    kKelvin = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(precision) << convFahrenheit;
    kFahrenheit = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(precision) << convRankine;
    kRankine = wostr.str();
    wostr.str(L"");
    wostr.clear();
    success = true;
  }
  if (success) {
    std::wcout << L"kCelsius = " << kCelsius << std::endl;
    std::wcout << L"kKelvin = " << kKelvin << std::endl;
    std::wcout << L"kFahrenheit = " << kFahrenheit << std::endl;
    std::wcout << L"kRankine = " << kRankine << std::endl;
    SetWindowTextW(hCelsiusEdit, kCelsius.c_str());
    SetWindowTextW(hKelvinEdit, kKelvin.c_str());
    SetWindowTextW(hFahrenheitEdit, kFahrenheit.c_str());
    SetWindowTextW(hRankineEdit, kRankine.c_str());
  }
  const unsigned int found_prec = GetCryoCalcPrecision();
  std::wcout << L"found_prec = " << found_prec << std::endl;
  return success;
}

void InitControls(HWND hWnd, HINSTANCE hInst) {
  SetClientRects(hWnd, hInst);
  const int kTempEditLeft = GetXOffset(STATIC_LEFT, LABEL_WIDTH + INTRA_PADDING, 1.0f);
  // Create staic box outline frame for all controls
  int padding = PADDING_X * 2; // Padding on left and right
  const int x_padding = -(PADDING_X + padding); // Padding already applied to left side + padding
  const int kFrameWidth = GetXOffset(CW_MAINWIDTH, x_padding, 1.0f);
  const unsigned int kLabelYPad = CW_STATICLABEL_HEIGHT + INTRA_PADDING; // Static label height plus 3 pixels between items.
  const unsigned int kEditYPad = CW_EDITCONTROL_HEIGHT + INTRA_PADDING; // Static label height plus 3 pixels between items.
  const unsigned int kFrameBottom = STATIC_TOP + CW_EDITCONTROL_HEIGHT + (kEditYPad * 4);
  unsigned int kButtonColLeft = PADDING_X + 1;
  const unsigned int kButtonCol2Left = PADDING_X + (BUTTON_WIDTH * 2) + PADDING_X;
  const unsigned int kButtonRowTop = kFrameBottom + (PADDING_Y * 2);
  const unsigned int kButtonRow2Top = kButtonRowTop + BUTTON_HEIGHT + PADDING_Y;
  const int kStopButtonRight = CW_MAINWIDTH - BUTTON_WIDTH - (STATIC_RIGHT + 1);
  hFrameLabel = CreateWindowExW(
      0, WC_STATIC, L"Frame",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_ETCHEDFRAME,
      PADDING_X,
      PADDING_Y,
      kFrameWidth,
      kFrameBottom,
      hWnd, nullptr, hInst, nullptr
  );
  // Static labels
  hInputLabel = CreateWindowExW(
      0,                      // Optional window styles
      WC_STATIC,              // Predefined class: Static
      L"Input:",              // Text
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN, // Styles
      STATIC_LEFT,            // x position
      STATIC_TOP,             // y position
      LABEL_WIDTH,            // Width
      CW_STATICLABEL_HEIGHT,  // Height
      hWnd,                   // Parent window
      (HMENU)IDC_LABEL_INPUT, // No menu, but control ID
      hInst,                 // This window instance
      nullptr                 // Additional app data
  );
  hCelsiusLabel = CreateWindowExW(
      0, WC_STATIC, L"Celsius:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      STATIC_TOP + kLabelYPad,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_C, hInst, nullptr
  );
  hKelvinLabel = CreateWindowExW(
      0, WC_STATIC, L"Kelvin:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * 2),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_K, hInst, nullptr
  );
  hFahrenheitLabel = CreateWindowExW(
      0, WC_STATIC, L"Fahrenheit:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * INTRA_PADDING),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, hInst, nullptr
  );
  hRankineLabel = CreateWindowExW(
      0, WC_STATIC, L"Rankine:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * 4),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, hInst, nullptr
  );
  hPrecisionLabel = CreateWindowExW(
      0, WC_STATIC, L"Precision:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      kButtonColLeft,
      kButtonRow2Top,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_PREC, hInst, nullptr
  );

  // Create the Edit controls
  // Temperature input
  hInputEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, L"77",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_INPUT, hInst, nullptr
  );
  // Temperature scale Combobox
  hTempSelectEdit = CreateWindowExW(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kTempEditLeft + EDIT_WIDTH + INTRA_PADDING,
      STATIC_TOP,
      COMBO_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_SCALE, hInst, nullptr
  );

  hCelsiusEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_CELSIUS, hInst, nullptr
  );
  hKelvinEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * 2),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_KELVIN, hInst, nullptr
  );
  hFahrenheitEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * INTRA_PADDING),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, hInst, nullptr
  );
  hRankineEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * 4),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, hInst, nullptr
  );

  // Create the "Convert" Button control
  hConvButton = CreateWindowExW(
      0, WC_BUTTON, CONV_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonColLeft,
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CONVERT_BUTTON, hInst, nullptr
  );
  // Create the "Precision" combobox
  hPrecisionEdit = CreateWindowExW(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonColLeft + LABEL_WIDTH + INTRA_PADDING,
      kButtonRow2Top,
      COMBO_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_PRECISION, hInst, nullptr
  );
  // Create the "Clear" Button control
  hClearButton = CreateWindowExW(
      0, WC_BUTTON, CLEAR_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol2Left, // Move about button manually to the right
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CLEAR_BUTTON, hInst, nullptr
  );
  // Create the "About" Button control
  hAboutButton = CreateWindowExW(
      0, WC_BUTTON, ABOUT_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol2Left,
      kButtonRow2Top,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_ABOUT_BUTTON, hInst, nullptr
  );
  // Lastly, create the status bar
  hStatusBar = CreateWindowExW(
      0, STATUSCLASSNAME, nullptr,
      WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
      0,
      0,
      0,
      0,
      hWnd, nullptr, hInst, nullptr
  );

  // Create the "Start" CPU Stress Button control
  hStartStresButton = CreateWindowExW(
      0, WC_BUTTON, START_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol2Left + BUTTON_WIDTH + PADDING_X, // Move about button manually to the right
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_START_BUTTON, hInst, nullptr
  );
  // Create the "Stop" CPU Stress Button control
  hStopStresButton = CreateWindowExW(
      0, WC_BUTTON, STOP_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kStopButtonRight, // Move about button manually to the right
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_STOP_BUTTON, hInst, nullptr
  );


  // Set temperature selection options in combobox
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempC.c_str()); // Celsius
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempK.c_str()); // Kelvin
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempF.c_str()); // Fahrenheit
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempR.c_str()); // Rankine
  //SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kDummyScale.c_str()); // Unknown
  // Precision combobox options
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"0");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"1");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"2");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"3");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"4");
  // Set default selections
  SendMessageW(hTempSelectEdit, CB_SETCURSEL, 0, 0L);
  SendMessageW(hPrecisionEdit, CB_SETCURSEL, static_cast<int>(CRYOCALC_PRECISION), 0);
  InitStatusBar(hWnd, hInst);
}

void InitStatusBar(HWND hWnd, HINSTANCE hInst) {
  const LPCWSTR kInitVersionText = GetVersionWstring().c_str();
  const int kStatusSplit = GetXOffset(CW_MAINWIDTH, -LABEL_WIDTH, 1.0f);
  const int kStatusParts[2] = { kStatusSplit, -1 }; // -1 = extend to right edge
  if (hStatusBar) {
    SendMessageW(hStatusBar, SB_SETPARTS, 2, (LPARAM)kStatusParts);
    SendMessageW(hStatusBar, SB_SETTEXT, 0, (LPARAM)kInitVersionText);
    SendMessageW(hStatusBar, SB_SETTEXT, 1, (LPARAM)L"Status");
  } else {
    std::wcerr << __FUNC__ << L"() failed: hStatusBar not initialized" << std::endl;
  }
}

void SetClientRects(HWND hWnd, HINSTANCE hInst) {
  // Structure to store painting info for hbrush handle
  PAINTSTRUCT paintStruct;
  // Declare rect to use for all future window layout
  RECT kWinRect;

  // Begin painting the contents of the window
  // HDC is a device context handle for GDI
  HDC hdc = BeginPaint(hWnd, &paintStruct);
  // Start logging as we are painting
  if (hdc) {
    // Get context of entire window
    const HDC winDC = GetWindowDC(hWnd);
    // Get rect size of window including titlebar
    GetClientRect(hWnd, &kWinRect);
    // Set color of text
    SetTextColor(winDC, COLOR_WINDOWTEXT);
    // Set window backgroun painting behavior
    SetBkMode(winDC, TRANSPARENT);
  } else {
    std::wcout << __FUNC__ << L"() Failed!" << std::endl;
    return;
  }
}

void ClearInput(HWND hWnd) {
  SetWindowTextW(hInputEdit, kBlank);
  std::wcout << L"Cleared input" << std::endl;
}

void ClearControls(HWND hWnd) {
  SetWindowTextW(hCelsiusEdit, kBlank);
  SetWindowTextW(hKelvinEdit, kBlank);
  SetWindowTextW(hFahrenheitEdit, kBlank);
  SetWindowTextW(hRankineEdit, kBlank);
  std::wcout << L"Cleared controls" << std::endl;
}

bool InputEntered(HWND hWnd) {
  return true;
}

void SetAboutHandled(bool handled) {
  _about_handled = handled;
}

bool GetAboutHandledState() {
  return _about_handled;
}

bool ShowAboutDialog(HWND hWnd) {
  const HINSTANCE gHinst = GetGlobalHinst();
  // Show "About" dialog box
  PlaySoundW(L"SystemNotification", nullptr, SND_ASYNC);
  DialogBoxW(gHinst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
  bool handled_dialog = GetAboutHandledState();
  if (handled_dialog) {
    std::wcout << L"Showed about dialog." << std::endl;
    return true;
  } else {
    std::wcerr << L"About dialog failed!" << std::endl;
    return false;
  }
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);

  bool AboutHandled = false; // Stores status of whether dialog has been handled user-wise.
  switch (message) {
    case WM_INITDIALOG:
      // Showed the dialog
      AboutHandled = true;
      SetAboutHandled(AboutHandled);
      break;
    case WM_COMMAND:
      // Exit the dialog
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        if (EndDialog(hDlg, LOWORD(wParam))) {
          AboutHandled = true;
          SetAboutHandled(AboutHandled);
          return (INT_PTR)AboutHandled;
        } else {
          AboutHandled = false;
          SetAboutHandled(AboutHandled);
          return (INT_PTR)AboutHandled;
        }
      } break;
    default:
      SetAboutHandled(true);
      break;
  }

  // About dialog failed
  return (INT_PTR)AboutHandled;
}
