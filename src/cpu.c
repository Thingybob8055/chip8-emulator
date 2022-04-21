#include "cpu.h"
#include <time.h>
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
    srand((unsigned int) time(NULL));
    for (int i = 0; i < 80; i++) {
        memory[i] = fontset[i];
    }
}

void load_rom(char* filename) {
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);

    long size = ftell(fp);
    rewind(fp);

    uint8_t* buffer = (uint8_t*)malloc((size + 1)*sizeof(uint8_t));
    fread(buffer, size, 1, fp);
    fclose(fp);

    for (int i = 0; i < size; i++) {
        memory[512 + i] = buffer[i]; // first 512 bytes are reserved
    }

    free(buffer);

}

void cpu_cycle() {
	draw_flag = 0;
	sound_flag = 0;
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
	uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x00FF) {
				case 0x00E0:
					 for (int i = 0; i < 64 * 32; i++) {
                        video[i] = 0;
                    }
					pc += 2;
					break;
				case 0x00EE:
					pc = stack[sp];
					sp--;
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					break;
			}
			break;
		case 0x1000:
			pc = opcode & 0xFFF;
			break;
		case 0x2000:
			sp++;
			stack[sp] = pc;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			if (registers[Vx] == (opcode & 0x00FF)) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0x4000:
			if (registers[Vx] != (opcode & 0x00FF)) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0x5000:
			if (registers[Vx] == registers[Vy]) {
				pc += 4;
			} 
			else {
				pc += 2;
			}
			break;
		case 0x6000:
			registers[Vx] = opcode & 0x00FFu;
			pc += 2;
			break;
		case 0x7000:
			registers[Vx] += opcode & 0x00FF;
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F){
				case 0x0000:
					registers[Vx] = registers[Vy];
					pc += 2;
					break;
				case 0x0001:
					registers[Vx] |= registers[Vy];
					pc += 2;
					break;
				case 0x0002:
					registers[Vx] &= registers[Vy];
					pc += 2;
					break;
				case 0x0003:;
					registers[Vx] ^= registers[Vy];
					pc += 2;
					break;
				case 0x0004:
					registers[0xF] = 0;
					if( (registers[Vx] + registers[Vy]) >0xFF ) {
						registers[0xF] = 1;
					}
					registers[Vx] +=  registers[Vy];
					pc += 2;
					break;
				case 0x0005:
					if (registers[Vx] > registers[Vy]) {
						registers[0xF] = 1;
					}
					else {
						registers[0xF] = 0;
					}

					registers[Vx] -= registers[Vy];
					pc += 2;
					break;
				case 0x0006:
					// Save LSB in VF
					registers[0xF] = (registers[Vx] & 0x1);

					registers[Vx] >>= 1;
					pc += 2;
					break;
				case 0x0007:
					if (registers[Vy] > registers[Vx]) {
						registers[0xF] = 1;
					}
					else {
						registers[0xF] = 0;
					}

					registers[Vx] = registers[Vy] - registers[Vx];
					pc += 2;
					break;
				case 0x000E:
					// Save MSB in VF
					registers[0xF] = (registers[Vx] & 0x80) >> 7u;
					registers[Vx] <<= 1;
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					break;
			}
			break;
		case 0x9000:
			if (registers[Vx] != registers[Vy]) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0xA000:
			INDEX = opcode & 0xFFF;
			pc += 2;
			break;
		case 0xB000:
			pc = registers[0] + (opcode & 0xFFF);
			break;
		case 0xC000:
			registers[Vx] = rand() & (opcode & 0x00FF);
			pc += 2;
			break;
		case 0xD000:
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
			pc += 2;
			break;
		case 0xE000:
			switch (opcode & 0x00FF) {
				case 0x009E:
					if(keypad[registers[Vx]]) {
						pc += 2;
					}
					pc += 2;
					break;
					case 0x00A1:
					if(!keypad[registers[Vx]]) {
						pc += 2;
					}
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
					registers[Vx] = delay_timer;
					pc += 2;
					break;
				case 0x000A:
					for(int i = 0; i<16; i++) {
						if(keypad[i]){
							registers[Vx] = i;
							pc += 2;
                            break;
						}
					}
					break;
				case 0x0015:
					delay_timer = registers[Vx];
					pc += 2;
					break;
				case 0x0018:
					sound_timer = registers[Vx];
					pc += 2;
					break;
				case 0x001E:
					INDEX += registers[Vx];
					pc += 2;
					break;
				case 0x0029:
					INDEX = registers[Vx] * 5;
					pc += 2;
					break;
				case 0x0033: // 0xFX33: store BCD(V[x]) in I, I+1, I+2
                    // The interpreter takes the decimal value of V[x],
                    // and places the hundreds digit in memory at location in I,
                    // the tens digit at location I+1, and the ones digit at 
                    // location I+2.
                    memory[INDEX] = registers[Vx] / 100;
                    memory[INDEX + 1] = registers[Vx] % 100 / 10;
                    memory[INDEX + 2] = registers[Vx] % 10;
                    pc += 2;
                    break;
                case 0x0055: // 0xFX55: store V[0] through V[x] in memory starting at location I
                    for (int i = 0; i <= Vx; i++) {
                        memory[INDEX + i] = registers[i];
                    }
                    pc += 2;
                    break;
                case 0x0065: // 0xFX65: load V[0] through V[x] from memory starting at location I
                    for (int i = 0; i <= Vx; i++) {
                        registers[i] = memory[INDEX + i];
                    }
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

    // beep and update sound timer
    if (sound_timer > 0) {
        sound_flag = 1;
        sound_timer--;
    }
}