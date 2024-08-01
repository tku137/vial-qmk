#pragma once
#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _COLEMAK_DH,
    _SYM,
    _NAV,
    _FUNCTION,
    _ADJUST,
    _GAMING,
    _GAMING_EXT,
};

// Aliases for readability
// Typing layers
#define QWERTY DF(_QWERTY)
#define COLEMAK DF(_COLEMAK_DH)

// Extend layers
#define SYM MO(_SYM)
#define NAV MO(_NAV)

// Function layers
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_ADJUST)

// Special layers
#define GAMING DF(_GAMING)
#define GMEXT MO(_GAMING_EXT)
