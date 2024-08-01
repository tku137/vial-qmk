#include <stdbool.h>
#include <stdint.h>

#include "layers.h"
#include "cyberdeck.h"

// Top decoration, Elora banner
void render_top_deco(void) {
    oled_set_cursor(0, 0);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _COLEMAK_DH:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _NAV:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _SYM:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _FUNCTION:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _ADJUST:
            oled_write_raw_P(top, sizeof(top));
            break;
        case _GAMING:
            oled_write_raw_P(top_gaming, sizeof(top_gaming));
            break;
        case _GAMING_EXT:
            oled_write_raw_P(top_gaming, sizeof(top_gaming));
            break;
        default:
            oled_write_P(PSTR("Undefined "), false);
    }
}
// Host keyboard layer status
void render_layer_status(uint8_t start_row) {
    oled_set_cursor(0, start_row);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_raw_P(qwerty, sizeof(qwerty));
            break;
        case _COLEMAK_DH:
            oled_write_raw_P(colemak_dh, sizeof(colemak_dh));
            break;
        case _NAV:
            oled_write_raw_P(navigation, sizeof(navigation));
            break;
        case _SYM:
            oled_write_raw_P(symbol, sizeof(symbol));
            break;
        case _FUNCTION:
            oled_write_raw_P(function, sizeof(function));
            break;
        case _ADJUST:
            oled_write_raw_P(adjust, sizeof(adjust));
            break;
        case _GAMING:
            oled_write_raw_P(gaming_base, sizeof(gaming_base));
            break;
        case _GAMING_EXT:
            oled_write_raw_P(gaming_ext, sizeof(gaming_ext));
            break;
        default:
            oled_write_P(PSTR("Undefined "), false);
    }
}

// Host Keyboard LED Status
void render_led_status(uint8_t start_row) {
    oled_set_cursor(0, start_row);
    led_t led_usb_state = host_keyboard_led_state();
    if (led_usb_state.caps_lock) {
        oled_write_raw_P(caps_on, sizeof(caps_on));
    } else {
        oled_write_raw_P(caps_off, sizeof(caps_off));
    }
}

void render_modifier_cmd_opt(uint8_t mods) {
    if (mods & MOD_MASK_GUI) {
        if (mods & MOD_MASK_ALT) {
            oled_write_raw_P(cmd_on_opt_on, sizeof(cmd_on_opt_on));
        } else {
            oled_write_raw_P(cmd_on_opt_off, sizeof(cmd_on_opt_off));
        }
    } else {
        if (mods & MOD_MASK_ALT) {
            oled_write_raw_P(cmd_off_opt_on, sizeof(cmd_off_opt_on));
        } else {
            oled_write_raw_P(cmd_off_opt_off, sizeof(cmd_off_opt_off));
        }
    }
}

void render_modifier_ctrl_shift(uint8_t mods) {
    if (mods & MOD_MASK_SHIFT) {
        if (mods & MOD_MASK_CTRL) {
            oled_write_raw_P(ctrl_on_shift_on, sizeof(ctrl_on_shift_on));
        } else {
            oled_write_raw_P(ctrl_off_shift_on, sizeof(ctrl_off_shift_on));
        }
    } else {
        if (mods & MOD_MASK_CTRL) {
            oled_write_raw_P(ctrl_on_shift_off, sizeof(ctrl_on_shift_off));
        } else {
            oled_write_raw_P(ctrl_off_shift_off, sizeof(ctrl_off_shift_off));
        }
    }
}

void render_modifiers(uint8_t start_row) {
    // Get the current modifiers
    uint8_t modifiers = get_mods() | get_oneshot_mods();

    // render first row of modifiers
    oled_set_cursor(0, start_row);
    render_modifier_cmd_opt(modifiers);

    // render second row of modifiers
    oled_set_cursor(0, start_row + 2); // Offset for the second row
    render_modifier_ctrl_shift(modifiers);
}

