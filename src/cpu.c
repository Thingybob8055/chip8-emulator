#include "cpu.h"
#include "common.h"

uint8_t memory[MEMORY_SIZE] = {0};
uint8_t registers[REGISTER_COUNT] = {0};
uint16_t INDEX = 0;
uint16_t pc = START_ADDRESS;
uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT] = {0};
uint16_t stack[STACK_NUMBERS] = {0};
uint8_t sp = 0;
uint8_t keypad[KEY_COUNT] = {0};
uint8_t draw_flag = 0;
uint8_t sound_flag = 0;
uint8_t delay_timer = 0;
uint8_t sound_timer = 0;

uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void load_font() {
    for (int i = 0; i < 80; i++) {
        memory[i] = fontset[i];
    }
}

void load_rom(char* filename) {
    FILE* fp = fopen(filename, "rb");
    int data;
    for (int i = 0; (data = fgetc(fp)) != EOF; i++) {
        memory[START_ADDRESS + i] = data; // first 512 bytes are reserved
    }
}

void OP_00E0() {
	for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
		video[i] = 0;
	}
}
void OP_00EE() {
	pc = stack[sp];
	sp--;
}
void OP_1nnn(uint16_t opcode) {
	uint16_t address = opcode & 0x0FFF;
	pc = address;
}
void OP_2nnn(uint16_t opcode) {
	uint16_t address = opcode & 0x0FFF;
	sp++;
	stack[sp] = pc;
	pc = address;
}
void OP_3xkk(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t byte = opcode & 0x00FF;
	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}
void OP_4xkk(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t byte = opcode & 0x00FF;
	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}
void OP_5xy0(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vx] == registers[Vy]) {
		pc += 4;
	} 
	else {
		pc += 2;
	}
}
void OP_6xkk(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t byte = opcode & 0x00FF;
	registers[Vx] = byte;
}
void OP_7xkk(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t byte = opcode & 0x00FF;
	registers[Vx] += byte;
}
void OP_8xy0(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] = registers[Vy];
}
void OP_8xy1(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] |= registers[Vy];
}
void OP_8xy2(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] &= registers[Vy];
}
void OP_8xy3(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] ^= registers[Vy];
}
void OP_8xy4(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	uint16_t sum = registers[Vx] + registers[Vy];
	if (sum > 255)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = sum & 0xFF;
}
void OP_8xy5(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] -= registers[Vy];
}
void OP_8xy6(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1);
	registers[Vx] >>= 1;
}
void OP_8xy7(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = registers[Vy] - registers[Vx];
}
void OP_8xyE(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80) >> 7;
	registers[Vx] <<= 1;
}
void OP_9xy0(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}
void OP_Annn(uint16_t opcode) {
	uint16_t address = opcode & 0x0FFF;
	INDEX = address;
}
void OP_Bnnn(uint16_t opcode) {
	uint16_t address = opcode & 0x0FFF;
	pc = registers[0] + address;
}
void OP_Cxkk(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	registers[Vx] = rand() & (opcode & 0x00FF);
}
void OP_Dxyn(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	draw_flag = 1;
	uint16_t height = opcode & 0x000F;
	uint16_t pixel;

	registers[0xF] = 0;
	for(int y = 0; y< height; y++) {
		pixel = memory[INDEX + y];
		for(int x = 0; x<8; x++) {
			if((pixel & (0x80 >> x)) != 0 ) {
				if(video[(registers[Vx] + x + ((registers[Vy] + y)*VIDEO_WIDTH))] == 1) {
					registers[0xF] = 1;
				}
				video[registers[Vx] + x + ((registers[Vy] + y) * VIDEO_WIDTH)] ^= 1;
			}
		}
	}
}
void OP_Ex9E(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	if(keypad[registers[Vx]]) {
		pc += 2;
	}
}
void OP_ExA1(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	if(!keypad[registers[Vx]]) {
		pc += 2;
	}
}
void OP_Fx07(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	registers[Vx] = delay_timer;
}
void OP_Fx0A(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	for(int i = 0; i<16; i++) {
		if(keypad[i]){
			registers[Vx] = i;
			pc += 2;
			break;
		}
	}
}
void OP_Fx15(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	delay_timer = registers[Vx];
}
void OP_Fx18(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	sound_timer = registers[Vx];
}
void OP_Fx1E(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	INDEX += registers[Vx];
}
void OP_Fx29(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t digit = registers[Vx];
	INDEX = registers[Vx] * 5;
}
void OP_Fx33(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t value = registers[Vx];
	memory[INDEX + 2] = value % 10;
	value /= 10;
	memory[INDEX + 1] = value % 10;
	value /= 10;
	memory[INDEX] = value % 10;
}
void OP_Fx55(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= Vx; ++i)
	{
		memory[INDEX + i] = registers[i];
	}
}
void OP_Fx65(uint16_t opcode) {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[INDEX + i];
	}
}

