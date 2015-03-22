#include <SDL.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>

#include "chip8.h"

// Function prototypes
int initialize_sdl(int screen_width, int screen_height);
void destroy_sdl();
void render(Chip8* chip);
void put_pixel(int x, int y, Uint32 pixel);

SDL_Window* window = NULL;
SDL_Surface* window_surface = NULL;
SDL_Surface* render_surface = NULL;
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

	while (running > 0)
	{
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

		if (chip->redraw > 0)
		{
			render(chip);
			SDL_UpdateWindowSurface(window);
		}

		// Add something that slows this down
	}

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
}

void put_pixel(int x, int y, Uint32 pixel_value)
{
	Uint32 *pixels = (Uint32*)window_surface->pixels;
	pixels[(y * window_surface->w) + x] = pixel_value;
}

int initialize_sdl(int screen_width, int screen_height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
	}

	return 0;
}

void destroy_sdl()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}
