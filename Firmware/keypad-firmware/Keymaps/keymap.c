// Copyright 2023 Nico Stuhlmueller (@ThePurox)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include <string.h>

#define LAYOUT_MACROPAD(\
  k00, k01, k02, k03, \
  k10, k11, k12, k13, \
  k20, k21, k22, k23\
) {\
  { k00, k01, k02, k03 }, \
  { k10, k11, k12, k13 }, \
  { k20, k21, k22, k23 }  \
}

enum layer_names{
    __MEDIA,
    __CAD,
    __OVERLAY,
    __CODE,
    __OLED,
};

#define COPY LCTL(KC_C)
#define PSTE LCTL(KC_V)
#define UNDO LCTL(KC_Z)

#define D_MUTE LCTL(KC_F13)
#define D_DEAF LCTL(KC_F14)
    
#define EXTRUDE LSFT(KC_E)
#define SKETCH LSFT(KC_S)
#define REVOLVE LSFT(KC_W)
#define FILLET LSFT(KC_F)
#define STEAM LSFT(KC_TAB)
#define DISCORD LSFT(KC_NUBS)

#define KILLER LCTL(LSFT(KC_ESC))
#define PERFORMANCE LCTL(KC_F3)

#define MAX_OLED_BRIGHTNESS 255
#define HIGH_OLED_BRIGHTNESS 130
#define MEDIUM_OLED_BRIGHTNESS 90
#define LOW_OLED_BRIGHTNESS 65
#define MIN_OLED_BRIGHTNESS 1

#define LAYER_CYCLE_START 0
#define LAYER_CYCLE_END 4

#define IMPORT_BIN(sect, file, sym) \
    asm(".section " #sect "\n" \
        ".balign 4\n" \
        ".global " #sym "\n" \
        #sym ":\n" \
        ".incbin \"" file "\"\n" \
        ".global _sizeof_" #sym "\n" \
        ".set _sizeof_" #sym ", . - " #sym "\n" \
        ".balign 4\n" \
        ".section \".text\"\n")

IMPORT_BIN(".rodata", "badapple.bin", badapple_data);

extern const uint8_t badapple_data[];
extern const char _sizeof_badapple_data[];

#define TOTAL_FRAMES 3194
#define FRAME_DELAY 100

static uint32_t oled_timer = 0;
static uint16_t frame_index = 0;

enum keycodes {
  KC_CYCLE_LAYERS = QK_USER,
  CK_CYCLE_LAYERS,
  ANIMATION_TOGGLE,
  OLED_BRIGHTNESS_UP,
  OLED_BRIGHTNESS_DOWN,
  OLED_BRIGHTNESS_DEFAULT,
  OLED_ON_OFF,
  ANIMATION_ONCE,
  PRINT_LN,
  PRINT,
  LOOP_STATEMENT,
  IF_STATEMENT,
  ELSE_IF_STATEMENT,
  ELSE_STATEMENT,
};

bool animation_value = false;
uint8_t loop_value = 0;
uint8_t brightness_layer = 2;

void brightness_picker(int level){
  switch(level){
    case 0:
      oled_set_brightness(MAX_OLED_BRIGHTNESS);
      break;
    case 1:
      oled_set_brightness(HIGH_OLED_BRIGHTNESS);
      break;
    case 2:
      oled_set_brightness(MEDIUM_OLED_BRIGHTNESS);
      break;
    case 3:
      oled_set_brightness(LOW_OLED_BRIGHTNESS);
      break;
    case 4:
      oled_set_brightness(MIN_OLED_BRIGHTNESS);
      break;
    default:
      break;
  }
}

