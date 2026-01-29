#include "controls.h"

// Static labels
static HWND hInputLabel;  
static HWND hCelsiusLabel;  
static HWND hKelvinLabel;
static HWND hFahrenheitLabel;
static HWND hRankineLabel;
static HWND hFrameOutline;
static HWND hPrecisionLabel;
static HWND hThreadsLabel;
static HWND hCacheSizeLabel;

// Controls/buttons forward decl
HWND hInputEdit;
HWND hTempSelectEdit;
HWND hPrecisionCombo;
HWND hCelsiusEdit;
HWND hKelvinEdit;
HWND hFahrenheitEdit;
HWND hRankineEdit;
HWND hConvButton;
HWND hClearButton;
HWND hAboutButton;
HWND hOsInfoButton;
HWND hStatusBar;

// For CPU Stressor
HWND hThreadsEdit;
HWND hProgressBar;
HWND hSSE2Checkbox;
HWND hCacheSizeCombo;
HWND hStartStresButton;
HWND hStopStresButton;

// Temp select titles
static const wchar_t* kBlank = L"";
static const std::wstring kTempC = L"C";
static const std::wstring kTempK = L"K";
static const std::wstring kTempF = L"F";
static const std::wstring kTempR = L"R";
static const std::wstring kDummyScale = L"U";

bool _about_handled = false;

unsigned int num_threads_ = 0;

// Declare rects to use for all future window layout
RECT kMainClientRect;
RECT kMainWinRect;

Scale parseScale(const std::wstring& wscale) {
  if (wscale == kTempC) return kScaleCelsius;
  if (wscale == kTempK) return kScaleKelvin;
  if (wscale == kTempF) return kScaleFahrenheit;
  if (wscale == kTempR) return kScaleRankine;
  if (wscale == kDummyScale) {
    LOG(DEBUG) << L"Using dummy unknown scale \"" << kDummyScale << L"\" for testing";
    return kScaleUnknown;
  }
  throw std::invalid_argument("Unknown scale!");
}

bool AboutButtonClicked(HWND hWnd) {
  if (!ShowAboutDialog(hWnd)) {
    LOG(ERROR) << L"About Button failed";
    return false;
  }
  return true;
}

bool OnConvertButtonClick(HWND hWnd) {
  return HandleConvert(hWnd);
}

