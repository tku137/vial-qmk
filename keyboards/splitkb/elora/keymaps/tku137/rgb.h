#pragma once

#include QMK_KEYBOARD_H

#include <stdint.h>

// struct to hold HSV color values
typedef struct {
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
    uint8_t mode;
    uint8_t speed;
} rgb_settings_t;

// Define some nice colors
extern const rgb_settings_t default_sand1;
extern const rgb_settings_t default_sand2;
extern const rgb_settings_t default_white;
extern const rgb_settings_t default_turquise;
extern const rgb_settings_t default_purple;
extern const rgb_settings_t default_orange;
extern const rgb_settings_t default_green;
extern const rgb_settings_t default_blue;

extern const rgb_settings_t colors[];
extern int                  current_color_index;
extern rgb_settings_t       default_setting;

// function declarations
void set_rgb_settings(rgb_settings_t setting);

void save_rgb_settings(rgb_settings_t setting);

void load_rgb_settings(void);

void cycle_color_up(void);

void cycle_color_down(void);

void set_current_rgb_as_default(void);
