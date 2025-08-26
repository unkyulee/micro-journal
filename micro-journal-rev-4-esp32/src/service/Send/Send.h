#pragma once

// 
void send_loop();

//
typedef void (*SendFunc)(int);
void send_register(SendFunc send_func);

//
void send_start();