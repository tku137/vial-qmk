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

#include QMK_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "layers.h"
#include "cyberdeck.h"
// #include "terminal.h"


#define CTL_ESC  MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT  MT(MOD_LALT, KC_ENT)

// App specific shortcuts
#define MICMUTE HYPR(KC_M)
#define KEYCAST LCAG(KC_K)

// Special character modes from EurKey
// This requires EurKey to be installed on the host computer!
#define EURO    RALT(KC_5)
#define GREEK   RALT(KC_M)
#define DEG     RALT(KC_SCLN)
#define ESSZ    RALT(KC_S)


// Default natural white color
typedef struct {
    uint16_t hue;
    uint8_t sat;
    uint8_t val;
} hsv_color_t;

const hsv_color_t default_color = {32, 102, 255}; // Define default color


// Define custom keycodes
enum custom_keycodes {
    NWHT = SAFE_RANGE, // Custom keycode for natural white
    TRQS,              // Custom keycode for cyberpunk turquoise
    PRPL,              // Custom keycode for cyberpunk purple
    ORNG,              // Custom keycode for cyberpunk orange
    NGRN,              // Custom keycode for cyberpunk green
    ELBL,              // Custom keycode for cyberpunk blue
    RNBW,              // Custom keycode for rainbow swirl
    SCR_TGL,           // Custom keycode for toggling scroll lock
    // Add other custom keycodes here, if any
};


// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |      |LShift|  |RShift|      |   6  |   7  |   8  |   9  |   0  |  Esc   |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |      |LCtrl |  | RCtrl|      |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |      | LAlt |  | RAlt |      |   H  |   J  |   K  |   L  | ;  : |Ctrl/' "|
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  |  (   |  |   )  |  ] } |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI | LAlt/| Space| Nav  |  | Sym  | Space| AltGr| RGUI |F-Keys|
 *                        |      |      | Enter|      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | Prev | Next | Pause | Stop |      | Mute |                 | Prev | Next | Pause | Stop |      | Mute |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_QWERTY] = LAYOUT_myr(
      KC_ESC  , KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,         KC_LSFT,     KC_RSFT,          KC_6 ,  KC_7 ,  KC_8 ,   KC_9 ,  KC_0 , KC_ESC,
      KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,         KC_LCTL,     KC_RCTL,          KC_Y ,  KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
      CTL_ESC , KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,         KC_LALT,     KC_RALT,          KC_H ,  KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
      KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC,KC_LPRN,     KC_RPRN, KC_RBRC, KC_N ,  KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                 ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV   ,     SYM    , KC_SPC ,KC_RALT, KC_RGUI, FKEYS,

      KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE,                            KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,    KC_MUTE
    ),

/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      | PgUp | Home |   ↑  | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        | Shift| Ctrl | Alt  | GUI  | Hyper|      |      |  |      |      | PgDn |  ←   |   ↓  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |MicMute|     |  |      |      | Pause|M Prev|M Play|M Next|VolMut|MicMute |
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
      _______, _______, _______, _______, _______, _______,          _______, _______,          KC_PGUP, KC_HOME, KC_UP  , KC_END , KC_VOLU, KC_DEL ,
      _______, KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, KC_HYPR,          _______, _______,          KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_VOLD, KC_INS ,
      _______, _______, _______, _______, _______, _______, MICMUTE, _______, _______, SCR_TGL,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, MICMUTE,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |  °     |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |  ^   |  <   |  >   |  "   |  '   |      |      |  |      |      |  &   |  [   |  ]   |  `   |  %   |  €     |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |  !   |  -   |  +   |  =   |  ,   |      |      |  |      |      |  |   |  (   |  )   |  :   |  ?   |  $     |
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
      _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, DEG    ,
      _______, KC_CIRC, KC_LABK, KC_RABK, KC_DQUO, KC_QUOT,          _______, _______,          KC_AMPR, KC_LBRC, KC_RBRC, KC_GRV , KC_PERC, EURO   ,
      _______, KC_EXLM, KC_MINS, KC_PLUS, KC_EQL , KC_COMM,          _______, _______,          KC_PIPE, KC_LPRN, KC_RPRN, KC_COLN, KC_QUES, KC_DLR ,
      _______, KC_TILD, KC_SLSH, KC_ASTR, KC_BSLS, KC_DOT , _______, _______, _______, _______, KC_UNDS, KC_LCBR, KC_RCBR, KC_SCLN, KC_AT  , GREEK  ,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      | BOOT |      |PrtSc | Num  | Caps | Scroll |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |KeyCst| F1   | F2   | F3   | F4   |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        | Shift| Ctrl | Alt  | GUI  | Hyper|      |      |      |  |      |      | F5   | F6   | F7   | F8   |        |
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
      _______, _______, _______, _______, _______, _______,          _______, _______,          QK_BOOT, _______, KC_PSCR, KC_NUM , KC_CAPS, KC_SCRL,
      _______, _______, _______, _______, _______, _______,          _______, _______,          KEYCAST,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______,
      _______, KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, KC_HYPR,          _______, _______,          _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,
      _______, _______, _______, _______, _______, _______, KC_LPAD, KC_MCTL, KC_MCTL, KC_LPAD, _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      | BOOT |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |QWERTY|Brite+|      |      |      |  |      |      | White| Turq | Purp | Oran | Gree |  Blue  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |Brite-|      |      |      |  |      |      | Rnbw | HUI  | SAI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      | RGB  | HUD  | SAD  | VAD  | RMOD |        |
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
      _______, _______, _______, _______, _______, QK_BOOT,         _______, _______,          _______, _______, _______, _______,  _______, _______,
      _______, _______, _______, QWERTY , KC_BRIU, _______,         _______, _______,           NWHT  ,  TRQS  ,  PRPL  ,  ORNG  ,  NGRN   ,  ELBL  ,
      _______, _______, _______, _______, KC_BRID, _______,         _______, _______,           RNBW  , RGB_HUI, RGB_SAI, RGB_VAI,  RGB_MOD, _______,
      _______, _______, _______, _______, _______, _______,_______, _______, _______, _______, RGB_TOG, RGB_HUD, RGB_SAD, RGB_VAD, RGB_RMOD, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______

    ),

