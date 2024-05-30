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

#include "keyboard.h"
#include "oled_driver.h"
#include QMK_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "transactions.h"
#include "timer.h"

#include "layers.h"
#include "rgb.h"
#include "cyberdeck.h"
// #include "terminal.h"
#include "bme680_integration.c"

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define SFT_ESC MT(MOD_LSFT, KC_ESC)
#define SFT_QUOT MT(MOD_RSFT, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)
#define ALT_ESC MT(MOD_LALT, KC_ESC)
#define GUI_ENT MT(MOD_LGUI, KC_ENT)
#define GUI_TAB MT(MOD_RGUI, KC_TAB)

// App specific shortcuts
#define MICMUTE HYPR(KC_M)
#define KEYCAST LCAG(KC_K)

// Special character modes from EurKey
// This requires EurKey to be installed on the host computer!
#define EURO RALT(KC_5)
#define GREEK RALT(KC_M)
#define DEG RALT(KC_SCLN)
#define ESSZ RALT(KC_S)

// Define custom keycodes
enum custom_keycodes {
    WPM_UP = SAFE_RANGE, // Custom keycode for increasing target WPM
    WPM_DOWN,            // Custom keycode for decreasing target WPM
    DFLT,                // Custom keycode for natural white
    WHT,                 // Custom keycode for white
    RNBW,                // Custom keycode for rainbow swirl
    RGB_UP,              // Custom keycode for cycling RGB mode
    RGB_DWN,             // Custom keycode for cycling RGB mode
    RGB_SAV,             // Custom keycode for setting current color as default
};

// Initialize WPM variables
uint16_t        target_wpm         = DEFAULT_WPM; // Initialization with default value
static bool     target_wpm_changed = false;       // Flag to indicate if target_wpm has changed
static uint32_t last_sync_time     = 0;

// This is sent to the slave side
typedef struct _master_to_slave_t {
    uint16_t target_wpm;             // Target WPM value
    bool     display_wpm_mode;       // Indicates if display mode is active
    uint32_t wpm_display_start_time; // Timestamp for the display start time
} master_to_slave_t;

// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |      |LShift|  |RShift|      |   6  |   7  |   8  |   9  |   0  |  -  _  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |      |LCtrl |  | RCtrl|      |   Y  |   U  |   I  |   O  |   P  |  \  |  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | Sft/Esc|   A  |   S  |   D  |   F  |   G  |      | LAlt |  | RAlt |      |   H  |   J  |   K  |   L  | ;  : | Sft/' "|
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | LCtrl  |   Z  |   X  |   C  |   V  |   B  | [ {  |  (   |  |   )  |  ] } |   N  |   M  | ,  < | . >  | /  ? | RCtrl  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LAlt/| LGUI/| Space| Nav  |  | Sym  | Bksp | RGUI/| RAlt |F-Keys|
 *                        |      | Esc  | Enter|      |      |  |      |      | Tab  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | Prev | Next | Pause | Stop |      | Mute |                 | Prev | Next | Pause | Stop |      | Mute |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_QWERTY] = LAYOUT_myr(
      KC_ESC  ,  KC_1  ,  KC_2  ,  KC_3  ,  KC_4  ,  KC_5  ,          KC_LSFT,     KC_RSFT,           KC_6  ,  KC_7  ,  KC_8  ,  KC_9  ,  KC_0  , KC_MINS,
      KC_TAB  ,  KC_Q  ,  KC_W  ,  KC_E  ,  KC_R  ,  KC_T  ,          KC_LCTL,     KC_RCTL,           KC_Y  ,  KC_U  ,  KC_I  ,  KC_O  ,  KC_P  , KC_BSLS,
      SFT_ESC ,  KC_A  ,  KC_S  ,  KC_D  ,  KC_F  ,  KC_G  ,          KC_LALT,     KC_RALT,           KC_H  ,  KC_J  ,  KC_K  ,  KC_L  , KC_SCLN,SFT_QUOT,
      KC_LCTL ,  KC_Z  ,  KC_X  ,  KC_C  ,  KC_V  ,  KC_B  , KC_LBRC, KC_LPRN,     KC_RPRN, KC_RBRC,  KC_N  ,  KC_M  , KC_COMM, KC_DOT , KC_SLSH, KC_RCTL,
                                  ADJUST , ALT_ESC, GUI_ENT, KC_SPC , NAV    ,      SYM   , KC_BSPC, GUI_TAB, KC_RALT,  FKEYS ,

      KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE,                            KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE
    ),

