#include <SDL2/SDL.h>
#include "ui.h"

SDL_Window *window;
SDL_Renderer *renderer;

bool quit;

void delay(uint32_t ms) {
    SDL_Delay(ms);
}

void ui_init() {
    quit = false;

	SDL_Init(SDL_INIT_VIDEO);
    printf("SDL_INIT\n");
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    }
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH*8, SCREEN_HEIGHT*8, 0, &window, &renderer);
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH*8, SCREEN_HEIGHT*8);
	SDL_SetWindowTitle(window, "CHIP-8 EMULATOR");
}

void draw_stuff(uint32_t* video) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (video[x + (y * 64)]) {
                SDL_Rect rect;
                rect.x = x * 8;
                rect.y = y * 8;
                rect.w = 8;
                rect.h = 8;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void input(uint8_t* key) {
	SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
			case SDL_QUIT: quit = true; break;
			case SDL_KEYDOWN: {
				switch (event.key.keysym.sym)
				{
					case SDLK_x: key[0] = 1; break;
					case SDLK_1: key[1] = 1; break;
					case SDLK_2: key[2] = 1; break;
					case SDLK_3: key[3] = 1; break;
					case SDLK_q: key[4] = 1; break;
					case SDLK_w: key[5] = 1; break;
					case SDLK_e: key[6] = 1; break;
					case SDLK_a: key[7] = 1; break;
					case SDLK_s: key[8] = 1; break;
					case SDLK_d: key[9] = 1; break;
					case SDLK_z: key[0xA] = 1; break;
					case SDLK_c: key[0xB] = 1; break;
					case SDLK_4: key[0xC] = 1; break;
					case SDLK_r: key[0xD] = 1; break;
					case SDLK_f: key[0xE] = 1;  break;
					case SDLK_v: key[0xF] = 1;  break;
				}
			} break;
			case SDL_KEYUP: {
				switch (event.key.keysym.sym)
				{
					case SDLK_x: key[0] = 0; break;
					case SDLK_1: key[1] = 0; break;
					case SDLK_2: key[2] = 0; break;
					case SDLK_3: key[3] = 0; break;
					case SDLK_q: key[4] = 0; break;
					case SDLK_w: key[5] = 0; break;
					case SDLK_e: key[6] = 0; break;
					case SDLK_a: key[7] = 0; break;
					case SDLK_s: key[8] = 0; break;
					case SDLK_d: key[9] = 0; break;
					case SDLK_z: key[0xA] = 0; break;
					case SDLK_c: key[0xB] = 0; break;
					case SDLK_4: key[0xC] = 0; break;
					case SDLK_r: key[0xD] = 0; break;
					case SDLK_f: key[0xE] = 0; break;
					case SDLK_v: key[0xF] = 0; break;
				}
			} break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    quit = true;
                }
            break;
		}
    }
}

bool is_quit() {
	return quit;
}


void stop_display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}
