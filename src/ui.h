#pragma once
#include "common.h"

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

void ui_init();
void draw_stuff(uint32_t* video);
void input(uint8_t* key);
bool is_quit();
void stop_display();