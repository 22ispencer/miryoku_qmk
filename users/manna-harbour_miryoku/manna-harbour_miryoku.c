// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
#define OLED_TIMEOUT 300000

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"

// Additional Features double tap guard

enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
    MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        reset_keyboard();
    }
}

#define MIRYOKU_X(LAYER, STRING)                                        \
    void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
        if (state->count == 2) {                                        \
            default_layer_set((layer_state_t)1 << U_##LAYER);           \
        }                                                               \
    }
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {[U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
                                          MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

// keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
    MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

// shift functions

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){&capsword_key_override, NULL};

#ifdef OLED_ENABLE

static void render_logo(void) {
    static const unsigned char PROGMEM raw_logo[] = {
        112,248,168,248,216,136,252, 12,  6,  6,  4, 14,158,254, 12,  0,
    };
    oled_write_raw_P(raw_logo, sizeof(raw_logo));
}

static void print_status_narrow(void) {
    // Print current layer
    oled_write_ln_P(PSTR("LAYER"), false);
    switch (get_highest_layer(layer_state)) {
        case U_BASE:
            oled_write_P(PSTR("base\n"), false);
            break;
        case U_NAV:
            oled_write_P(PSTR("nav\n"), false);
            break;
        case U_MOUSE:
            oled_write_P(PSTR("mouse\n"), false);
            break;
        case U_MEDIA:
            oled_write_P(PSTR("media\n"), false);
            break;
        case U_NUM:
            oled_write_P(PSTR("num\n"), false);
            break;
        case U_SYM:
            oled_write_P(PSTR("sym\n"), false);
            break;
        case U_FUN:
            oled_write_P(PSTR("fun\n"), false);
            break;
        default:
            oled_write_ln_P(PSTR("undef"), false);
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_narrow();
    } else {
        render_logo();
    }
    return false;
}

#endif

// thumb combos

#if defined(MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[]  = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[]        = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[]      = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[]      = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[]        = {KC_0, KC_MINS, COMBO_END};
#    if defined(MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
#    else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
#    endif
const uint16_t PROGMEM thumbcombos_fun[]       = {KC_SPC, KC_TAB, COMBO_END};
combo_t                key_combos[COMBO_COUNT] = {COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
                                                  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
                                                  COMBO(thumbcombos_nav, KC_DEL),
                                                  COMBO(thumbcombos_mouse, KC_BTN3),
                                                  COMBO(thumbcombos_media, KC_MUTE),
                                                  COMBO(thumbcombos_num, KC_DOT),
#    if defined(MIRYOKU_LAYERS_FLIP)
                                   COMBO(thumbcombos_sym, KC_RPRN),
#    else
                                   COMBO(thumbcombos_sym, KC_LPRN),
#    endif
                                                  COMBO(thumbcombos_fun, KC_APP)};
#endif
