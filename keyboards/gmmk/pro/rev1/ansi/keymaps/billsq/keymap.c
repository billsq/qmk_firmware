/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#ifdef RGB_MATRIX_ENABLE
    #ifndef RGB_CONFIRMATION_BLINKING_TIME
        #define RGB_CONFIRMATION_BLINKING_TIME 2000 // 2 seconds
    #endif
#endif // RGB_MATRIX_ENABLE

enum custom_keycodes {
    KVM_1 = SAFE_RANGE,
    KVM_2,
    KVM_3,
    KVM_4
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//      ESC      F1       F2       F3       F4       F5       F6       F7       F8       F9       F10      F11      F12	     Del          Rotary(Mute)
//      ~        1        2        3        4        5        6        7        8        9        0         -       (=)	     BackSpc           Home
//      Tab      Q        W        E        R        T        Y        U        I        O        P        [        ]        \                 PgUp
//      Caps     A        S        D        F        G        H        J        K        L        ;        "                 Enter             PgDn
//      Sh_L              Z        X        C        V        B        N        M        ,        .        ?                 Sh_R     Up       End
//      Ct_L     Win_L    Alt_L                               SPACE                               Alt_R    FN       Ct_R     Left     Down     Right


    // The FN key by default maps to a momentary toggle to layer 1 to provide access to the RESET key (to put the board into bootloader mode). Without
    // this mapping, you have to open the case to hit the button on the bottom of the PCB (near the USB cable attachment) while plugging in the USB
    // cable to get the board into bootloader mode - definitely not fun when you're working on your QMK builds. Remove this and put it back to KC_RGUI
    // if that's your preference.
    //
    // To put the keyboard in bootloader mode, use FN+backslash. If you accidentally put it into bootloader, you can just unplug the USB cable and
    // it'll be back to normal when you plug it back in.
    //
    // This keyboard defaults to 6KRO instead of NKRO for compatibility reasons (some KVMs and BIOSes are incompatible with NKRO).
    // Since this is, among other things, a "gaming" keyboard, a key combination to enable NKRO on the fly is provided for convenience.
    // Press Fn+N to toggle between 6KRO and NKRO. This setting is persisted to the EEPROM and thus persists between restarts.

	[0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,           KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LALT, KC_LGUI,                            KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

	[1] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, KC_INS,           _______,
        RGB_TOG, KVM_1,   KVM_2,   KVM_3,   KVM_4,   _______, _______, _______, _______, _______, _______, _______, _______, _______,          KC_PSCR,
        _______, _______, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RESET,            _______,
        _______, _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, RGB_HUI, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          _______, RGB_MOD, KC_SCRL,
        _______, _______, _______,                            _______,                            _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    )
};
// clang-format on

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
      //tap_code(KC_MS_WH_DOWN);
      tap_code(KC_VOLU);
    } else {
      //tap_code(KC_MS_WH_UP);
      tap_code(KC_VOLD);
    }
    return true;
}
#endif // ENCODER_ENABLE

#ifdef RGB_MATRIX_ENABLE

/* Renaming those to make the purpose on this keymap clearer */
#define LED_FLAG_CAPS LED_FLAG_NONE
#define LED_FLAG_EFFECTS LED_FLAG_INDICATOR

static void set_rgb_caps_leds(bool);

#if RGB_CONFIRMATION_BLINKING_TIME > 0
static uint16_t effect_started_time = 0;
static uint8_t r_effect = 0x0, g_effect = 0x0, b_effect = 0x0;
static void start_effects(void);

/* The higher this is, the slower the blinking will be */
#ifndef TIME_SELECTED_BIT
    #define TIME_SELECTED_BIT 8
#endif
#if TIME_SELECTED_BIT < 0 || TIME_SELECTED_BIT >= 16
    #error "TIME_SELECTED_BIT must be a positive integer smaller than 16"
#endif
#define effect_red() r_effect = 0xFF, g_effect = 0x0, b_effect = 0x0
#define effect_green() r_effect = 0x0, g_effect = 0xFF, b_effect = 0x0
#endif // RGB_CONFIRMATION_BLINKING_TIME > 0

