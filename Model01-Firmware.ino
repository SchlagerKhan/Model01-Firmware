// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

// http://www.keyboard-layout-editor.com/#/gists/f0e56d43677bd4c2592e4b506329b2c8

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif


/**
 * These #include directives pull in the Kaleidoscope firmware core,
 * as well as the Kaleidoscope plugins we use in the Model 01's firmware
 */


// The Kaleidoscope core
#include "Kaleidoscope.h"

// Support for storing the keymap in EEPROM
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"

// Support for communicating with the host via a simple Serial protocol
#include "Kaleidoscope-FocusSerial.h"

// Support for macros
#include "Kaleidoscope-Macros.h"

// Support for controlling the keyboard's LEDs
#include "Kaleidoscope-LEDControl.h"

// Support for the "Boot greeting" effect, which pulses the 'LED' button for 10s
// when the keyboard is connected to a computer (or that computer is powered on)
#include "Kaleidoscope-LEDEffect-BootGreeting.h"

// Support for shared palettes for other plugins, like Colormap below
#include "Kaleidoscope-LED-Palette-Theme.h"

// Support for an LED mode that lets one configure per-layer color maps
#include "Kaleidoscope-Colormap.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

// Support for USB quirks, like changing the key state report protocol
#include "Kaleidoscope-USB-Quirks.h"

// One Shot (Custom)
#include "Kaleidoscope-ShapeShifter.h"
#include "Kaleidoscope-TopsyTurvy.h"
#include "Kaleidoscope-TapDance.h"

#include "Kaleidoscope-OneShot.h"
#include "kaleidoscope/hid.h"
#include "Kaleidoscope-LED-ActiveModColor.h"

// Layers
enum { PRIMARY, FUNCTION };

// TapDance enums
enum {
  L_PAREN, // Left Parentesis/[Curly] Bracket
  R_PAREN // Right Parentesis/[Curly] Bracket
};

// Keymaps
KEYMAPS(
  [PRIMARY] = KEYMAP_STACKED
  ( ___,                      Key_9,              Key_7,                Key_5,          Key_3,      Key_1,      ___,                  // Row 1
    TOPSY(Backslash),         Key_Q,              Key_W,                Key_E,          Key_R,      Key_T,      TD(L_PAREN),          // Row 2
    Key_Tab,                  Key_A,              Key_S,                Key_D,          Key_F,      Key_G,                            // Row 3
    ___,                      Key_Z,              Key_X,                Key_C,          Key_V,      Key_B,      OSM(LeftAlt),         // Row 4
    TOPSY(Semicolon),         Key_Backspace,      OSM(LeftShift),       Key_Escape,                                                   // Thumb
    ShiftToLayer(FUNCTION),                                                                                                           // Function

    
    ___,                      Key_0,              Key_2,            Key_4,              Key_6,          Key_8,            ___,            // Row 1
    TD(R_PAREN),              Key_Y,              Key_U,            Key_I,              Key_O,          Key_P,            Key_Backtick,   // Row 2
                              Key_H,              Key_J,            Key_K,              Key_L,          Key_Equals,       Key_Quote,      // Row 3
    OSM(RightControl),        Key_N,              Key_M,            Key_Comma,          Key_Period,     Key_Slash,        ___,            // Row 4
    Key_Minus,                OSM(RightAlt),      Key_Spacebar,     OSM(RightGui),                                                        // Thumb
    ShiftToLayer(FUNCTION),                                                                                                               // Function
  ),

  [FUNCTION] =  KEYMAP_STACKED
  ( Key_F11,            Key_F9,             Key_F7,           Key_F5,             Key_F3,     Key_F1,       Key_PrintScreen,          // Row 1
    XXX,                XXX,                XXX,              XXX,                XXX,        XXX,          XXX,                      // Row 2
    XXX,                XXX,                XXX,              XXX,                XXX,        XXX,                                    // Row 3
    XXX,                XXX,                XXX,              XXX,                XXX,        XXX,          XXX,                      // Row 4
    XXX,                Key_Delete,         LGUI(Key_Z),      LSHIFT(LGUI(Key_Z)),                                                    // Thumb
    XXX,                                                                                                                              // Function


    XXX,                                Key_F10,                        Key_F2,                           Key_F4,                           Key_F6,               Key_F8,                 Key_F12,      // Row 1
    Consumer_PlaySlashPause,            Consumer_ScanPreviousTrack,     Consumer_ScanNextTrack,           XXX,                              XXX,                  XXX,                    XXX,          // Row 2
                                        Key_LeftArrow,                  Key_DownArrow,                    Key_UpArrow,                      Key_RightArrow,       XXX,                    XXX,          // Row 3
    XXX,                                XXX,                            XXX,                              XXX,                              XXX,                  XXX,                    XXX,          // Row 4
    XXX,                                XXX,                            Key_Enter,                        XXX,                                                                                          // Thumb
    XXX                                                                                                                                                                                                 // Function
  )
)