// Render OLED display parts
void render_cyberdeck(void) {
    // Part 1: Top decoration, Elora banner
    render_top_deco();

    // Part 2: Layer status
    render_layer_status(4);

    // Part 3: Spacer between layer status and modifier status
    oled_set_cursor(0, 6);
    oled_write_raw_P(spacer1, sizeof(spacer1));

    // Part 4: Modifier status
    render_modifiers(7);

    // Part 5: Spacer between modifier status and LED status
    oled_set_cursor(0, 11);
    oled_write_raw_P(spacer2, sizeof(spacer2));

    // Part 6: LED status
    render_led_status(13);

    // Part 7: Bottom decoration, tku137
    oled_set_cursor(0, 14);
    oled_write_raw_P(tku137, sizeof(tku137));
}

// WPM-responsive animation rendering

// Draw a column on the OLED buffer
void oled_draw_column(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool fill) {
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            oled_write_pixel(x + j, y - i, fill);
        }
    }
}

// Constants for the WPM-based columns
bool     display_wpm_mode       = false;
uint32_t wpm_display_start_time = 0;

// Function to add wobble effect to columns at peak performance
void add_peak_performance_wobble(uint8_t *column_start_xs, uint8_t *column_start_y, uint16_t wpm) {
    if (wpm >= target_wpm) {
        // Apply a random wobble for peak performance
        for (int i = 0; i < 3; ++i) {
            int wobble_x = (rand() % (PEAK_WOBBLE_INTENSITY * 2 + 1)) - PEAK_WOBBLE_INTENSITY;
            column_start_xs[i] += wobble_x;
        }
        int wobble_y = (rand() % (PEAK_WOBBLE_INTENSITY * 2 + 1)) - PEAK_WOBBLE_INTENSITY;
        *column_start_y += wobble_y;
    }
}

void display_target_wpm_as_images(uint16_t target_wpm) {
    char wpm_str[5]; // Buffer for the target WPM number, assuming it won't exceed 4 digits
    snprintf(wpm_str, sizeof(wpm_str), "%u", target_wpm);

    int num_digits       = strlen(wpm_str); // Get the number of digits in the target WPM
    int num_empty_digits = 6 - num_digits;  // Calculate how many empty digits we need

    // First, display the empty digits
    for (int i = 0; i < num_empty_digits; i++) {
        oled_set_cursor(i, 8);                          // Set cursor for each empty digit
        oled_write_raw_P((const char *)digit_empty, 6); // Assume digit_empty is 6 bytes
    }

    // Then, display the actual WPM digits
    for (int i = 0; i < num_digits; i++) {
        const uint8_t *digit_bitmap = NULL;

        // Determine which digit bitmap to use
        switch (wpm_str[i]) {
            case '0':
                digit_bitmap = digit_0;
                break;
            case '1':
                digit_bitmap = digit_1;
                break;
            case '2':
                digit_bitmap = digit_2;
                break;
            case '3':
                digit_bitmap = digit_3;
                break;
            case '4':
                digit_bitmap = digit_4;
                break;
            case '5':
                digit_bitmap = digit_5;
                break;
            case '6':
                digit_bitmap = digit_6;
                break;
            case '7':
                digit_bitmap = digit_7;
                break;
            case '8':
                digit_bitmap = digit_8;
                break;
            case '9':
                digit_bitmap = digit_9;
                break;
        }

        // If we have a valid bitmap for the digit, draw it
        if (digit_bitmap != NULL) {
            oled_set_cursor(num_empty_digits + i, 8);        // Adjust cursor for each digit
            oled_write_raw_P((const char *)digit_bitmap, 6); // Draw the digit bitmap
        }
    }
}

// Constants for column configuration
const uint8_t COLUMN_WIDTHS[]   = {COLUMN1_END_X - COLUMN1_START_X, COLUMN2_END_X - COLUMN2_START_X, COLUMN3_END_X - COLUMN3_START_X};
const uint8_t COLUMN_START_XS[] = {COLUMN1_START_X, COLUMN2_START_X, COLUMN3_START_X};