bool HandleConvert(HWND hWnd) {
  bool success = false;
  std::wstring kCelsius, kKelvin, kFahrenheit, kRankine;
  long double convCelsius, convKelvin, convFahrenheit, convRankine;
  long double input = 0.0L;
  // Get the length of the text in the edit control
  DWORD dwInputSize = GetWindowTextLength(hInputEdit);
  DWORD dwScaleSize = GetWindowTextLength(hTempSelectEdit);
  DWORD dwPrecisionSize = GetWindowTextLength(hPrecisionCombo);
  const bool is_empty = (BOOL)(dwInputSize == 0);
  bool is_invalid = is_empty; // Initial value is is_empty, since that's invalid too
  const bool bad_temp_scale = (BOOL)(dwScaleSize == 0);
  unsigned int found_prec = 255u;
  if (is_empty || bad_temp_scale) {
    if (bad_temp_scale) {
      MessageBoxW(hWnd, L"Invalid Temp scale!", L"Error!", MB_OK | MB_ICONERROR);
    }
    if (is_empty) {
      MessageBoxW(hWnd, L"No text entered!", L"Empty Temp. Input", MB_OK | MB_ICONWARNING);
    }
    success = false;
  } else {
    std::wostringstream wostr;
    // Create buffers to store the text (using wstring for automatic memory management)
    std::wstring input_buff(dwInputSize + 1, L'\0');
    std::wstring scale_buff(dwScaleSize + 1, L'\0');
    std::wstring preci_buff(dwPrecisionSize + 1, L'\0');

    // Get the text from the edit control
    GetWindowTextW(hInputEdit, &input_buff[0], dwInputSize + 1);
    // Check that it isn't invalid input, like text characters
    if (!IsValidNumericInput(input_buff.c_str())) {
      is_invalid = true; // Bad
    }

    if (is_invalid) {
       MessageBoxW(hWnd, L"Invalid Input!", L"Error!", MB_OK | MB_ICONWARNING);
       success = false;
       ClearInput(hWnd);
       return success; // Fail on invalid input.
    } else {
      input = ConvertInputToLD(input_buff.c_str());
      LOG(INFO) << L"Input = " << std::setprecision(MAX_PRECISION) << input;
    }

    GetWindowTextW(hTempSelectEdit, &scale_buff[0], dwScaleSize + 1);
    GetWindowTextW(hPrecisionCombo, &preci_buff[0], dwPrecisionSize + 1);

    std::wstring scale(scale_buff.c_str());
    std::wstring preci(preci_buff.c_str());

    unsigned int precision_ = std::stoi(preci);
    if (precision_ < MIN_PRECISION || precision_ > MAX_PRECISION) {
      LOG(ERROR) << L"Precision out of range " << MIN_PRECISION
                 << L" - " << MAX_PRECISION << L" Setting precision to max: " 
                 << MAX_PRECISION;
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
    LOG(INFO) << L"Scale = " << kChosenScale;
    const unsigned int precision = GetCryoCalcPrecision();
    found_prec = precision;
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
  if (found_prec > MAX_PRECISION) {
    LOG(ERROR) << L"found_prec out of bounds! " << found_prec;
    success = false;
  } else {
    LOG(INFO) << L"Precision = " << found_prec;
    success = true;
  }
  if (success) {
    LOG(INFO) << L"kCelsius = " << kCelsius;
    LOG(INFO) << L"kKelvin = " << kKelvin;
    LOG(INFO) << L"kFahrenheit = " << kFahrenheit;
    LOG(INFO) << L"kRankine = " << kRankine;
    SetWindowTextW(hCelsiusEdit, kCelsius.c_str());
    SetWindowTextW(hKelvinEdit, kKelvin.c_str());
    SetWindowTextW(hFahrenheitEdit, kFahrenheit.c_str());
    SetWindowTextW(hRankineEdit, kRankine.c_str());
  }
  return success;
}

void InitControls(HWND hWnd, HINSTANCE hInst) {
  const int kTempEditLeft = GetXOffset(STATIC_LEFT, LABEL_WIDTH + INTRA_PADDING, 1.0f);
  // Create staic box outline frame for all controls
  int padding = PADDING_X * 2; // Padding on left and right
  const int x_padding = -(PADDING_X + padding); // Padding already applied to left side + padding
  const int kFrameWidth = GetXOffset(CW_MAINWIDTH, x_padding, 1.0f);
  const unsigned int kLabelYPad = CW_STATICLABEL_HEIGHT + INTRA_PADDING; // Static label height plus 3 pixels between items.
  const unsigned int kEditYPad = CW_EDITCONTROL_HEIGHT + INTRA_PADDING; // Static label height plus 3 pixels between items.
  const unsigned int kFrameBottom = STATIC_TOP + CW_EDITCONTROL_HEIGHT + (kEditYPad * 4);
  unsigned int kButtonColLeft = PADDING_X;
  const unsigned int kButtonCol2Left = (CW_MAINWIDTH / 2) - (BUTTON_WIDTH / 2u) - PADDING_X;
  const unsigned int kButtonCol3Left = kButtonCol2Left + BUTTON_WIDTH + PADDING_X;
  const unsigned int kButtonRowTop = kFrameBottom + (PADDING_Y * 2u);
  const unsigned int kButtonRow2Top = kButtonRowTop + BUTTON_HEIGHT + (PADDING_Y * 2u);
  const unsigned int kProgressBarTop = STATIC_TOP + CW_STATICLABEL_HEIGHT + PADDING_Y;
  hFrameOutline = CreateWindowExW(
      0, WC_STATIC, nullptr,
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
      INPUT_LABEL,             // Text
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY, // Styles
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
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      STATIC_LEFT,
      STATIC_TOP + kLabelYPad,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_C, hInst, nullptr
  );
  hKelvinLabel = CreateWindowExW(
      0, WC_STATIC, L"Kelvin:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * 2),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_K, hInst, nullptr
  );
  hFahrenheitLabel = CreateWindowExW(
      0, WC_STATIC, L"Fahrenheit:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * INTRA_PADDING),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, hInst, nullptr
  );
  hRankineLabel = CreateWindowExW(
      0, WC_STATIC, L"Rankine:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      STATIC_LEFT,
      STATIC_TOP + (kLabelYPad * 4),
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, hInst, nullptr
  );
  hPrecisionLabel = CreateWindowExW(
      0, WC_STATIC, PREC_LABEL,
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
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
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
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
      WS_EX_CLIENTEDGE, WC_EDIT, L"",
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_CELSIUS, hInst, nullptr
  );
  hKelvinEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, L"",
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * 2),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_KELVIN, hInst, nullptr
  );
  hFahrenheitEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, L"",
      WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * INTRA_PADDING),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, hInst, nullptr
  );
  hRankineEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, L"",
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
      kTempEditLeft,
      STATIC_TOP + (kEditYPad * 4),
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, hInst, nullptr
  );

  // Create the "Convert" Button control
  hConvButton = CreateWindowExW(
      0, WC_BUTTON, CONV_BUTTON,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonColLeft,
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CONVERT_BUTTON, hInst, nullptr
  );
  // Create the "Precision" combobox
  hPrecisionCombo = CreateWindowExW(
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
      0, WC_BUTTON, CLEAR_BUTTON,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol2Left,
      kButtonRowTop,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CLEAR_BUTTON, hInst, nullptr
  );
  // Create the "About" Button control
  hAboutButton = CreateWindowExW(
      0, WC_BUTTON, ABOUT_BUTTON,
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
      0, 0, 0, 0,
      hWnd, nullptr, hInst, nullptr
  );

  hThreadsLabel = CreateWindowExW(
      0, WC_STATIC, THREADS_LABEL,
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      kButtonCol3Left,
      STATIC_TOP,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_THREADS, hInst, nullptr
  );

  const unsigned int default_threads = GetDefaultNumThreads();
  std::wstring ws = std::to_wstring(default_threads);
  const wchar_t* THREADS_DEFAULT = ws.c_str();
  // Create the threads number input combobox.
  hThreadsEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, THREADS_DEFAULT,
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
      kButtonCol3Left + LABEL_WIDTH + PADDING_X,
      STATIC_TOP,
      EDIT_WIDTH / 2u, CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_THREADS, hInst, nullptr
  );

  // Create the CPU stressor progress bar.
  hProgressBar = CreateWindowExW(
      0, PROGRESS_CLASS, nullptr,
      WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
      kButtonCol3Left,
      kProgressBarTop,
      PROGBAR_WIDTH, PROGBAR_HEIGHT,
      hWnd, (HMENU)IDC_PROGRESS, hInst, nullptr
  );

  hCacheSizeLabel = CreateWindowEx(
      0, WC_STATIC, CACHE_SIZEQ,
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_NOTIFY,
      kButtonCol3Left,
      kProgressBarTop + PROGBAR_HEIGHT + PADDING_Y,
      LABEL_WIDTH - 16u, CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_CACHE, hInst, nullptr
  );
  hCacheSizeCombo = CreateWindowEx(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol3Left + LABEL_WIDTH + PADDING_X - 16u,
      kProgressBarTop + PROGBAR_HEIGHT + PADDING_Y,
      COMBO_WIDTH + 16u, BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CACHE_SIZE, hInst, nullptr
  );

  hSSE2Checkbox = CreateWindowEx(
      0, WC_BUTTON, USE_SSE2Q,
      WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,
      kButtonCol3Left,
      kProgressBarTop + PROGBAR_HEIGHT + PADDING_Y + CW_EDITCONTROL_HEIGHT + PADDING_Y,
      100u, CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_SSE2_CHECKBOX, hInst, nullptr
  );

  // Create the "Start" CPU Stress Button control
  hStartStresButton = CreateWindowExW(
      0, WC_BUTTON, START_BUTTON,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol3Left,
      kButtonRowTop,
      BUTTON_WIDTH, BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_START_BUTTON, hInst, nullptr
  );
  // Create the "Stop" CPU Stress Button control
  hStopStresButton = CreateWindowExW(
      0, WC_BUTTON, STOP_BUTTON,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol3Left + BUTTON_WIDTH + PADDING_X,
      kButtonRowTop,
      BUTTON_WIDTH, BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_STOP_BUTTON, hInst, nullptr
  );
  // Create the "Show OS Info" Button control
  hOsInfoButton = CreateWindowExW(
      0, WC_BUTTON, OSINFO_BUTTON,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      kButtonCol3Left,
      kButtonRow2Top,
      BUTTON_WIDTH * 2u, BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_OSINFO_BUTTON, hInst, nullptr
  );

  // Set temperature selection options in combobox
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempC.c_str()); // Celsius
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempK.c_str()); // Kelvin
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempF.c_str()); // Fahrenheit
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempR.c_str()); // Rankine
  //SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kDummyScale.c_str()); // Unknown
  // Precision combobox options
  SendMessageW(hPrecisionCombo, CB_ADDSTRING, 0, (LPARAM)L"0");
  SendMessageW(hPrecisionCombo, CB_ADDSTRING, 0, (LPARAM)L"1");
  SendMessageW(hPrecisionCombo, CB_ADDSTRING, 0, (LPARAM)L"2");
  SendMessageW(hPrecisionCombo, CB_ADDSTRING, 0, (LPARAM)L"3");
  SendMessageW(hPrecisionCombo, CB_ADDSTRING, 0, (LPARAM)L"4");
  SendMessageW(hCacheSizeCombo, CB_ADDSTRING, 0, (LPARAM)L"1MB");
  SendMessageW(hCacheSizeCombo, CB_ADDSTRING, 0, (LPARAM)L"2MB");
  SendMessageW(hCacheSizeCombo, CB_ADDSTRING, 0, (LPARAM)L"3MB");
  SendMessageW(hCacheSizeCombo, CB_ADDSTRING, 0, (LPARAM)L"4MB");
  // Set Progress bar to initially stopped and in 0 position
  SendMessageW(hProgressBar, PBM_SETPOS, 0, 0);
  SendMessageW(hProgressBar, PBM_SETMARQUEE, FALSE, 0);
  // Make output edit controls read only.
  SendMessageW(hCelsiusEdit, EM_SETREADONLY, TRUE, 0);
  SendMessageW(hKelvinEdit, EM_SETREADONLY, TRUE, 0);
  SendMessageW(hFahrenheitEdit, EM_SETREADONLY, TRUE, 0);
  SendMessageW(hRankineEdit, EM_SETREADONLY, TRUE, 0);
  // Set default selections
  SendMessageW(hTempSelectEdit, CB_SETCURSEL, 0, 0L);
  SendMessageW(hPrecisionCombo, CB_SETCURSEL, static_cast<int>(GetDefaultPrecision()), 0);
  SendMessageW(hCacheSizeCombo, CB_SETCURSEL, 1, 0);
  SendMessageW(hSSE2Checkbox, BM_SETCHECK, BST_CHECKED, 0);
  InitStatusBar(hWnd, hInst);
  AppendTooltips(hWnd, hInst);
}

