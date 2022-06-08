#include "common.h"
#include "cpu.h"
#include "ui.h"
#include <nfd.h>
#include <time.h>
#include "audio.h"

int main() {
    NFD_Init();
    srand((unsigned int) time(NULL));
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = { { "Chip8 ROM", "c8,ch8" }};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
    if (result == NFD_OKAY) {
        puts("Success!");
    }
    else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
        exit(-1);
    }
    else {
        printf("Error: %s\n", NFD_GetError());
        exit(-1);
    }

    printf("OPENED: %s\n", (char*)outPath);

    load_font();

    load_rom((char*)outPath);

    ui_init();

    while(1) {
        cpu_cycle();
        input(keypad);

        if(is_quit()) {
            printf("Closed\n");
            break;
        }

        if(draw_flag) {
            draw_stuff(video);
        }

        if(sound_flag) {
            beep();
        }
        
        delay(3);
    }

    NFD_Quit();
    stop_display();
    stop_audio();

    return 0;
}