/* Copyright 2023 splitkb.com <support@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// Myriad boilerplate
#define MYRIAD_ENABLE

// Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed
#define DEBOUNCE 5

/// Vial-specific settings

// Increase the EEPROM size for layout options
#define VIA_EEPROM_LAYOUT_OPTIONS_SIZE 2

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES

// Default is 2, needed because keymap has 7 layers
#define DYNAMIC_KEYMAP_LAYER_COUNT 6

#define VIAL_KEYBOARD_UID {0xB3, 0x8D, 0x94, 0xDA, 0xB3, 0xD7, 0xDC, 0x3D}

#define VIAL_UNLOCK_COMBO_ROWS { 3, 9 }
#define VIAL_UNLOCK_COMBO_COLS { 2, 5 }


// Fix idle right half
// #define SPLIT_USB_DETECT
// #define SPLIT_USB_TIMEOUT 10000
// #define SPLIT_USB_TIMEOUT_POLL 10
// #define SPLIT_WATCHDOG_ENABLE
// #define EE_HANDS
// #define MASTER_LEFT


// enable information transmitting
// #define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE
// #define SPLIT_WPM_ENABLE
#define SPLIT_OLED_ENABLE
#define SPLIT_MODS_ENABLE

#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define POINTING_DEVICE_TASK_THROTTLE_MS 8
#define PIMORONI_TRACKBALL_DEBOUNCE_CYCLES 10
#define POINTING_DEVICE_INVERT_X_RIGHT
#define POINTING_DEVICE_INVERT_Y_RIGHT


// RGB magic
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
