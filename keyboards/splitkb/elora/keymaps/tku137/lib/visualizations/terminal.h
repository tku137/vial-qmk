#pragma once

#include QMK_KEYBOARD_H

#include <stdint.h>


void render_layer_status_terminal(void);
void render_mod_status_terminal(uint8_t modifiers);
void render_led_status_terminal(void);
void render_terminal(void);
