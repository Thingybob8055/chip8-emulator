#pragma once
#include "common.h"

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_NUMBERS 16
#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH 64
#define FONTSET_SIZE 80
#define START_ADDRESS 0x200
#define KEY_COUNT 16


extern uint8_t memory[MEMORY_SIZE];
extern uint8_t registers[REGISTER_COUNT];
extern uint16_t INDEX;
extern uint16_t pc;
extern uint8_t delay_timer;
extern uint8_t sound_timer;
extern uint16_t stack[STACK_NUMBERS];
extern uint8_t sp;
extern uint16_t opcode;
extern uint8_t keypad[KEY_COUNT];
extern uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT];
extern uint8_t draw_flag;
extern uint8_t sound_flag;

void load_rom(char* filename);
void delay(uint32_t ms);
void load_font();
void cpu_cycle();
void load_font();