// Drawing function for WPM-based columns
void draw_wpm_columns(uint16_t wpm) {
    // Show the current WPM on the display for 3 seconds when updating the target WPM
    if (display_wpm_mode) {
        // Check if 3 seconds have passed since the WPM mode was activated
        if (timer_elapsed32(wpm_display_start_time) > 3000) {
            display_wpm_mode = false; // Revert to column animation mode
        } else {
            // Display the current TARGET_WPM and exit the function early
            display_target_wpm_as_images(target_wpm);
            return;
        }
    }

    // Prepare temporary variables for the adjusted column positions
    uint8_t temp_column_start_xs[3] = {COLUMN_START_XS[0], COLUMN_START_XS[1], COLUMN_START_XS[2]};
    uint8_t temp_column_start_y     = COLUMN_START_Y;

    // Add the peak performance wobble if necessary
    add_peak_performance_wobble(temp_column_start_xs, &temp_column_start_y, wpm);

    // Calculate the height of the column based on the current WPM
    float   height_ratio   = (float)wpm / target_wpm;
    uint8_t dynamic_height = (uint8_t)((MAX_COLUMN_HEIGHT - BASE_HEIGHT) * height_ratio);

    // Ensure the dynamic height does not exceed MAX_COLUMN_HEIGHT
    dynamic_height = MIN(dynamic_height, MAX_COLUMN_HEIGHT);

    // Calculate dynamic wobble effect
    uint8_t max_wobble = wpm / WPM_WOBBLE_RATIO;
    max_wobble         = MIN(max_wobble, MAX_WOBBLE_HEIGHT); // Clamp the value to the maximum wobble height

    // Apply the wobble effect and draw the columns
    for (int i = 0; i < 3; ++i) {
        // Clear the column at the specified location
        oled_draw_column(temp_column_start_xs[i], temp_column_start_y, COLUMN_WIDTHS[i], MAX_COLUMN_HEIGHT, false);

        // Adjust wobble for each column
        int wobble = (rand() % (max_wobble * 2 + 1)) - max_wobble;

        // Calculate and clamp the column height
        uint8_t column_height = BASE_HEIGHT + dynamic_height + wobble;
        column_height         = MIN(MAX(column_height, BASE_HEIGHT), MAX_COLUMN_HEIGHT);

        // Draw the column at the specified location with calculated height and width
        oled_draw_column(temp_column_start_xs[i], temp_column_start_y, COLUMN_WIDTHS[i], column_height, true);
    }

    // Push the buffer to the OLED
    oled_render();
}

// Function to call from the main loop
void render_wpm_columns_animation(void) {
    // get the current words per minute
    uint16_t current_wpm = get_current_wpm();

    // render the static background image
    oled_write_raw_P(wpm_background, sizeof(wpm_background));

    // render the dynamic columns based on the current WPM
    draw_wpm_columns(current_wpm);
}

// Main render functions
// These functions are called in the `oled_task_user` function
// They are meant as an interface to minimize maintenance when switching to
// other oled libraries or display types

// Global states used in renders
// For pixel blinking effect
static uint32_t last_blink_time = 0;
static bool     blink_state     = false;

// Render the master OLED
void render_master(void) {
    // show cyberdeck visualisation
    render_cyberdeck();

    // let some pixels blink
    if (timer_elapsed32(last_blink_time) >= BLINK_INTERVAL) {
        last_blink_time = timer_read32(); // Reset the timer
        blink_state     = !blink_state;   // Toggle the blink state
    }
    oled_write_pixel(58, 123, blink_state);
    oled_write_pixel(60, 123, blink_state);
    oled_write_pixel(62, 123, blink_state);

    oled_render();
}

// Render the slave OLED
void render_slave(void) {
    // render the WPM columns animation
    render_wpm_columns_animation();

    // let some pixels blink
    if (timer_elapsed32(last_blink_time) >= BLINK_INTERVAL) {
        last_blink_time = timer_read32(); // Reset the timer
        blink_state     = !blink_state;   // Toggle the blink state
    }
    oled_write_pixel(1, 123, blink_state);
    oled_write_pixel(3, 123, blink_state);
    oled_write_pixel(5, 123, blink_state);

    oled_render();
}
