#pragma once

#include <Arduino.h>

void keypad_load_config(String filename, int *layers, int num_cols);
uint8_t keypad_convert_wp_keys(String key);