// ShapeShifter
static const kaleidoscope::plugin::ShapeShifter::dictionary_t shape_shift_dictionary[] PROGMEM = {
  // Numerical
  {Key_1, Key_4},
  {Key_3, Key_6},
  {Key_5, Key_8},
  {Key_7, Key_2},
  {Key_9, Key_NoKey},
  {Key_0, Key_5},
  {Key_2, Key_1},
  {Key_4, Key_3},
  {Key_6, Key_7},
  {Key_8, Key_NoKey},

  // Others
  {Key_NoKey, Key_NoKey},
};

// TapDance
void tapDanceAction(uint8_t tap_dance_index, byte row, byte col, uint8_t tap_count, kaleidoscope::plugin::TapDance::ActionType tap_dance_action) { 
  switch (tap_dance_index) {
    case L_PAREN:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_LeftParen, Key_LeftBracket, Key_LeftCurlyBracket);
    case R_PAREN:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_RightParen, Key_RightBracket, Key_RightCurlyBracket);
  } 
}

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    LEDControl.paused = true;
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    break;
  }
}

/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

// Init plugins
KALEIDOSCOPE_INIT_PLUGINS(
  // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an editable keymap in EEPROM.
  EEPROMSettings,
  EEPROMKeymap,

  // Focus allows bi-directional communication with the host, and is the interface through which the keymap in EEPROM can be edited.
  Focus,

  // FocusSettingsCommand adds a few Focus commands, intended to aid in
  // changing some settings of the keyboard, such as the default layer (via the
  // `settings.defaultLayer` command)
  FocusSettingsCommand,

  // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
  // both debugging, and in backing up one's EEPROM contents.
  FocusEEPROMCommand,

  // The boot greeting effect pulses the LED button for 10 seconds after the keyboard is first connected
  BootGreetingEffect,

  // LEDControl provides support for other LED modes
  LEDControl,

  // We start with the LED effect that turns off all the LEDs.
  LEDOff,

  // The LED Palette Theme plugin provides a shared palette for other plugins,
  // like Colormap below
  LEDPaletteTheme,

  // The Colormap effect makes it possible to set up per-layer colormaps
  ColormapEffect,

  // The macros plugin adds support for macros
  Macros,

  // The HostPowerManagement plugin allows us to turn LEDs off when then host
  // goes to sleep, and resume them when it wakes up.
  HostPowerManagement,

  // The USBQuirks plugin lets you do some things with USB that we aren't
  // comfortable - or able - to do automatically, but can be useful
  // nevertheless. Such as toggling the key report protocol between Boot (used
  // by BIOSes) and Report (NKRO).
  USBQuirks,

  // Custom
  TopsyTurvy,
  TapDance,
  
  OneShot,
  ActiveModColorEffect,
  
  ShapeShifter
);

/** The 'setup' function is one of the two standard Arduino sketch functions.
 * It's called when your keyboard first powers up. This is where you set up
 * Kaleidoscope and any plugins.
 */
void setup() {
  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();

  // We want to make sure that the firmware starts with LED effects off
  // This avoids over-taxing devices that don't have a lot of power to share
  // with USB devices
  LEDOff.activate();

  // We need to tell the Colormap plugin how many layers we want to have custom
  // maps for. To make things simple, we set it to five layers, which is how
  // many editable layers we have (see above).
  ColormapEffect.max_layers(5);

  ShapeShifter.dictionary = shape_shift_dictionary;
}

/** loop is the second of the standard Arduino sketch functions.
  * As you might expect, it runs in a loop, never exiting.
  *
  * For Kaleidoscope-based keyboard firmware, you usually just want to
  * call Kaleidoscope.loop(); and not do anything custom here.
  */

void loop() {
  Kaleidoscope.loop();
}
