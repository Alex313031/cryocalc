// {{NO_DEPENDENCIES}}
// For #define-ing static resources for resource script file(s).
// Used by resource.rc

// Icons
#define IDI_CRYOCALC                101
#define IDI_SMALL                   102

// Main application resource
#define IDC_CRYOCALC                103

// Dialogs
#define IDD_ABOUT_DIALOG            104
#define IDD_ABOUTBOX                105

// Menu items
#define IDM_ABOUT                   200
#define IDM_EXIT                    201
#define IDM_CEXIT                   202
#define IDM_HELP                    203
#define IDM_CLEAR                   204
#define IDM_CUT                     205
#define IDM_COPY                    206
#define IDM_PASTE                   207

// Buttons
#define IDC_CONVERT                 300
#define IDC_CONVERT_BUTTON          301
#define IDC_CLEAR_BUTTON            302
#define IDC_ABOUT_BUTTON            303

// User input controls
#define IDC_INPUT                   309
#define IDC_SCALE                   310
#define IDC_PRECISION               311

// Labels
#define IDC_LABEL_INPUT             312
#define IDC_LABEL_C                 313
#define IDC_LABEL_K                 314
#define IDC_LABEL_F                 315
#define IDC_LABEL_R                 316

// Output boxes
#define IDC_CELSIUS                 317
#define IDC_KELVIN                  318
#define IDC_FAHRENHEIT              319
#define IDC_RANKINE                 320

// Our main Window Class unique name
#define CRYOCALC_MAIN_WNDCLASS      L"CryoCalcClass"

// For resources to be loaded without an ID from the system.
#ifndef IDC_STATIC
 #define IDC_STATIC                -1
#endif // IDC_STATIC

// For importing .manifest in .rc file
#define APP_MANIFEST                1