/*
 * Base Layer: Colemak DH
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |      |LShift|  |RShift|      |   6  |   7  |   8  |   9  |   0  |  -  _  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   F  |   P  |   B  |      |LCtrl |  | RCtrl|      |   J  |   L  |   U  |   Y  | ;  : |  \  |  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | Sft/Esc|   A  |   R  |   S  |   T  |   G  |      | LAlt |  | RAlt |      |   M  |   N  |   E  |   I  |   O  | Sft/' "|
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | LCtrl  |   Z  |   X  |   C  |   D  |   V  | [ {  |  (   |  |   )  |  ] } |   K  |   H  | ,  < | . >  | /  ? | RCtrl |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LAlt/| LGUI/| Space| Nav  |  | Sym  | Bksp | RGUI/| RAlt |F-Keys|
 *                        |      | Esc  | Enter|      |      |  |      |      | Tab  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | Prev | Next | Pause | Stop |      | Mute |                 | Prev | Next | Pause | Stop |      | Mute |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_COLEMAK_DH] = LAYOUT_myr(
      KC_ESC  ,  KC_1  ,  KC_2  ,  KC_3  ,  KC_4  ,  KC_5  ,          KC_LSFT,     KC_RSFT,           KC_6  ,  KC_7  ,  KC_8  ,  KC_9  ,  KC_0  , KC_MINS,
      KC_TAB  ,  KC_Q  ,  KC_W  ,  KC_F  ,  KC_P  ,  KC_B  ,          KC_LCTL,     KC_RCTL,           KC_J  ,  KC_L  ,  KC_U  ,  KC_Y  , KC_SCLN, KC_BSLS,
      SFT_ESC ,  KC_A  ,  KC_R  ,  KC_S  ,  KC_T  ,  KC_G  ,          KC_LALT,     KC_RALT,           KC_M  ,  KC_N  ,  KC_E  ,  KC_I  ,  KC_O  ,SFT_QUOT,
      KC_LCTL ,  KC_Z  ,  KC_X  ,  KC_C  ,  KC_D  ,  KC_V  , KC_LBRC, KC_LPRN,     KC_RPRN, KC_RBRC,  KC_K  ,  KC_H  , KC_COMM, KC_DOT , KC_SLSH, KC_RCTL,
                                  ADJUST , ALT_ESC, GUI_ENT, KC_SPC , NAV    ,      SYM   , KC_BSPC, GUI_TAB, KC_RALT,  FKEYS ,

      KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE,                            KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE
    ),

/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |      |M Next|M Play|M Prev| VolUp| Delete |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        | Ctrl | Alt  | GUI  | Shift| Hyper|      |      |  |      |      | PgUp |  ←   |   ↑  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |MicMute|     |  |      |      | PgDn | Home |   ↓  | End  |VolMut|MicMute |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | Prev | Next | Pause | Stop |      | Mute |                 | Prev | Next | Pause | Stop |      | Mute |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_NAV] = LAYOUT_myr(
      _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______, _______,          _______, _______,          _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_VOLU, KC_DEL ,
      _______, KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_HYPR,          _______, _______,          KC_PGUP, KC_LEFT, KC_UP  , KC_RGHT, KC_VOLD, KC_INS ,
      _______, _______, _______, _______, _______, _______, MICMUTE, _______, _______, _______, KC_PGDN, KC_HOME, KC_DOWN, KC_END , KC_MUTE, MICMUTE,
                                 _______, _______, _______, _______, _______, _______, KC_DEL , _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  °     |  ^   |  <   |  >   |  "   |  '   |      |      |  |      |      |  &   |  [   |  ]   |  `   |  %   |  €     |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  |     |  !   |  -   |  +   |  =   |  ,   |      |      |  |      |      |  #   |  (   |  )   |  :   |  ?   |  $     |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |  ~   |  /   |  *   |  \   |  .   |      |      |  |      |      |  _   |  {   |  }   |  ;   |  @   | Greek> |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_SYM] = LAYOUT_myr(
      _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
       DEG   , KC_CIRC, KC_LABK, KC_RABK, KC_DQUO, KC_QUOT,          _______, _______,          KC_AMPR, KC_LBRC, KC_RBRC, KC_GRV , KC_PERC, EURO   ,
      KC_PIPE, KC_EXLM, KC_MINS, KC_PLUS, KC_EQL , KC_COMM,          _______, _______,          KC_HASH, KC_LPRN, KC_RPRN, KC_COLN, KC_AT  , KC_DLR ,
      _______, KC_TILD, KC_SLSH, KC_ASTR, KC_BSLS, KC_DOT , _______, _______, _______, _______, KC_UNDS, KC_LCBR, KC_RCBR, KC_SCLN, KC_QUES, GREEK  ,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |PrtSc | Num  | Caps | Scroll |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |KeyCst| F1   | F2   | F3   | F4   |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        | Ctrl | Alt  | GUI  | Shift| Hyper|      |      |      |  |      |      | F5   | F6   | F7   | F8   |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      | LPad |MContr|  |MContr| LPad |      | F9   | F10  | F11  | F12  |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_FUNCTION] = LAYOUT_myr(
      _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, KC_PSCR, KC_NUM , KC_CAPS, KC_SCRL,
      _______, _______, _______, _______, _______, _______,          _______, _______,          KEYCAST,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______,
      _______, KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_HYPR,          _______, _______,          _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,
      _______, _______, _______, _______, _______, _______, KC_LPAD, KC_MCTL, KC_MCTL, KC_LPAD, _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |EEclr |      |      |  |      |      |RGBtog|RGBsav|      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |QWERTY|Brite+| WPM+ |      |      |  |      |      | Dflt | White| Rnbw |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |COLEMA|Brite-| WPM- |      |      |  |      |      | RGBup| HUI  | SAI  | VAI  | SPI  |  MOD   |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |RGBdwn| HUD  | SAD  | VAD  | SPD  |  RMOD  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_ADJUST] = LAYOUT_myr(
      _______, _______, _______, _______, _______, EE_CLR ,         _______, _______,          RGB_TOG, RGB_SAV, _______, _______, _______, _______,
      _______, _______, _______, QWERTY , KC_BRIU, WPM_UP ,         _______, _______,           DFLT  ,  WHT   ,  RNBW  , _______, _______, _______,
      _______, _______, _______, COLEMAK, KC_BRID,WPM_DOWN,         _______, _______,          RGB_UP , RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI , RGB_MOD,
      _______, _______, _______, _______, _______, _______,_______, _______, _______, _______, RGB_DWN, RGB_HUD, RGB_SAD, RGB_VAD, RGB_SPD ,RGB_RMOD,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______

    ),

};

/* The default OLED and rotary encoder code can be found at the bottom of qmk_firmware/keyboards/splitkb/elora/rev1/rev1.c
 * These default settings can be overriden by your own settings in your keymap.c
 * DO NOT edit the rev1.c file; instead override the weakly defined default functions by your own.
 */

