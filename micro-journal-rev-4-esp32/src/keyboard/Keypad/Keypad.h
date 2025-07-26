#pragma once

#include <Arduino.h>

void keypad_load_config(String filename, int *layers, int num_cols);
int keypad_convert_wp_keys(String key);