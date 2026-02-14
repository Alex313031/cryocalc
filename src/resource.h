// {{NO_DEPENDENCIES}}
// For #define-ing static resources for resource script file(s).
// Used by resource.rc

// For importing .manifest in .rc file
#define APP_MANIFEST                1

// Icons
#define IDI_CRYOCALC                101
#define IDI_SMALL                   102
#define IDI_WINFLAG                 103

// Main application resource
#define IDC_CRYOCALC                104

// Dialogs
#define IDD_ABOUT_DIALOG            105
#define IDD_ABOUTBOX                106

// Menu items
#define IDM_ABOUT                   200
#define IDM_EXIT                    201
#define IDM_CEXIT                   202
#define IDM_HELP                    203
#define IDM_CLEAR                   204
#define IDM_CUT                     205
#define IDM_COPY                    206
#define IDM_PASTE                   207
#define IDM_OSINFO                  208
#define IDM_HELPEX                  209
#define IDM_RUN                     210
#define IDM_ATTACH_CON              211
#define IDM_DETACH_CON              212
#define IDM_CLEAR_CON               213
#define IDM_CLEAR_LOG               214
#define IDM_TEST_LOG                215
#define IDM_OPEN_LOG                216

// Buttons
#define IDC_CONVERT                 300
#define IDC_CONVERT_BUTTON          301
#define IDC_CLEAR_BUTTON            302
#define IDC_ABOUT_BUTTON            303
#define IDC_START_BUTTON            304
#define IDC_STOP_BUTTON             305
#define IDC_OSINFO_BUTTON           306
#define IDC_WINVER_BUTTON           307
#define IDC_MSINFO_BUTTON           308
#define IDC_RUNAPP_BUTTON           309
#define IDC_ALLOC_MEM               310

// User input controls
#define IDC_INPUT                   320
#define IDC_SCALE                   321
#define IDC_PRECISION               322
#define IDC_THREADS                 323
// Stress SSE2 checkbox
#define IDC_SSE2_CHECKBOX           324
// Cache size input combobox
#define IDC_CACHE_SIZE              325

// Labels
#define IDC_LABEL_INPUT             330
#define IDC_LABEL_PREC              331
#define IDC_LABEL_C                 332
#define IDC_LABEL_K                 333
#define IDC_LABEL_F                 334
#define IDC_LABEL_R                 335
#define IDC_LABEL_THREADS           336
#define IDC_LABEL_CACHE             337

// Output boxes
#define IDC_CELSIUS                 340
#define IDC_KELVIN                  341
#define IDC_FAHRENHEIT              342
#define IDC_RANKINE                 343

// Progress bar
#define IDC_PROGRESS                350
// Command to close Os Info Window gracefully when parent window is closing
#define IDC_CLOSE_OSINFO            351
// Os Info Logging Output Window
#define IDC_OSINFO_OUT              352

// Name of the App
#define APP_NAME L"CryoCalc"

// Our main Window Class unique name
#define CRYOCALC_MAIN_WNDCLASS      L"CryoCalcClass"

// Os Info Window Class
#define CRYOCALC_OSINFO_WNDCLASS    L"CryoCalcOsInfoClass"

// For resources to be loaded without an ID from the system.
#ifndef IDC_STATIC
 #define IDC_STATIC                -1
#endif // IDC_STATIC
