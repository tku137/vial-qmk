#pragma once
#include QMK_KEYBOARD_H


enum layers {
    _QWERTY = 0,
    _SYM,
    _NAV,
    _FUNCTION,
    _ADJUST,
};


// Aliases for readability
#define QWERTY   DF(_QWERTY)

#define SYM      MO(_SYM)
#define NAV      MO(_NAV)
#define FKEYS    MO(_FUNCTION)
#define ADJUST   MO(_ADJUST)

