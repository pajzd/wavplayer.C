#include <stdio.h>
#include <SDL2/SDL.h>

#define FILE_PATH "/Users/lesupralittoralofbrownnoise/Desktop/vietnamese/MixPre-095.WAV"

// a structure = array of different datatypes

struct AudioData {
    Uint8 *pos;
    Uint32 length;
};

typedef struct AudioData AudioData;

// a function that actually plays the audio
// and copies data of the audio to our device !

void MyAudioCallBack(void *userdata, Uint8 *stream, int streamLength)
{
    AudioData *audio = (AudioData*)userdata;

    if (audio->length == 0) {
        return;
    }

    Uint32 length = (Uint32)streamLength;
    length = (length > audio->length ? audio->length : length);

    SDL_memcpy(stream, audio->pos, length);

    audio->pos += length;
    audio->length -= length;
}

int main(int argc, char **argv)
{
    // Just to make sure SDL is working
	// SDL_Init(SDL_INIT_EVERYTHING); // SDL_INIT_EVERYTHING includes AUDIO but we can write also SDL_INIT_AUDIO

    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec wavSpec; // various data of the wave file : channels, fs, ...
    Uint8 *wavStart; // location and memory of the wave file
    Uint32 wavLength; // the length of the wave file, how big is it

    // load the wave file


    if (SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL) {
        fprintf(stderr, "Error: %s could not be loaded as an audio file\n", FILE_PATH);
        return 1;
    }

    AudioData audio;
    audio.pos = wavStart;
    audio.length = wavLength;

    wavSpec.callback = MyAudioCallBack;
    wavSpec.userdata = &audio;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (device == 0) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        SDL_FreeWAV(wavStart);
        SDL_Quit();
        return 1;
    }

    printf("File: %s has been loaded successfully!\n", FILE_PATH);

    // speelt de eigenlijk audio met SDL_PauseAudioDevice :) !!!

    SDL_PauseAudioDevice(device, 0);

    // 1/10 seconde delay 

    while (audio.length > 0) {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(device); // at the end we close the Audio Device
    SDL_FreeWAV(wavStart); // after file played, we want to free again the location memory & delete it
    SDL_Quit();
    return 0;
}
