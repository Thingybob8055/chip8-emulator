#include "audio.h"
#include "common.h"
#include <math.h>
#include <SDL2/SDL.h>

void audio_callback(void *user_data, unsigned char *raw_buffer, int bytes) {
    double sample_rate = 44100.0;
    int amplitude = 28000;

    Sint16* buffer = (Sint16*) raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int sample_nr = *(int*) user_data;

    for (int i = 0; i < length; i++, sample_nr++) {
        double time = sample_nr / sample_rate;
        buffer[i] = (Sint16)(amplitude * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}
//beep
void beep() {
    int sample_nr = 0;

    SDL_AudioSpec want;
    SDL_zero(want);

    want.freq = 44100; // number of samples per second
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 2048; // buffer-size
    want.callback = audio_callback; // function SDL calls periodically to refill the buffer
    want.userdata = &sample_nr; // counter, keeping track of current sample number

    SDL_AudioSpec have;
    if (SDL_OpenAudio(&want, &have) != 0)
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
    if (want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");

    SDL_PauseAudio(0); // start playing sound
    SDL_Delay(15); // wait while sound is playing
    SDL_PauseAudio(1); // stop playing sound
}

void stop_audio() {
    SDL_CloseAudio();
}