/* DELETE THIS LINE TO UNCOMMENT (1/2)
#ifdef OLED_ENABLE
bool oled_task_user(void) {
  // Your code goes here
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
  // Your code goes here
}
#endif
DELETE THIS LINE TO UNCOMMENT (2/2) */


// Vial-specific encoder code

bool encoder_update_user(uint8_t index, bool clockwise) {
  return false;
}

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [1] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [2] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [3] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [4] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [5] = {
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
      ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
};


// This function is called when the slave receives data from the master
void target_wpm_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    if (in_buflen == sizeof(master_to_slave_t)) {
        master_to_slave_t m2s; // Create a local instance of the struct
        memcpy(&m2s, in_data, sizeof(m2s)); // Safely copy data into the struct

        // Use the struct's data to update the slave's variables
        target_wpm = m2s.target_wpm;
        display_wpm_mode = m2s.display_wpm_mode;
        wpm_display_start_time = m2s.wpm_display_start_time;

        // Save the updated TARGET_WPM to EEPROM
        eeprom_update_word(EEPROM_TARGET_WPM_ADDR, target_wpm);
    }
}


void housekeeping_task_user(void) {
    // Only proceed if we're on the keyboard master side and a change has been flagged
    // and also if enough time has passed since the last sync.
    // This minimizes the number of syncs and reduces the risk of data loss.
    if (is_keyboard_master() && target_wpm_changed && timer_elapsed32(last_sync_time) > SYNC_INTERVAL) {
        master_to_slave_t m2s_data = {
            .target_wpm = target_wpm,
            .display_wpm_mode = display_wpm_mode,
            .wpm_display_start_time = wpm_display_start_time
        };

        // Attempt to send the data to the slave.
        bool sent = transaction_rpc_send(TARGET_WPM_SYNC, sizeof(m2s_data), &m2s_data);
        if (sent) {
            // If data has been sent successfully, update the last sync time to the current time.
            last_sync_time = timer_read32();
            // Reset the change flag.
            target_wpm_changed = false;
        }
    }
}


void matrix_init_user(void) {
    bme680_init();
}


// This is run at boot
void keyboard_post_init_user(void) {

    // Load RGB settings from EEPROM
    load_rgb_settings();

    // Load TARGET_WPM from EEPROM
    target_wpm = eeprom_read_word(EEPROM_TARGET_WPM_ADDR);

    // Check for EEPROM unset or invalid values, adjust range as necessary
    if (target_wpm == 0xFFFF || target_wpm < MIN_WPM || target_wpm > MAX_WPM) {
        target_wpm = DEFAULT_WPM; // Default value if EEPROM is unset or value is out of valid range
    }

    // Register custom data sync handler
    transaction_register_rpc(TARGET_WPM_SYNC, target_wpm_sync_slave_handler);

}


