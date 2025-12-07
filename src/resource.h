// {{NO_DEPENDENCIES}}
// For #define-ing static resources for resource script file(s).
// Used by resource.rc

// Icons
#define IDI_CRYOCALC			          101
#define IDI_SMALL				            102

// Main application resource
#define IDC_CRYOCALC			          103

// Dialogs
#define IDD_ABOUT_DIALOG  	        104
#define IDD_ABOUTBOX			          105

// Menu items
#define IDM_ABOUT				            200
#define IDM_EXIT				            201
#define IDM_HELP				            202

// Controls
#define IDC_BUTTON1   			        300
#define IDC_CELSIUS  	              301
#define IDC_KELVIN  	              302
#define IDC_FAHRENHEIT              303
#define IDC_RANKINE                 304
#define IDC_LABEL_C                 305
#define IDC_LABEL_K                 306
#define IDC_LABEL_F                 307
#define IDC_LABEL_R                 308

// Static values
#define CW_MAINWIDTH                200
#define CW_MAINHEIGHT               400

// For resources to be loaded without an ID from the system.
#ifndef IDC_STATIC
 #define IDC_STATIC				         -1
#endif // IDC_STATIC

#define APP_MANIFEST                1
