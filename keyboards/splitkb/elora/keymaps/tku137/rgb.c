#include "rgb.h"

const rgb_settings_t default_sand1    = {25, 178, 255, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_sand2    = {30, 178, 230, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_white    = {32, 102, 255, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_turquise = {118, 183, 209, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_purple   = {212, 255, 128, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_orange   = {27, 255, 255, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_green    = {78, 235, 255, RGB_MATRIX_SOLID_COLOR, 50};
const rgb_settings_t default_blue     = {146, 255, 255, RGB_MATRIX_SOLID_COLOR, 50};

rgb_settings_t default_setting;
int            current_color_index = 0;

// Define array of colors
const rgb_settings_t colors[] = {default_sand1, default_sand2, default_white, default_turquise, default_purple, default_orange, default_green, default_blue};
#define NUM_COLORS (sizeof(colors) / sizeof(rgb_settings_t))

// Custom RGB underglow functions
void set_rgb_settings(rgb_settings_t settings) {
    rgb_matrix_sethsv(settings.hue, settings.sat, settings.val);
    rgb_matrix_mode(settings.mode);
    rgb_matrix_set_speed(settings.speed);
}

void save_rgb_settings(rgb_settings_t settings) {
    eeprom_update_block((const void *)&settings, (void *)EEPROM_RGB_ADDR, sizeof(rgb_settings_t));
}

void load_rgb_settings(void) {
    rgb_settings_t settings;
    eeprom_read_block((void *)&settings, (const void *)EEPROM_RGB_ADDR, sizeof(rgb_settings_t));
    default_setting = settings; // Set loaded settings as default
}

void cycle_color_up(void) {
    current_color_index      = (current_color_index + 1) % NUM_COLORS;
    rgb_settings_t new_color = colors[current_color_index];
    rgb_matrix_sethsv(new_color.hue, new_color.sat, new_color.val);
}

void cycle_color_down(void) {
    if (current_color_index == 0) current_color_index = NUM_COLORS;
    current_color_index--;
    rgb_settings_t new_color = colors[current_color_index];
    rgb_matrix_sethsv(new_color.hue, new_color.sat, new_color.val);
}

void set_current_rgb_as_default(void) {
    rgb_settings_t current_settings = {.hue = rgb_matrix_get_hue(), .sat = rgb_matrix_get_sat(), .val = rgb_matrix_get_val(), .mode = rgb_matrix_get_mode(), .speed = rgb_matrix_get_speed()};
    default_setting                 = current_settings;
    save_rgb_settings(current_settings);
}