// Custom keycode handling
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QWERTY:
            // Switch to the QWERTY layer persistently
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
            }
            return false;
        case COLEMAK:
            // Switch to the COLEMAK_DH layer persistently
            if (record->event.pressed) {
                set_single_persistent_default_layer(_COLEMAK_DH);
            }
            return false;  // Skip further processing of this key code
        case RNBW:
            // When the key is pressed, set the RGB matrix to rainbow swirl mode
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_RAINBOW_MOVING_CHEVRON);
                rgb_matrix_set_speed(10); // 0-255
                rgb_matrix_sethsv(default_setting.hue, 255, default_setting.val);
            }
            return false; // Skip further processing of this key
        case WHT:
            // When the key is pressed, set the RGB matrix to white color
            if (record->event.pressed) {
                rgb_matrix_sethsv(default_white.hue, default_white.sat, default_white.val); // Set all LEDs to natural white
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case DFLT:
            // When the key is pressed, set the RGB matrix to default color
            if (record->event.pressed) {
                set_rgb_settings(default_setting);
            }
            return false; // Skip further processing of this key
        case RGB_UP:
            // When the key is pressed, cycle through the colors
            if (record->event.pressed) {
                cycle_color_up();
            }
            return false; // Skip further processing of this key
        case RGB_DWN:
            // When the key is pressed, cycle through the colors
            if (record->event.pressed) {
                cycle_color_down();
            }
            return false; // Skip further processing of this key
        case RGB_SAV:
            // When the key is pressed, set the current color as the default color
            if (record->event.pressed) {
                set_current_rgb_as_default();
            }
            return false; // Skip further processing of this key
        case WPM_UP:
            if (record->event.pressed) {
                // Increase TARGET_WPM by WPM_INCREMENT with each key press
                // Prevent it from going below a minimum value
                if (target_wpm < MAX_WPM) { // Ensure TARGET_WPM stays positive or within a logical minimum
                    target_wpm += WPM_INCREMENT;
                    // Save the updated TARGET_WPM to EEPROM
                    eeprom_update_word(EEPROM_TARGET_WPM_ADDR, target_wpm);
                }
                target_wpm_changed = true; // Indicate that the target WPM has changed
                display_wpm_mode = true; // Activate display mode to show updated TARGET_WPM
                wpm_display_start_time = timer_read32(); // Capture the start time
            }
            return false; // Skip further processing to prevent default behavior
        case WPM_DOWN:
            if (record->event.pressed) {
                // Decrease TARGET_WPM by WPM_INCREMENT with each key press
                // Prevent it from going below a minimum value
                if (target_wpm > MIN_WPM) { // Ensure TARGET_WPM stays positive or within a logical minimum
                    target_wpm -= WPM_INCREMENT;
                    // Save the updated TARGET_WPM to EEPROM
                    eeprom_update_word(EEPROM_TARGET_WPM_ADDR, target_wpm);
                }
                target_wpm_changed = true; // Indicate that the target WPM has changed
                display_wpm_mode = true; // Activate display mode to show updated TARGET_WPM
                wpm_display_start_time = timer_read32(); // Capture the start time
            }
            return false;
        default:
            return true; // Process all other keycodes normally
    }
}


// OLED handling
bool oled_task_user(void) {

    if (is_keyboard_master()) {

        struct bme680_data sensor_data;
        int8_t rslt = bme680_read_data(&sensor_data);

        if (rslt == BME68X_OK) {
            char temp_str[8];
            char hum_str[8];
            char pres_str[12];
            char gas_str[12];

            snprintf(temp_str, sizeof(temp_str), "T:%d C", sensor_data.temperature);
            snprintf(hum_str, sizeof(hum_str), "H:%d %%", sensor_data.humidity);
            snprintf(pres_str, sizeof(pres_str), "P:%d hPa", sensor_data.pressure / 100);
            snprintf(gas_str, sizeof(gas_str), "P:%lu hPa", (unsigned long)sensor_data.gas_resistance);

            // Render OLED display with sensor data
            oled_set_cursor(0, 0);
            oled_write(temp_str, false);
            oled_set_cursor(0, 2);
            oled_write(hum_str, false);
            oled_set_cursor(0, 4);
            oled_write(pres_str, false);
            oled_set_cursor(0, 6);
            oled_write(gas_str, false);

        } else {
            oled_write("BME680 read failed", false);
        }

    } else {

        render_slave();

    }

    return false;
}

