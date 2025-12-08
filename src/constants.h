#ifndef CRYOCALC_CONSTANTS_H_
#define CRYOCALC_CONSTANTS_H_

#include "globals.h"

constexpr unsigned int CRYOCALC_PRECISION = 3;

constexpr long double KELVIN_OFFSET = 273.150L;

constexpr long double RANKINE_OFFSET = 459.670L;

constexpr long double kDummyCelsius = 0.000L; // Freezing point of water

constexpr long double kDummyKelvin = 77.000L; // Boiling point of Liquid Nitrogen

constexpr long double kDummyFahrenheit = 212.000L; // Boiling point of water

constexpr long double kDummyRankine = 671.641L; // Boiling point of water

// Static values
constexpr unsigned int CW_MAINWIDTH  = 212; // Width of main window
constexpr unsigned int CW_MAINHEIGHT = 302; // Height of main window

constexpr unsigned int CW_STATICLABEL_HEIGHT = 24; // Height of static text label controls
constexpr unsigned int CW_EDITCONTROL_HEIGHT = CW_STATICLABEL_HEIGHT; // Height of edit controls

constexpr int          STATIC_LEFT   = 10; // How many pixels to pad left side before labels
constexpr int          EDIT_LEFT     = 75; // How many pixels to pad left side before edit controls
constexpr unsigned int LABEL_WIDTH   = 60; // Width in pixels of a static label
constexpr unsigned int EDIT_WIDTH    = 75; // Width in pixels of an edit control
constexpr unsigned int BUTTON_Y      = 170; // Y pixel units from top of frame to position buttons
constexpr unsigned int BUTTON_WIDTH  = 75; // Width in pixels buttons
constexpr unsigned int BUTTON_HEIGHT = 30; // Height in pixels of buttons

#endif // CRYOCALC_CONSTANTS_H_
