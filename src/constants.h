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
constexpr unsigned int CW_MAINWIDTH  = 232; // Width of main window
constexpr unsigned int CW_MAINHEIGHT = 308; // Height of main window

constexpr unsigned int CW_STATICLABEL_HEIGHT = 24; // Height of static text label controls
constexpr unsigned int CW_EDITCONTROL_HEIGHT = CW_STATICLABEL_HEIGHT; // Height of edit controls

constexpr INT  STATIC_LEFT   = 10;  // How many pixels to pad left side before labels
constexpr INT  EDIT_LEFT     = 90;  // How many pixels to pad left side before edit controls
constexpr UINT LABEL_WIDTH   = 60;  // Width in pixels of a static label
constexpr UINT EDIT_WIDTH    = 75;  // Width in pixels of an edit control
constexpr UINT BUTTON_Y      = 175; // Y pixel units from top of frame to position buttons
constexpr UINT BUTTON_WIDTH  = 75;  // Width in pixels buttons
constexpr UINT BUTTON_HEIGHT = 30;  // Height in pixels of buttons

#endif // CRYOCALC_CONSTANTS_H_
