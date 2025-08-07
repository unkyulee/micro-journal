#pragma once

#include <Arduino.h>

void keypad_load_config(String filename, int *layers, int size, const char **keys, int num_layers);
int keypad_convert_wp_keys(String key);