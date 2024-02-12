#include <stdbool.h>
#include <stdint.h>

#include "layers.h"
#include "cyberdeck.h"


// Host keyboard layer status
void render_layer_status(uint8_t start_row) {
    oled_set_cursor(0, start_row);
    switch (get_highest_layer(layer_state  | default_layer_state)) {
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


//Host Keyboard LED Status
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
uint32_t anim_timer = 0;
uint8_t current_frame = 0;

void render_wpm_based_animation(void) {
    uint8_t frame_count;
    const char (*anim_frames)[ANIM_SIZE];

    if (get_current_wpm() > FAST_ANIM_SPEED) {
        frame_count = FAST_ANIM_FRAMES;
        anim_frames = fast_frames;
    } else if (get_current_wpm() > NORMAL_ANIM_SPEED) {
        frame_count = NORMAL_ANIM_FRAMES;
        anim_frames = normal_frames;
    } else if (get_current_wpm() > SLOW_ANIM_SPEED) {
        frame_count = SLOW_ANIM_FRAMES;
        anim_frames = slow_frames;
    } else {
        frame_count = IDLE_ANIM_FRAMES;
        anim_frames = idle_frames;
    }

    if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
        anim_timer = timer_read32();
        oled_write_raw_P(anim_frames[current_frame], ANIM_SIZE);
        current_frame = (current_frame + 1) % frame_count;
    }
}


void render_master(void) {
    render_cyberdeck();
}

void render_slave(void) {
    render_wpm_based_animation();
}
