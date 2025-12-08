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
#define IDM_CLEAR				            203

// Controls
#define IDC_CONVERT_BUTTON   			  300
#define IDC_ABOUT_BUTTON   		   	  301
#define IDC_INPUT    	              302
#define IDC_SCALE    	              303
#define IDC_CELSIUS  	              304
#define IDC_KELVIN  	              305
#define IDC_FAHRENHEIT              306
#define IDC_RANKINE                 307
#define IDC_LABEL_INPUT             308
#define IDC_LABEL_C                 309
#define IDC_LABEL_K                 310
#define IDC_LABEL_F                 311
#define IDC_LABEL_R                 312

#define CRYOCALC_MAIN_WNDCLASS      L"CryoCalcClass"

// For resources to be loaded without an ID from the system.
#ifndef IDC_STATIC
 #define IDC_STATIC				         -1
#endif // IDC_STATIC

#define APP_MANIFEST                1