// /*
//  * Layer template - LAYOUT
//  *
//  * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),

// /*
//  * Layer template - LAYOUT_myr
//  *
//  * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  *
//  * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
//  * |        |      |      |      |      |      |                |      |      |      |      |      |      |
//  * `-----------------------------'      `------'                `---------------------------'      '------'
//  */
//     [_LAYERINDEX] = LAYOUT_myr(
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//
//       _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
//     ),
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
};


void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom(); // Enables RGB, without saving settings
    // rgb_matrix_sethsv_noeeprom(42, 24, 255); // Set all LEDs to cold white
    rgb_matrix_sethsv_noeeprom(default_color.hue, default_color.sat, default_color.val); // Set all LEDs to natural white
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    // rgb_matrix_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_SWIRL);
    pimoroni_trackball_set_rgbw(0, 0, 0, 150);

    debug_enable=true;
    debug_matrix=true;
}

static bool is_scrolling = false; // Tracks the current mode - scrolling or cursor movement

void toggle_scrolling_mode(void) {
    is_scrolling = !is_scrolling;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SCR_TGL:
            if (record->event.pressed) {
                toggle_scrolling_mode(); // Toggle the scrolling mode on key press
            }
            return false; // Skip further processing of this key
        case RNBW:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_RAINBOW_MOVING_CHEVRON); // or RGB_MATRIX_CYCLE_ALL
                rgb_matrix_set_speed(10); // 0-255
                rgb_matrix_sethsv(default_color.hue, 255, default_color.val); // Set all LEDs to natural white
            }
            return false; // Skip further processing of this key
        case NWHT:
            if (record->event.pressed) {
                // rgblight_sethsv(42, 24, 255); // Set all LEDs to cold white
                rgb_matrix_sethsv(default_color.hue, default_color.sat, default_color.val); // Set all LEDs to natural white
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case TRQS:
            if (record->event.pressed) {
                rgb_matrix_sethsv(118, 183, 209); // Set all LEDs to cyberpunk turquoise
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case PRPL:
            if (record->event.pressed) {
                rgb_matrix_sethsv(212, 255, 128); // Set all LEDs to cyberpunk purple
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case ORNG:
            if (record->event.pressed) {
                rgb_matrix_sethsv(27, 255, 255); // Set all LEDs to cyberpunk orange
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case NGRN:
            if (record->event.pressed) {
                rgb_matrix_sethsv(78, 235, 255); // Set all LEDs to cyberpunk green
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        case ELBL:
            if (record->event.pressed) {
                rgb_matrix_sethsv(146, 255, 255); // Set all LEDs to cyberpunk blue
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false; // Skip further processing of this key
        default:
            return true; // Process all other keycodes normally
    }
}
// NWHT  ,  TRQS  ,  PRPL  ,  ORNG  ,  NGRN   ,  ELBL

bool oled_task_user(void) {

    if (is_keyboard_master()) {

        render_master();

    } else {

        render_slave();

    }

    return false;
}


// Pointing device settings

static report_mouse_t ema_mouse_report = {0}; // EMA smoothed report
static const float ema_alpha = 0.2; // Smoothing factor, adjust as needed (0 < alpha < 1)
static const float sensitivity = 0.5; // Sensitivity adjustment, adjust as needed

static report_mouse_t debounced_mouse_report = {0};  // To store the debounced values
static const int debounce_threshold = 5;  // Threshold for debouncing, adjust as needed

report_mouse_t debounce_trackball(report_mouse_t mouse_report) {
    // Debounce X movement
    if (abs(mouse_report.x - debounced_mouse_report.x) > debounce_threshold) {
        debounced_mouse_report.x = mouse_report.x;
    }

    // Debounce Y movement
    if (abs(mouse_report.y - debounced_mouse_report.y) > debounce_threshold) {
        debounced_mouse_report.y = mouse_report.y;
    }

    return debounced_mouse_report;
}


report_mouse_t smooth_mouse_movement(report_mouse_t mouse_report) {
    // Adjust sensitivity
    mouse_report.x *= sensitivity;
    mouse_report.y *= sensitivity;

    if (!is_scrolling) {
        // Apply EMA for cursor movement smoothing
        ema_mouse_report.x = (ema_alpha * mouse_report.x) + ((1 - ema_alpha) * ema_mouse_report.x);
        ema_mouse_report.y = (ema_alpha * mouse_report.y) + ((1 - ema_alpha) * ema_mouse_report.y);
    } else {
        // Apply EMA for scrolling smoothing
        ema_mouse_report.h = (ema_alpha * mouse_report.x) + ((1 - ema_alpha) * ema_mouse_report.h);
        ema_mouse_report.v = (ema_alpha * mouse_report.y) + ((1 - ema_alpha) * ema_mouse_report.v);
        ema_mouse_report.x = 0; // Neutralize cursor movement in scroll mode
        ema_mouse_report.y = 0;
    }

    return ema_mouse_report; // Return the EMA smoothed report
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    mouse_report = debounce_trackball(mouse_report);
    mouse_report = smooth_mouse_movement(mouse_report);
    return mouse_report;
}

