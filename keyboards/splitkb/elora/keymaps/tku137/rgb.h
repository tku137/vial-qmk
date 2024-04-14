#pragma once

#include QMK_KEYBOARD_H

#include <stdint.h>

// struct to hold all RGB settings
typedef struct {
    // HSV values for color definition
    uint8_t hue;
    uint8_t sat;
    uint8_t val;

    // Effect mode and speed
    uint8_t mode;
    uint8_t speed;
} rgb_settings_t;

// Define some nice colors
extern const rgb_settings_t default_sand1;    // Sand-like color mathing the keycaps
extern const rgb_settings_t default_sand2;    // Sand-like color mathing the keycaps
extern const rgb_settings_t default_white;    // White color mathing the OLED
extern const rgb_settings_t default_turquise; // Cyberpunk turquise color
extern const rgb_settings_t default_purple;   // Cyberpunk purple color
extern const rgb_settings_t default_orange;   // Cyberpunk orange color
extern const rgb_settings_t default_green;    // Cyberpunk green color
extern const rgb_settings_t default_blue;     // Cyberpunk blue color

extern const rgb_settings_t colors[];            // Array of colors
extern int                  current_color_index; // Index of the current color
extern rgb_settings_t       default_setting;     // Default RGB settings

// function declarations

// Set the RGB settings using appropriate QMK functions
void set_rgb_settings(rgb_settings_t setting);

// Save the RGB settings to EEPROM
void save_rgb_settings(rgb_settings_t setting);

// Load the default RGB settings from EEPROM
// Set to the default settings
void load_rgb_settings(void);

// Cycle through the colors upwards
// The colors are defined in the colors array
// Increase the current_color_index to get the next color
// Set this next color as static RGB color
void cycle_color_up(void);

// Cycle through the colors downwards
// The colors are defined in the colors array
// Decrease the current_color_index to get the previous color
// Set this previous color as static RGB color
void cycle_color_down(void);

// Set the current RGB color as the default color
// Saves the current RGB settings to EEPROM
void set_current_rgb_as_default(void);
