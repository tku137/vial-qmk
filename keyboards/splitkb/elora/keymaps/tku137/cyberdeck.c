#include <stdbool.h>
#include <stdint.h>

#include "layers.h"
#include "cyberdeck.h"

// Host keyboard layer status
void render_layer_status(uint8_t start_row) {
    oled_set_cursor(0, start_row);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_raw_P(base, sizeof(base));
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
    oled_write_raw_P(top, sizeof(top));

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

// Function to calculate animation frame duration based on WPM
uint16_t calculate_frame_duration(uint16_t wpm) {
    // Define ranges and corresponding frame durations
    if (wpm > FAST_ANIM_SPEED) {
        return FAST_ANIM_FRAME_DURATION; // Faster animation for high WPM
    } else if (wpm > NORMAL_ANIM_SPEED) {
        return NORMAL_ANIM_FRAME_DURATION; // Medium animation speed
    } else if (wpm > SLOW_ANIM_SPEED) {
        return SLOW_ANIM_FRAME_DURATION; // Slower animation for low WPM
    } else {
        return IDLE_ANIM_FRAME_DURATION; // Idle or very slow typing speed
    }
}

// Main render function for WPM-based animation
void render_wpm_based_animation(void) {
    static uint32_t anim_timer    = 0;
    static uint8_t  current_frame = 0;

    uint8_t frame_count;
    const char(*anim_frames)[ANIM_SIZE];
    uint16_t current_wpm    = get_current_wpm();
    uint16_t frame_duration = calculate_frame_duration(current_wpm);

    if (current_wpm > FAST_ANIM_SPEED) {
        frame_count = FAST_ANIM_FRAMES;
        anim_frames = fast_frames;
    } else if (current_wpm > NORMAL_ANIM_SPEED) {
        frame_count = NORMAL_ANIM_FRAMES;
        anim_frames = normal_frames;
    } else if (current_wpm > SLOW_ANIM_SPEED) {
        frame_count = SLOW_ANIM_FRAMES;
        anim_frames = slow_frames;
    } else {
        frame_count = IDLE_ANIM_FRAMES;
        anim_frames = idle_frames;
    }

    if (timer_elapsed32(anim_timer) > frame_duration) {
        anim_timer = timer_read32();
        oled_write_raw_P(anim_frames[current_frame], ANIM_SIZE);
        current_frame = (current_frame + 1) % frame_count;
    }
}

// Draw a column on the OLED buffer
void oled_draw_column(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            oled_write_pixel(x + j, y + i, true);
        }
    }
}

// Drawing function for WPM-based columns
void draw_wpm_columns(uint16_t wpm) {
    // Constants for column widths, heights, and spacing
    const uint8_t column_width      = (OLED_HEIGHT / 3) - COLUMN_SPACING;
    const uint8_t max_column_height = OLED_WIDTH - BASE_HEIGHT;

    // Calculate base heights for the columns based on WPM
    uint8_t column_heights[3];
    for (int i = 0; i < 3; ++i) {
        column_heights[i] = BASE_HEIGHT + (wpm / WPM_HEIGHT_RATIO);
    }

    // Add wobble effect by adding or subtracting a small value to the column heights
    for (int i = 0; i < 3; ++i) {
        int wobble = rand() % MAX_WOBBLE_HEIGHT;
        column_heights[i] += wobble;
        column_heights[i] = column_heights[i] > max_column_height ? max_column_height : column_heights[i];
    }

    // Clear OLED buffer
    oled_clear();

    // Draw the columns with current heights
    for (int i = 0; i < 3; ++i) {
        oled_draw_column(COLUMN_START_X + (i * (column_width + COLUMN_SPACING)), COLUMN_START_Y, column_width, column_heights[i]);
    }

    // Push the buffer to the OLED
    oled_render();
}

// Function to call from the main loop
void render_wpm_columns_animation(void) {
    uint16_t current_wpm = get_current_wpm();
    draw_wpm_columns(current_wpm);
}

// Main render functions
// These functions are called in the `oled_task_user` function
// They are meant as an interface to minimize maintenance when switching to
// other oled libraries or display types
void render_master(void) {
    // render_cyberdeck();
    render_wpm_columns_animation();
}

void render_slave(void) {
    // render_wpm_based_animation();
    render_wpm_columns_animation();
}