void AppendTooltips(HWND hWnd, HINSTANCE hInst) {
  AddTooltip(hWnd, hInputLabel, hInst, L"Temperature input.");
  AddTooltip(hWnd, hInputEdit, hInst, L"Input your temp to calculate here!");
  AddTooltip(hWnd, hTempSelectEdit, hInst, L"Choose temperature scale for Input");
  AddTooltip(hWnd, hConvButton, hInst, L"Convert input to all units");
  AddTooltip(hWnd, hStartStresButton, hInst, L"Start running specified number of CPU threads");
  AddTooltip(hWnd, hStopStresButton, hInst, L"Stop running all CPU threads");
  AddTooltip(hWnd, hPrecisionLabel, hInst, L"Decimal precision for temp calculation");
  AddTooltip(hWnd, hPrecisionCombo, hInst, L"Select decimal precision for temp calculation");
  AddTooltip(hWnd, hThreadsLabel, hInst, L"Input number of threads to stress CPU");
  AddTooltip(hWnd, hThreadsEdit, hInst, L"(default = number of logical CPU cores)");
  AddTooltip(hWnd, hCelsiusLabel, hInst, L"Converted Celsius output");
  AddTooltip(hWnd, hCelsiusEdit, hInst, L"° Celsius");
  AddTooltip(hWnd, hKelvinLabel, hInst, L"Converted Kelvin output");
  AddTooltip(hWnd, hKelvinEdit, hInst, L"° Kelvin");
  AddTooltip(hWnd, hFahrenheitLabel, hInst, L"Converted Fahrenheit output");
  AddTooltip(hWnd, hFahrenheitEdit, hInst, L"° Fahrenheit");
  AddTooltip(hWnd, hRankineLabel, hInst, L"Converted Rankine output");
  AddTooltip(hWnd, hRankineEdit, hInst, L"° Rankine");
  AddTooltip(hWnd, hCacheSizeLabel, hInst, L"The L2 Cache size, in MB, to be used for stressor functions");
  AddTooltip(hWnd, hCacheSizeCombo, hInst, L"Choose the L2 Cache size, in MB, for stressor functions");
  AddTooltip(hWnd, hSSE2Checkbox, hInst, L"Whether to use the SSE2 SIMD stressor function instead of regular one.");
  AddTooltip(hWnd, hClearButton, hInst, L"Clear all temperature output");
  AddTooltip(hWnd, hOsInfoButton, hInst, L"Open OS Information window + utilities");
  AddTooltip(hWnd, hAboutButton, hInst, L"Show About dialog");
  AddTooltip(hWnd, hProgressBar, hInst, L"Threads computation status");
}

