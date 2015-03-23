#include <SDL.h>
#include <stdio.h>
#include <math.h>

#include "chip8.h"
#include "timer.h"

// Function prototypes
int initialize_sdl(int screen_width, int screen_height);
void destroy_sdl();
void render(Chip8* chip);
void put_pixel(int x, int y, Uint32 pixel);
void audio_callback(void *userdata, Uint8 *stream, int len);
void debug_keys(Chip8* chip);

SDL_Window* window = NULL;
SDL_Surface* window_surface = NULL;
SDL_Surface* render_surface = NULL;

// Length of the audio file
Uint32 wav_length = 0;
// Audio file buffer
Uint8 *wav_buffer = NULL;
// Audio file spec
SDL_AudioSpec wav_spec = { 0 };
// Audio file
char* audio_file = "beep.wav";
// Current position in the audio buffer
Uint8* audio_position;
// Remaining audio length
Uint32 audio_length; 

int resolution_scale = 10;

int main(int argc, char* argv[])
{
	if (initialize_sdl(64 * resolution_scale, 32 * resolution_scale) > 0)
	{
		return 1;
	}

	char* rom_name = argv[1];

	Chip8* chip = create_chip();
	if (chip == NULL)
	{
		return 1;
	}

	if (load_rom(chip, rom_name) != 0)
	{
		return 1;
	}

	SDL_Event e;
	Uint8 running = 1;
	Timer* timer = create_timer();

	while (running > 0)
	{
		set_ticks(timer);

		// Handle the events in the event queue
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				running = 0;
			}
		
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			{
				running = 0;
			}
		
			handle_event(e, chip);
		}

		emulate(chip);

		if (chip->sound_timer == 1)
		{
			printf("A beep should have been played! \n");
		}

		if (chip->redraw > 0)
		{
			render(chip);
			SDL_UpdateWindowSurface(window);
		}

		Uint32 ticks = get_ticks(timer);
		
		Uint32 sleep_time = 8 - ticks;

		if (sleep_time < 16)
			SDL_Delay(sleep_time);

		// printf("Ticks: %d \n", sleep_time);
	}

	destroy_timer(timer);
	destroy_chip(chip);
	destroy_sdl();
	
	return 0;
}

void render(Chip8* chip)
{
	for (int i = 0; i < 2048; i++)
	{
		Uint32 color;
		if (chip->display_buffer[i] == 0)
			color = SDL_MapRGB(window_surface->format, 0, 0, 0);
		else
			color = SDL_MapRGB(window_surface->format, 255, 255, 255);

		int x = i % 64;
		int y = floor(i / 64);

		// x, y , w, h
		SDL_Rect fillRect = { x * resolution_scale, y * resolution_scale, 10, 10 };
		SDL_FillRect(window_surface, &fillRect, color);
	}

	chip->redraw = 0;
}

void debug_keys(Chip8* chip)
{
	if (chip->key_state[0] == 1)
		printf("Key: 1");

	if (chip->key_state[1] == 1)
		printf("Key: 2");

	if (chip->key_state[2] == 1)
		printf("Key: 3");

	if (chip->key_state[3] == 1)
		printf("Key: 4");

	if (chip->key_state[4] == 1)
		printf("Key: 5");

	if (chip->key_state[5] == 1)
		printf("Key: 6");

	if (chip->key_state[6] == 1)
		printf("Key: 7");

	if (chip->key_state[7] == 1)
		printf("Key: 8");

	if (chip->key_state[8] == 1)
		printf("Key: q");

	if (chip->key_state[9] == 1)
		printf("Key: w");

	if (chip->key_state[10] == 1)
		printf("Key: e");

	if (chip->key_state[11] == 1)
		printf("Key: r");

	if (chip->key_state[12] == 1)
		printf("Key: t");

	if (chip->key_state[13] == 1)
		printf("Key: y");

	if (chip->key_state[14] == 1)
		printf("Key: u");

	if (chip->key_state[15] == 1)
		printf("Key: i");
}

void put_pixel(int x, int y, Uint32 pixel_value)
{
	Uint32 *pixels = (Uint32*)window_surface->pixels;
	pixels[(y * window_surface->w) + x] = pixel_value;
}

int initialize_sdl(int screen_width, int screen_height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
		window = SDL_CreateWindow("Chippy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("A window could not be created! SDL_Error: %s\n", SDL_GetError());
			return 1;
		}
	}

	window_surface = SDL_GetWindowSurface(window);
	if (window_surface == NULL)
	{
		printf("A window surface could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	//if (SDL_LoadWAV(audio_file, &wav_spec, &wav_buffer, &wav_length) == NULL)
	//{
	//	printf("Could not load %s! SDL_Error: %s\n", audio_file, SDL_GetError());
	//	return 1;
	//}
	//
	//wav_spec.callback = audio_callback;
	//wav_spec.userdata = NULL;
	//
	//// Copy the sound buffer
	//audio_position = wav_buffer;
	//// Copy the file length
	//audio_length = wav_length;
	//
	//// Open the audio device
	//if (SDL_OpenAudio(&wav_spec, NULL) < 0)
	//{
	//	printf("Couldn't open the audio: %s\n", SDL_GetError());
	//	return 1;
	//}

	return 0;
}

void audio_callback(void *userdata, Uint8 *stream, int len) {

	if (audio_length == 0)
		return;

	len = len > audio_length ? audio_length : len;

	SDL_MixAudio(stream, audio_position, len, SDL_MIX_MAXVOLUME);

	audio_position += len;
	audio_length -= len;
}

void destroy_sdl()
{
	//SDL_CloseAudio();
	//SDL_FreeWAV(wav_buffer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
