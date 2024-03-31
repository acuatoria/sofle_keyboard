// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later
#include "quantum.h"

#ifdef SWAP_HANDS_ENABLE

__attribute__ ((weak))
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] =
    // The LAYOUT macro could work for this, but it was harder to figure out the
    // identity using it.

    // This is the identity layout.
/*
{
    { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0} },
    { {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1} },
    { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2} },
    { {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3} },
    { {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4} },

    { {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5} },
    { {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6} },
    { {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7} },
    { {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8} },
    { {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9} },
};
*/

    // This is the mirror, q <-> p, w <-> o, etc...
{
 { {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5} },
 { {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6} },
 { {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7} },
 { {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8} },
 { {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9} },

 { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0} },
 { {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1} },
 { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2} },
 { {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3} },
 { {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4} },
};

#    ifdef ENCODER_MAP_ENABLE
const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = {1, 0};
#    endif

#endif


//********************************************
//* matrix animation begining      *
//********************************************
#include "config.h"
#include "matrix.h"
#include "quantum.h"

#define MAX_CHAIN_LENGTH 5
#define CHAINS 5
#define ROW_LIMIT 15
#define BULLET_TIME 2

uint16_t animation_delay = 200; // Default animation delay in milliseconds
const char characters[] = "0123456789QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";

typedef struct {
    uint8_t x;
    float y;
    float speed;
    char symbol[MAX_CHAIN_LENGTH + 1]; // Changed to an array
} Drop;

Drop drops[CHAINS];

void init_drop(Drop *drop, int i) {
    drop->x = i;
    drop->y = -1;
    drop->speed = (rand() % 3) + 1;
    for (int j = 0; j < (rand() % MAX_CHAIN_LENGTH + 1); j++) {
        drop->symbol[j] = characters[rand() % (sizeof(characters) - 1)];
    }
    drop->symbol[MAX_CHAIN_LENGTH] = '\0'; // Null terminate the string
}

void init_drops(void) {
    for (int i = 0; i < CHAINS; i++) { // Corrected loop start
        init_drop(&drops[i], i);
    }
}

void update_drop(Drop *drop) {
    drop->y += drop->speed / BULLET_TIME;
    if ((uint8_t)drop->y > (ROW_LIMIT + MAX_CHAIN_LENGTH)) {
        init_drop(drop, drop->x);
    }
    if (strlen(drop->symbol) > 0 && (rand() % 10) < 5) {
        int index = rand() % strlen(drop->symbol);
        drop->symbol[index] = characters[rand() % (sizeof(characters) - 1)];
    }
}

void draw_drop(Drop *drop) {
    for(int i = 0; i < strlen(drop->symbol); i++) {
        float pos_y = drop->y - i;
        /* out of bounds */
        if (pos_y < 1 || pos_y > ROW_LIMIT)  {
            continue;
        }
        oled_set_cursor(drop->x, (uint8_t)pos_y);
        oled_write_char(drop->symbol[i], false);
    }
}

void render_animation(void) {
    oled_clear();

    for (int i = 0; i < CHAINS; i++) { // Corrected loop start
        update_drop(&drops[i]);
        draw_drop(&drops[i]);
    }

    oled_render(); 
}

void matrix_scan_user(void) {
    static uint32_t last_animation_time = 0;

    if (timer_elapsed(last_animation_time) > animation_delay) {
        render_animation();
        last_animation_time = timer_read();
    }
}

void matrix_init_user(void) {
    init_drops();
}

//********************************************
//* matrix animation end      *
//********************************************

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}


bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        
    } else {
        matrix_scan_user();
    }
    return true;
}

#endif

#ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false;
    }
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
            /* animation_delay = animation_delay + 10; */
        } else {
            tap_code(KC_VOLD);
            /* animation_delay = animation_delay - 10; */
        }
    } else if (index == 1) {
        if (clockwise) {
            tap_code(KC_PGDN);
            /* animation_delay = animation_delay + 10; */
        } else {
            tap_code(KC_PGUP);
            /* animation_delay = animation_delay - 10; */
        }
    }
    return true;
}
#endif