void HandleResize(HWND hWnd) {
  if (hWnd == nullptr) {
    return;
  }

  // TODO, move copy of this to InitControls with CW_MAINWIDTH/CW_MAINHEIGHT
  const unsigned int width = current_width;
  const unsigned int height = current_height;
  const int kStatusSplit = width - LABEL_WIDTH;
  const int kStatusParts[2] = { kStatusSplit, -1 }; // -1 = extend to right edge
  const int frame_bottom = GetYOffset(height, 0, 0.6f) - STATIC_BOTTOM;
  const int button_top = frame_bottom + (INTRA_PADDING * 3u);
  const int button2_top = button_top + BUTTON_HEIGHT + PADDING_Y;
  const int kButtonCol2Left = (width / 2) - (BUTTON_WIDTH / 2u) - PADDING_X;
  const int kButtonCol3Left = kButtonCol2Left + BUTTON_WIDTH + PADDING_X;
  const int kButtonCol4Left = kButtonCol3Left + BUTTON_WIDTH + PADDING_X;
  const unsigned int kOsInfoButtonWidth = (BUTTON_WIDTH * 2u) + PADDING_X;
  const unsigned int kFrameWidth = width - END_PADDING;

  MoveWindow(hFrameOutline, PADDING_X, PADDING_Y, kFrameWidth, frame_bottom, TRUE);
  MoveWindow(hConvButton, PADDING_X, button_top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hPrecisionLabel, PADDING_X, button2_top, LABEL_WIDTH, CW_STATICLABEL_HEIGHT, TRUE);
  MoveWindow(hPrecisionCombo, PADDING_X + LABEL_WIDTH + INTRA_PADDING, button2_top, COMBO_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hClearButton, kButtonCol2Left, button_top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hAboutButton, kButtonCol2Left, button2_top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hStartStresButton, kButtonCol3Left, button_top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hStopStresButton, kButtonCol4Left, button_top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
  MoveWindow(hOsInfoButton, kButtonCol3Left, button2_top, kOsInfoButtonWidth, BUTTON_HEIGHT, TRUE);

  if (hStatusBar) {
    SendMessageW(hStatusBar, WM_SIZE, 0, 0);
    SendMessageW(hStatusBar, SB_SETPARTS, 2, (LPARAM)kStatusParts);
  }
}

void InitStatusBar(HWND hWnd, HINSTANCE hInst) {
  std::wstring status_text = kAppName + L" ver. " + GetVersionWstring();
  static std::wstring status_bubble = L"Status";
  const int kStatusSplit = CW_MAINWIDTH - LABEL_WIDTH;
  const int kStatusParts[2] = { kStatusSplit, -1 }; // -1 means extend to right edge
  if (!hStatusBar) {
    LOG(ERROR) << __FUNC__ << L"() failed: Status bar not initialized";
  } else {
    SendMessageW(hStatusBar, SB_SETPARTS, 2, (LPARAM)kStatusParts);
    SendMessageW(hStatusBar, SB_SETTEXT, 0, (LPARAM)status_text.c_str());
    SendMessageW(hStatusBar, SB_SETTEXT, 1, (LPARAM)status_bubble.c_str());
  }
}

bool SetClientRects(HWND hWnd, HINSTANCE hInst) {
  // Get rect size of window including titlebar, for setting other Window's positions relative to this window
  if (!GetWindowRect(hMainWindow, &kMainWinRect)) {
    return false;
  }
  // Get internal rects inside Window, excluding titlebar, for setting control positions inside Window
  if (!GetClientRect(hWnd, &kMainClientRect)) {
    return false;
  }
  return true;
}

void ClearInput(HWND hWnd) {
  SetWindowTextW(hInputEdit, kBlank);
  LOG(WARN) << L"Cleared input";
}

void ClearControls(HWND hWnd) {
  SetWindowTextW(hCelsiusEdit, kBlank);
  SetWindowTextW(hKelvinEdit, kBlank);
  SetWindowTextW(hFahrenheitEdit, kBlank);
  SetWindowTextW(hRankineEdit, kBlank);
  LOG(WARN) << L"Cleared controls";
}

static errno_t wcsncpy_s_compat(wchar_t* dest, size_t destsz, const wchar_t* src, size_t count) {
  if (!dest || destsz == 0) {
    return EINVAL;
  }

  if (!src) {
    dest[0] = L'\0';
    return EINVAL;
  }

  size_t i = 0;
  if (count == _TRUNCATE) {
    // Copy until dest is full or src ends
    for (; i < destsz - 1 && src[i]; ++i) {
      dest[i] = src[i];
    }

    dest[i] = L'\0';
    return 0;  // truncation is NOT an error
  }

  // Normal bounded copy
  for (; i < count && i < destsz - 1 && src[i]; ++i) {
    dest[i] = src[i];
  }

  dest[i] = L'\0';

  // If we ran out of space before copying count characters → error
  if (i == destsz - 1 && src[i] && i < count) {
    return ERANGE;
  }
  return 0;
}

bool GetClipboardTextW(wchar_t* buffer, size_t bufferSize) {
  bool success = false;
  if (!buffer || bufferSize == 0) {
    return false;
  }
  if (!OpenClipboard(nullptr)) {
    return false;
  }

  HANDLE hData = GetClipboardData(CF_UNICODETEXT);
  if (hData) {
    const wchar_t* pText =
        static_cast<const wchar_t*>(GlobalLock(hData));
    if (pText) {
      if (debug_mode) {
        LOG(DEBUG) << __FUNC__ << L"() got: " << pText;
      }
      wcsncpy_s_compat(buffer, bufferSize, pText, _TRUNCATE);
      GlobalUnlock(hData);
      success = true;
    }
  }

  CloseClipboard();
  return success;
}

bool HandlePaste(HWND hWnd) {
  std::wstring clpbrd_buff(255, L'\0');

  // Get text from clipboard
  if (!GetClipboardTextW(&clpbrd_buff[0], 255)) {
    return false;
  }

  if (!IsValidNumericInput(clpbrd_buff.c_str())) {
    MessageBoxW(hWnd, L"Invalid Paste Input!", L"Error!", MB_OK | MB_ICONWARNING);
    return false;
  }

  SetWindowTextW(hInputEdit, clpbrd_buff.c_str());
  return true;
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
    LOG(INFO) << L"Showed about dialog.";
  } else {
    LOG(ERROR) << L"About dialog failed!";
  }
  return handled_dialog;
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);

  bool AboutHandled = false; // Stores status of whether dialog has been handled user-wise.
  static const HICON kSmallIcon = LoadIcon(GetInstanceFromHwnd(hDlg), MAKEINTRESOURCE(IDI_SMALL));
  switch (message) {
    case WM_INITDIALOG: {
      // Set icon in titlebar of about dialog
      SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)kSmallIcon);
      SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)kSmallIcon);
      AboutHandled = true;
      SetAboutHandled(AboutHandled);
    } break;
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
          break;
        }
      } break;
    default:
      SetAboutHandled(true);
      break;
  }

  // About dialog failed
  return (INT_PTR)AboutHandled;
}

