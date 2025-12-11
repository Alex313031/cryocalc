#include "controls.h"

// Static labels
static HWND hInputLabel;  
static HWND hCelsiusLabel;  
static HWND hKelvinLabel;
static HWND hFahrenheitLabel;
static HWND hRankineLabel;
static HWND hFrameLabel;

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

// Temp select titles
static const std::wstring kTempC = L"C";
static const std::wstring kTempK = L"K";
static const std::wstring kTempF = L"F";
static const std::wstring kTempR = L"R";

// Button titles
static const std::wstring CONV_BUTTON  = L"Convert";
static const std::wstring CLEAR_BUTTON = L"Clear";
static const std::wstring ABOUT_BUTTON = L"About";

bool _about_handled = false;

Scale parseScale(const std::wstring& wscale) {
  if (wscale == kTempC) return kScaleCelsius;
  if (wscale == kTempK) return kScaleKelvin;
  if (wscale == kTempF) return kScaleFahrenheit;
  if (wscale == kTempR) return kScaleRankine;
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
  std::wstring kCelsius, kKelvin, kFahrenheit, kRankine;
  long double convCelsius, convKelvin, convFahrenheit, convRankine;
  long double input = 0.0L;
  // Get the length of the text in the edit control
  DWORD dwInputSize = GetWindowTextLength(hInputEdit);
  DWORD dwScaleSize = GetWindowTextLength(hTempSelectEdit);
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
    wchar_t* scale_buff = new wchar_t[dwScaleSize + 1]; // Allocate memory for the text

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
    }

    GetWindowTextW(hTempSelectEdit, scale_buff, dwScaleSize + 1);
    input = ConvertInputToLD(input_buff);
    std::wstring scale(scale_buff);
    std::wcout << L"Input = " << std::fixed
               << std::setprecision(CRYOCALC_PRECISION) << input << std::endl;

    Scale selected_scale = parseScale(scale);
    std::wstring kChosenScale;
    switch (selected_scale) {
      case kScaleCelsius:
        convCelsius = input;
        convKelvin = kelvin::fromCelsius(input);
        convFahrenheit = fahrenheit::fromCelsius(input);
        convRankine = rankine::fromCelsius(input);
        kChosenScale = L"Celsius";
        break;
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
        kChosenScale = L"Unknown";
        MessageBoxW(hWnd, L"Temp scale not handled", L"Error", MB_OK | MB_ICONERROR);
    }
    std::wcout << L"Scale = " << kChosenScale << std::endl;
    wostr << std::fixed << std::setprecision(CRYOCALC_PRECISION) << convCelsius;
    kCelsius = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(CRYOCALC_PRECISION) << convKelvin;
    kKelvin = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(CRYOCALC_PRECISION) << convFahrenheit;
    kFahrenheit = wostr.str();
    wostr.str(L"");
    wostr << std::fixed << std::setprecision(CRYOCALC_PRECISION) << convRankine;
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
  return success;
}

void InitControls(HWND hWnd) {
  const HINSTANCE gHinst = GetGlobalHinst();
  hInputLabel = CreateWindowExW(
      0,                      // Optional window styles
      WC_STATIC,              // Predefined class: Static
      L"Input:",              // Text
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN, // Styles
      STATIC_LEFT,            // x position
      25,                     // y position
      LABEL_WIDTH,            // Width
      CW_STATICLABEL_HEIGHT,  // Height
      hWnd,                   // Parent window
      (HMENU)IDC_LABEL_INPUT, // No menu, but control ID
      gHinst,                 // This window instance
      nullptr                 // Additional app data
  );

  // Temperature input
  hInputEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_EDIT, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      EDIT_LEFT,
      25,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_INPUT, gHinst, nullptr
  );
  // Temperature scale Combobox
  hTempSelectEdit = CreateWindowExW(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      172,
      25,
      40,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_SCALE, gHinst, nullptr
  );

  // Label (Static Control)
  hCelsiusLabel = CreateWindowExW(
      0, WC_STATIC, L"Celsius:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      52,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_C, gHinst, nullptr
  );
  // Create the Edit controls
  hCelsiusEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      EDIT_LEFT,
      52,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_CELSIUS, gHinst, nullptr
  );
  hKelvinLabel = CreateWindowExW(
      0, WC_STATIC, L"Kelvin:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      77,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_K, gHinst, nullptr
  );
  hKelvinEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      EDIT_LEFT,
      77,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_KELVIN, gHinst, nullptr
  );
  hFahrenheitLabel = CreateWindowExW(
      0, WC_STATIC, L"Fahren:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      102,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, gHinst, nullptr
  );
  hFahrenheitEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      EDIT_LEFT,
      102,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, gHinst, nullptr
  );
  hRankineLabel = CreateWindowExW(
      0, WC_STATIC, L"Rankine:",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
      STATIC_LEFT,
      127,
      LABEL_WIDTH,
      CW_STATICLABEL_HEIGHT,
      hWnd, (HMENU)IDC_LABEL_F, gHinst, nullptr
  );
  hRankineEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
      EDIT_LEFT,
      127,
      EDIT_WIDTH,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_FAHRENHEIT, gHinst, nullptr
  );

  hFrameLabel = CreateWindowExW(
      0, WC_STATIC, L"Frame",
      WS_CHILD | WS_VISIBLE | SS_LEFT | SS_ETCHEDFRAME,
      5,
      15,
      217,
      150,
      hWnd, nullptr, gHinst, nullptr
  );

  // Create the "Convert" Button control
  hConvButton = CreateWindowExW(
      0, WC_BUTTON, CONV_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      STATIC_LEFT,
      BUTTON_Y,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CONVERT_BUTTON, gHinst, nullptr
  );
  // Create the "Clear" Button control
  hClearButton = CreateWindowExW(
      0, WC_BUTTON, CLEAR_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      128, // Move about button manually to the right
      BUTTON_Y,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_CLEAR_BUTTON, gHinst, nullptr
  );
  // Create the "Precision" combobox
  hPrecisionEdit = CreateWindowExW(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      STATIC_LEFT,
      BUTTON_Y + 48,
      40,
      CW_EDITCONTROL_HEIGHT,
      hWnd, (HMENU)IDC_PRECISION, gHinst, nullptr
  );
  // Create the "About" Button control
  hAboutButton = CreateWindowExW(
      0, WC_BUTTON, ABOUT_BUTTON.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      128,
      BUTTON_Y + 48,
      BUTTON_WIDTH,
      BUTTON_HEIGHT,
      hWnd, (HMENU)IDC_ABOUT_BUTTON, gHinst, nullptr
  );

  // Set temperature selection options in combobox
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempC.c_str()); // Celsius
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempK.c_str()); // Kelvin
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempF.c_str()); // Fahrenheit
  SendMessageW(hTempSelectEdit, CB_ADDSTRING, 0, (LPARAM)kTempR.c_str()); // Rankine
  // Precision combobox options
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"0");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"1");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"2");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"3");
  SendMessageW(hPrecisionEdit, CB_ADDSTRING, 0, (LPARAM)L"4");
  // Set default selections
  SendMessageW(hTempSelectEdit, CB_SETCURSEL, 0, 0);
  SendMessageW(hPrecisionEdit, CB_SETCURSEL, 3, 0);
}

static const wchar_t* kBlank = L"";

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
  DialogBoxW(gHinst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
  bool handled_dialog = GetAboutHandledState();
  if (handled_dialog) {
    std::wcout << L"Showed about dialog." << std::endl;
    return true;
  } else {
    std::wcerr << L"About dialog failed!" << std::endl;
    return false;
  }
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
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
