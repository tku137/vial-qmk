#include <stdbool.h>
#include <stdint.h>

#include "layers.h"
#include "terminal.h"


void render_layer_status_terminal(void) {
// Host Keyboard Layer Status
// oled_write_P(PSTR("Layer:\n"), false);
switch (get_highest_layer(layer_state  | default_layer_state)) {
    case _QWERTY:
        oled_write_P(PSTR("<>        "), false);
        oled_write_P(PSTR("QWERTY    "), false);
        break;
    case _SYM:
        oled_write_P(PSTR("  <>      "), false);
        oled_write_P(PSTR("Symbols   "), false);
        break;
    case _NAV:
        oled_write_P(PSTR("    <>    "), false);
        oled_write_P(PSTR("Navigation"), false);
        break;
    case _FUNCTION:
        oled_write_P(PSTR("      <>  "), false);
        oled_write_P(PSTR("F-Keys    "), false);
        break;
    case _ADJUST:
        oled_write_P(PSTR("        <>"), false);
        oled_write_P(PSTR("Adjust    "), false);
        break;
    default:
        oled_write_P(PSTR("Undefined "), false);
    }
}

void render_mod_status_terminal(uint8_t modifiers) {
    // Modifiers can be displayed with text for simplicity
    // oled_write_P(PSTR("Mods:\n"), false);

    if (modifiers & MOD_MASK_GUI) {
        oled_write_P(PSTR(" CMD "), true);
    } else {
        oled_write_P(PSTR(" CMD "), false);
    }
    if (modifiers & MOD_MASK_ALT) {
        oled_write_P(PSTR(" OPT "), true);
    } else {
        oled_write_P(PSTR(" OPT "), false);
    }
    oled_write_P(PSTR("\n"), false);
    if (modifiers & MOD_MASK_CTRL) {
        oled_write_P(PSTR(" CTL "), true);
    } else {
        oled_write_P(PSTR(" CTL "), false);
    }
    if (modifiers & MOD_MASK_SHIFT) {
        oled_write_P(PSTR(" SFT "), true);
    } else {
        oled_write_P(PSTR(" SFT "), false);
    }
}

void render_led_status_terminal(void) {
    //Host Keyboard LED Status
    led_t led_usb_state = host_keyboard_led_state();
    if (led_usb_state.caps_lock) {
        oled_write_P(PSTR("Caps: ON  "), true);
    } else {
        oled_write_P(PSTR("Caps: off "), false);
    }
}

void render_terminal(void) {

    oled_write_P(PSTR("~]$ status"), false);
    oled_write_P(PSTR("          "), false);
    oled_write_P(PSTR("Elora rev1"), false);
    oled_write_P(PSTR("----------"), false);
    render_layer_status_terminal();
    oled_write_P(PSTR("          "), false);
    oled_write_P(PSTR("Mods:     "), false);
    oled_write_P(PSTR("          "), false);
    render_mod_status_terminal(get_mods() | get_oneshot_mods());
    oled_write_P(PSTR("          "), false);
    render_led_status_terminal();
    oled_write_P(PSTR("          "), false);
    oled_write_P(PSTR("~]$ _"), false);
}