DWORD WINAPI AnimateProg(LPVOID lpParam) {
  while (true) {
    SendMessageW(hProgressBar, PBM_DELTAPOS, 25, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    SendMessageW(hProgressBar, PBM_DELTAPOS, 50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    SendMessageW(hProgressBar, PBM_DELTAPOS, 75, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    SendMessageW(hProgressBar, PBM_DELTAPOS, 100, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    SendMessageW(hProgressBar, PBM_DELTAPOS, 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return 0;
}

bool GetThreadsInput(HWND hWnd) {
  DWORD dwThrInputSize = GetWindowTextLength(hThreadsEdit);
  const bool is_empty = (BOOL)(dwThrInputSize == 0);
  bool is_invalid = is_empty;
  if (is_empty) {
    MessageBoxW(hWnd, L"No threads entered!", L"Empty Threads Input", MB_OK | MB_ICONWARNING);
    return false;
  }
  std::wstring in_buff(dwThrInputSize + 1, L'\0');
  GetWindowTextW(hThreadsEdit, &in_buff[0], dwThrInputSize + 1);
  if (!IsValidThreadsInput(in_buff.c_str())) {
    is_invalid = true;
  }
  unsigned int get_threads;
  const unsigned int num_logical_cpus = static_cast<unsigned int>(GetLogicalProcessorCount());
  if (is_invalid) {
    SendMessageW(hProgressBar, PBM_SETMARQUEE, FALSE, 0);
    const std::wstring valid_msg = L"Invalid Threads Input! \nValid values are 1 - " + std::to_wstring(MAX_THREADS);
    MessageBoxW(hWnd, valid_msg.c_str(), L"Error.", MB_OK | MB_ICONWARNING);
    return false;
  } else {
    std::wstring threads_input(in_buff.c_str());
    get_threads = std::stoi(threads_input);
    LOG(DEBUG) << L"Number of system CPU threads: " << num_logical_cpus;
    if (get_threads > num_logical_cpus) {
      LOG(ERROR) << L"Threads input is larger than the number of system CPU threads! " << num_logical_cpus;
      std::wostringstream wostr;
      wostr << L"You have entered more threads (" << get_threads << ") than the machine's CPU has (" << num_logical_cpus << ") "
            << L"\nWould you still like to continue?";
      std::wstring wout = wostr.str();
      int allow_threads =
          MessageBoxW(nullptr, wout.c_str(), L"Threads Confirmation",
                      MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
      switch (allow_threads) {
        case IDNO:
        case IDCANCEL:
          return false;
        case IDOK:
          break;
        default:
          return false;
      }
    }
  }
  if (debug_mode) {
    LOG(DEBUG) << L"Got " << get_threads << L" number of threads to start from input box.";
  }
  num_threads_ = get_threads;
  return num_threads_ >= MIN_THREADS && num_threads_ <= MAX_THREADS;
}

// Get number of threads and launch them.
void OnStartButtonClick(HWND hWnd) {
  // Start animating the progress bar marquee
  SendMessageW(hProgressBar, PBM_SETMARQUEE, TRUE, 100);
  if (GetThreadsInput(hWnd)) {
    LOG(INFO) << L"Starting " << num_threads_ << L" CPU stressor threads.";
    set_run_state(true);
    std::thread StressorLaunchThread(LaunchThreads, num_threads_);
    StressorLaunchThread.detach(); // Make sure to join the stress thread before exiting
  } else {
    LOG(ERROR) << __FUNC__ << L"() failed!";
    // Stop animating if we failed for some reason.
    SendMessageW(hProgressBar, PBM_SETMARQUEE, FALSE, 0);
  }
}

// Stop all threads. Called when "Stop" button pressed and when closing app/shutting down windows.
void OnStopButtonClick(HWND hWnd) {
  // Stop animating the progress bar and reset to empty state
  SendMessageW(hProgressBar, PBM_SETPOS, 0, 0);
  SendMessageW(hProgressBar, PBM_SETMARQUEE, FALSE, 0);
  StopAllThreads();
}