void cpu_cycle() {
	draw_flag = 0;
	sound_flag = 0;
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x00FF) {
				case 0x00E0:
					OP_00E0();
					pc += 2;
					break;
				case 0x00EE:
					OP_00EE();
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					break;
			}
			break;
		case 0x1000:
			OP_1nnn(opcode);
			break;
		case 0x2000:
			OP_2nnn(opcode);
			break;
		case 0x3000:
			OP_3xkk(opcode);
			pc += 2;
			break;
		case 0x4000:
			OP_4xkk(opcode);
			pc += 2;
			break;
		case 0x5000:
			OP_5xy0(opcode);
			break;
		case 0x6000:
			OP_6xkk(opcode);
			pc += 2;
			break;
		case 0x7000:
			OP_7xkk(opcode);
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F){
				case 0x0000:
					OP_8xy0(opcode);
					pc += 2;
					break;
				case 0x0001:
					OP_8xy1(opcode);
					pc += 2;
					break;
				case 0x0002:
					OP_8xy2(opcode);
					pc += 2;
					break;
				case 0x0003:;
					OP_8xy3(opcode);
					pc += 2;
					break;
				case 0x0004:
					OP_8xy4(opcode);
					pc += 2;
					break;
				case 0x0005:
					OP_8xy5(opcode);
					pc += 2;
					break;
				case 0x0006:
					OP_8xy6(opcode);
					pc += 2;
					break;
				case 0x0007:
					OP_8xy7(opcode);
					pc += 2;
					break;
				case 0x000E:
					OP_8xyE(opcode);
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					break;
			}
			break;
		case 0x9000:
			OP_9xy0(opcode);
			pc += 2;
			break;
		case 0xA000:
			OP_Annn(opcode);
			pc += 2;
			break;
		case 0xB000:
			OP_Bnnn(opcode);
			break;
		case 0xC000:
			OP_Cxkk(opcode);
			pc += 2;
			break;
		case 0xD000:
			OP_Dxyn(opcode);
			pc += 2;
			break;
		case 0xE000:
			switch (opcode & 0x00FF) {
				case 0x009E:
					OP_Ex9E(opcode);
					pc += 2;
					break;
					case 0x00A1:
					OP_ExA1(opcode);
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
				case 0x0007:
					OP_Fx07(opcode);
					pc += 2;
					break;
				case 0x000A:
					OP_Fx0A(opcode);
					break;
				case 0x0015:
					OP_Fx15(opcode);
					pc += 2;
					break;
				case 0x0018:
					OP_Fx18(opcode);
					pc += 2;
					break;
				case 0x001E:
					OP_Fx1E(opcode);
					pc += 2;
					break;
				case 0x0029:
					OP_Fx29(opcode);
					pc += 2;
					break;
				case 0x0033: 
					OP_Fx33(opcode);
                    pc += 2;
                    break;
                case 0x0055:
                    OP_Fx55(opcode);
                    pc += 2;
                    break;
                case 0x0065:
                    OP_Fx65(opcode);
                    pc += 2;
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
			}
			break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
            break;
	}

	if (delay_timer > 0) {
        delay_timer--;
    }

    if (sound_timer > 0) {
        sound_flag = 1;
        sound_timer--;
    }
}