void render_bad_apple(void) {
    if (timer_elapsed32(oled_timer) > FRAME_DELAY) {
        oled_timer = timer_read32();

        const uint8_t* frame_ptr = badapple_data + (frame_index * 512);

        // --- Fix 2: Added (const char*) cast to satisfy the compiler ---
        oled_write_raw_P((const char*)frame_ptr, 512);

        frame_index++;
        if (frame_index >= TOTAL_FRAMES) {
            frame_index = 0;
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_CYCLE_LAYERS: // Forward Cycle
      if (record->event.pressed) {
        uint8_t current_layer = get_highest_layer(layer_state);
        if (current_layer >= LAYER_CYCLE_START && current_layer <= LAYER_CYCLE_END) {
          uint8_t next_layer = current_layer + 1;
          if (next_layer > LAYER_CYCLE_END) {
              next_layer = LAYER_CYCLE_START;
          }
          layer_move(next_layer);
        }
      }
      return false;

    case CK_CYCLE_LAYERS: // Backward Cycle
      if (record->event.pressed) {
        uint8_t current_layer = get_highest_layer(layer_state);
        if (current_layer >= LAYER_CYCLE_START && current_layer <= LAYER_CYCLE_END) {
          uint8_t next_layer = current_layer - 1;
          if (next_layer < LAYER_CYCLE_START || next_layer > LAYER_CYCLE_END) { // Checks for underflow
              next_layer = LAYER_CYCLE_END;
          }
          layer_move(next_layer);
        }
      }
      return false;
    case ANIMATION_TOGGLE:
    if(record->event.pressed){  
    if(animation_value){
        animation_value = false;
        oled_timer = 0;
        frame_index = 0;
        oled_clear();
      }
      else{
        animation_value = true;
        oled_timer = 0;
        frame_index = 0;
      }
    }
      return false;
    
    case OLED_BRIGHTNESS_UP:
      if (record->event.pressed)
      {
      if(brightness_layer < 4){
        brightness_layer = brightness_layer + 1;
      }
      else if(brightness_layer == 4){
        brightness_layer = 0;
      }
      brightness_picker(brightness_layer);
    }
      return false;
    
    case OLED_BRIGHTNESS_DOWN:
      
    if(record->event.pressed){
      if(brightness_layer > 0){
        brightness_layer = brightness_layer - 1;
      }
      else if(brightness_layer == 0){
        brightness_layer = 4;
      }
      brightness_picker(brightness_layer);
    } 
      return false;
    
    case OLED_BRIGHTNESS_DEFAULT:
    if(record->event.pressed){  
    brightness_layer = 2;
      brightness_picker(brightness_layer);
    }
      return false;
    
    case OLED_ON_OFF:
      
    if(record->event.pressed){
      if(is_oled_on()){
        oled_off();
      }
      else{
        oled_on();
      }
      
    }
    return false;
    case PRINT_LN:
        if(record->event.pressed){
        SEND_STRING("System.out.println(");
        
        }
    return false;

    case IF_STATEMENT:
        if(record->event.pressed){
        SEND_STRING("if (){\n\n}");
        }
    return false;

    case ELSE_IF_STATEMENT:
        if(record->event.pressed){
        SEND_STRING("else if (){\n\n}");
        }
    return false;

    case ELSE_STATEMENT:
        if(record->event.pressed){
        SEND_STRING("else {\n\n}");
        }
    return false;

    case LOOP_STATEMENT:
        if(record->event.pressed){
        SEND_STRING("while (){\n\n}");
        }
    return false;

    case PRINT:
        if(record->event.pressed){
        SEND_STRING("System.out.print(");
        }
    return false;


    default:
      return true;
  }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [__MEDIA] = LAYOUT_MACROPAD(
        KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,
        KC_WBAK, KC_WREF, KC_WFWD, KC_CYCLE_LAYERS,
        COPY, PSTE, UNDO, CK_CYCLE_LAYERS
    ),
    [__CAD] = LAYOUT_MACROPAD(
        EXTRUDE, REVOLVE, FILLET, KC_MUTE,
        SKETCH, KC_U, KC_O, KC_CYCLE_LAYERS,
        COPY, PSTE, UNDO, CK_CYCLE_LAYERS
    ),

    [__OVERLAY] = LAYOUT_MACROPAD(
        KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,
        STEAM, PERFORMANCE, KILLER, KC_CYCLE_LAYERS,
        DISCORD, D_MUTE, D_DEAF, CK_CYCLE_LAYERS
    ),

    [__CODE] = LAYOUT_MACROPAD(
        KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,
        PRINT_LN, PRINT, LOOP_STATEMENT, KC_CYCLE_LAYERS,
        IF_STATEMENT, ELSE_IF_STATEMENT, ELSE_STATEMENT, CK_CYCLE_LAYERS
    ),

    [__OLED] = LAYOUT_MACROPAD(
      KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,  
      OLED_BRIGHTNESS_DOWN, OLED_BRIGHTNESS_DEFAULT, OLED_BRIGHTNESS_UP, KC_CYCLE_LAYERS,
      OLED_ON_OFF, ANIMATION_TOGGLE, UNDO, CK_CYCLE_LAYERS
    )

};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [__MEDIA] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [__CAD] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [__OVERLAY] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [__CODE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [__OLED] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)}
};
#endif

#ifdef OLED_ENABLE
bool oled_task_user(void) {
if(animation_value) {
      render_bad_apple();
      return false;
}
else{
// Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);

    switch (get_highest_layer(layer_state)) {
        case __MEDIA:
            oled_write_P(PSTR("MEDIA\n"), false);
            break;
        case __CAD:
            oled_write_P(PSTR("CAD\n"), false);
            break;
        case __OVERLAY:
            oled_write_P(PSTR("OVERLAY\n"), false);
            break;
        case __CODE:
            oled_write_P(PSTR("CODE\n"), false);
            break;
        case __OLED:
            oled_write_P(PSTR("OLED\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }
    
    return false;
}
}


#endif