bool led_update_user(led_t led_state) {
    if (led_state.caps_lock) {
        if (!rgb_matrix_is_enabled()) {
            /* Turn ON the RGB Matrix for CAPS LOCK */
            rgb_matrix_set_flags(LED_FLAG_CAPS);
            rgb_matrix_enable();
        }
    } else if (rgb_matrix_get_flags() == LED_FLAG_CAPS) {
        /* RGB Matrix was only ON because of CAPS LOCK. Turn it OFF. */
        rgb_matrix_set_flags(LED_FLAG_ALL);
        rgb_matrix_disable();
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint16_t kvm_keycode = 0;

    switch (keycode) {
    #ifdef NKRO_ENABLE
    #if RGB_CONFIRMATION_BLINKING_TIME > 0
        case NK_TOGG:
            if (record->event.pressed) {
                if (keymap_config.nkro) {
                    /* Turning NKRO OFF */
                    effect_red();
                } else {
                    /* Turning NKRO ON */
                    effect_green();
                }
                start_effects();
            }
            break;
        case NK_ON:
            if (record->event.pressed) {
                if (!keymap_config.nkro) {
                    /* Turning NKRO ON */
                    effect_green();
                    start_effects();
                }
            }
            break;
        case NK_OFF:
            if (record->event.pressed) {
                if (keymap_config.nkro) {
                    /* Turning NKRO OFF */
                    effect_red();
                    start_effects();
                }
            }
            break;
    #endif // RGB_CONFIRMATION_BLINKING_TIME > 0
    #endif // NKRO_ENABLE
        case RGB_MOD:
        case RGB_RMOD:
        case RGB_HUI:
        case RGB_HUD:
        case RGB_SAI:
        case RGB_SAD:
        case RGB_VAI:
        case RGB_VAD:
        case RGB_SPI:
        case RGB_SPD:
            if (record->event.pressed) {
                if (rgb_matrix_get_flags() != LED_FLAG_ALL) {
                    /* Ignore changes to RGB settings while only it's supposed to be OFF */
                    return false;
                }
            }
            break;
        case RGB_TOG:
            if (record->event.pressed) {
                if (rgb_matrix_is_enabled()) {
                    switch (rgb_matrix_get_flags()) {
                        #if RGB_CONFIRMATION_BLINKING_TIME > 0
                        case LED_FLAG_EFFECTS:
                        #endif
                        case LED_FLAG_CAPS:
                            /* Turned ON because of EFFECTS or CAPS, is actually OFF */
                            /* Change to LED_FLAG_ALL to signal it's really ON */
                            rgb_matrix_set_flags(LED_FLAG_ALL);
                            /* Will be re-enabled by the processing of the toggle */
                            rgb_matrix_disable_noeeprom();
                            break;
                        case LED_FLAG_ALL:
                            /* Is actually ON */
                            #if RGB_CONFIRMATION_BLINKING_TIME > 0
                            if (effect_started_time > 0) {
                                /* Signal EFFECTS */
                                rgb_matrix_set_flags(LED_FLAG_EFFECTS);
                                /* Will be re-enabled by the processing of the toggle */
                                rgb_matrix_disable_noeeprom();
                            } else
                            #endif
                            if (host_keyboard_led_state().caps_lock) {
                                /* Signal CAPS */
                                rgb_matrix_set_flags(LED_FLAG_CAPS);
                                /* Will be re-enabled by the processing of the toggle */
                                rgb_matrix_disable_noeeprom();
                            }
                            break;
                    }
                }
            }
            break;

        case KVM_1:
            kvm_keycode = KC_1;
            break;
        case KVM_2:
            kvm_keycode = KC_2;
            break;
        case KVM_3:
            kvm_keycode = KC_3;
            break;
        case KVM_4:
            kvm_keycode = KC_4;
            break;
    }

    if (kvm_keycode != 0 && record->event.pressed) {
        register_code(KC_SCRL);
        unregister_code(KC_SCRL);
        register_code(KC_SCRL);
        unregister_code(KC_SCRL);
        register_code(kvm_keycode);
        unregister_code(kvm_keycode);
        register_code(KC_ENT);
        unregister_code(KC_ENT);
    }

    return true;
}


void rgb_matrix_indicators_user() {
    #if RGB_CONFIRMATION_BLINKING_TIME > 0
    if (effect_started_time > 0) {
        /* Render blinking EFFECTS */
        uint16_t deltaTime = sync_timer_elapsed(effect_started_time);
        if (deltaTime <= RGB_CONFIRMATION_BLINKING_TIME) {
            uint8_t led_state = ((~deltaTime) >> TIME_SELECTED_BIT) & 0x01;
            uint8_t val_r = led_state * r_effect;
            uint8_t val_g = led_state * g_effect;
            uint8_t val_b = led_state * b_effect;
            rgb_matrix_set_color_all(val_r, val_g, val_b);
            set_rgb_caps_leds(host_keyboard_led_state().caps_lock);
            return;
        } else {
            /* EFFECTS duration is finished */
            effect_started_time = 0;
            if (rgb_matrix_get_flags() == LED_FLAG_EFFECTS) {
                /* It was turned ON because of EFFECTS */
                if (host_keyboard_led_state().caps_lock) {
                    /* CAPS is still ON. Demote to CAPS */
                    rgb_matrix_set_flags(LED_FLAG_CAPS);
                } else {
                    /* There is nothing else keeping RGB enabled. Reset flags and turn if off. */
                    rgb_matrix_set_flags(LED_FLAG_ALL);
                    rgb_matrix_disable_noeeprom();
                }
            }
        }
    }
    #endif // RGB_CONFIRMATION_BLINKING_TIME > 0
    if (rgb_matrix_get_flags() == LED_FLAG_CAPS) {
        rgb_matrix_set_color_all(0x0, 0x0, 0x0);
    }
    set_rgb_caps_leds(host_keyboard_led_state().caps_lock);
}

#if RGB_CONFIRMATION_BLINKING_TIME > 0
static void start_effects() {
    effect_started_time = sync_timer_read();
    if (!rgb_matrix_is_enabled()) {
        /* Turn it ON, signal the cause (EFFECTS) */
        rgb_matrix_set_flags(LED_FLAG_EFFECTS);
        rgb_matrix_enable_noeeprom();
    } else if (rgb_matrix_get_flags() == LED_FLAG_CAPS) {
        /* It's already ON, promote the cause from CAPS to EFFECTS */
        rgb_matrix_set_flags(LED_FLAG_EFFECTS);
    }
}
#endif // RGB_CONFIRMATION_BLINKING_TIME > 0

static void set_caps_indicator_leds(uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_matrix_set_color(67, red, green, blue); // Left side LED 1
    rgb_matrix_set_color(68, red, green, blue); // Right side LED 1
    rgb_matrix_set_color(70, red, green, blue); // Left side LED 2
    rgb_matrix_set_color(71, red, green, blue); // Right side LED 2
    rgb_matrix_set_color(73, red, green, blue); // Left side LED 3
    rgb_matrix_set_color(74, red, green, blue); // Right side LED 3
    rgb_matrix_set_color(76, red, green, blue); // Left side LED 4
    rgb_matrix_set_color(77, red, green, blue); // Right side LED 4
    rgb_matrix_set_color(80, red, green, blue); // Left side LED 5
    rgb_matrix_set_color(81, red, green, blue); // Right side LED 5
    rgb_matrix_set_color(83, red, green, blue); // Left side LED 6
    rgb_matrix_set_color(84, red, green, blue); // Right side LED 6
    rgb_matrix_set_color(87, red, green, blue); // Left side LED 7
    rgb_matrix_set_color(88, red, green, blue); // Right side LED 7
    rgb_matrix_set_color(91, red, green, blue); // Left side LED 8
    rgb_matrix_set_color(92, red, green, blue); // Right side LED 8
    rgb_matrix_set_color(3, red, green, blue); // CAPS LED
}

static void set_rgb_caps_leds(bool state) {
    static bool old_state = 0;
    static uint16_t caps_enabled_time = 0;
    static bool blinker_state = 0;

    if (state != old_state)
    {
        if (state)
        {
            caps_enabled_time = sync_timer_read();
            blinker_state = 1;
        }
        else
        {
            caps_enabled_time = 0;
            blinker_state = 0;
        }
    }

    old_state = state;

    if (state)
    {
        uint16_t deltaTime = sync_timer_elapsed(caps_enabled_time);

        if ((deltaTime >> 9) > 0)
        {
            blinker_state = !blinker_state;
            caps_enabled_time = sync_timer_read();
        }

        if (blinker_state)
        {
            set_caps_indicator_leds(0xFF, 0, 0);
        }
        else
        {
            set_caps_indicator_leds(0, 0, 0);
        }
    }
}

#endif // RGB_MATRIX_ENABLE
