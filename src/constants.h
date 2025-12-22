#ifndef CRYOCALC_CONSTANTS_H_
#define CRYOCALC_CONSTANTS_H_

#include "globals.h"

constexpr unsigned int MIN_PRECISION = 0;

constexpr unsigned int MAX_PRECISION = 4;

constexpr unsigned int CRYOCALC_PRECISION = 2;

constexpr long double KELVIN_OFFSET = 273.150L;

constexpr long double RANKINE_OFFSET = 459.670L;

constexpr long double kDummyCelsius = 0.000L; // Freezing point of water

constexpr long double kDummyKelvin = 77.000L; // Boiling point of Liquid Nitrogen

constexpr long double kDummyFahrenheit = 212.000L; // Boiling point of water

constexpr long double kDummyRankine = 671.641L; // Boiling point of water

// Static values
constexpr unsigned int CW_MAINWIDTH  = 428; // Width of main window
constexpr unsigned int CW_MAINHEIGHT = 320; // Height of main window

constexpr unsigned int CW_STATICLABEL_HEIGHT = 24; // Height of static text label controls
constexpr unsigned int CW_EDITCONTROL_HEIGHT = CW_STATICLABEL_HEIGHT; // Height of edit controls

// Padding metrics
constexpr UINT PADDING_X     = 6;  // Minimum pixels to pad all content on the left and right
constexpr UINT PADDING_Y     = 6;  // Minimum pixels to pad all content on the left and right
constexpr INT STATIC_TOP     = 12;  // Minimum pixels to pad all content from top
constexpr INT STATIC_LEFT    = 12;  // How many pixels to pad left side before labels
constexpr INT STATIC_RIGHT   = 12;  // How many pixels to pad right side after labels
constexpr INT STATIC_BOTTOM  = 12;  // How many pixels to pad bottom

// Control layout metrics
constexpr UINT LABEL_WIDTH   = 80;  // Width in pixels of a static label
constexpr UINT EDIT_WIDTH    = 80;  // Width in pixels of an edit control
constexpr UINT BUTTON_Y      = CW_MAINHEIGHT / 2; // Y pixel units from top of frame to position buttons
constexpr UINT BUTTON_WIDTH  = 75;  // Width in pixels buttons
constexpr UINT BUTTON_HEIGHT = 30;  // Height in pixels of buttons
constexpr UINT COMBO_WIDTH   = 40;  // Width in pixels of comboboxes
constexpr UINT INTRA_PADDING = 3;   // Value in pixels of intra-item padding
constexpr UINT BOTTOM_AREA   = CW_MAINHEIGHT / 3;   // Minimum height of bottom buttom controls area

#endif // CRYOCALC_CONSTANTS_H_
