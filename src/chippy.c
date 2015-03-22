#include <SDL.h>
#include <stdio.h>
#include <math.h>

#include "chip8.h"

// Function prototypes
int initializeSDL(int screen_width, int screen_height);
void destroySDL();
void renderDisplayBuffer(Chip8* chip);
void putPixel(int x, int y, Uint32 pixel);

SDL_Window* window = NULL;
SDL_Surface* window_surface = NULL;
SDL_Surface* render_surface = NULL;

int main(int argc, char* args[])
{
	if (initializeSDL(640, 320) > 0)
	{
		return 1;
	}

	char* file = "roms//pong2.c8";

	Chip8* chip = createChip();
	if (chip == NULL)
	{
		return 1;
	}

	if (loadROM(chip, file) != 0)
	{
		return 1;
	}

	while (1) 
	{
		emulate(chip);
		if (chip->redraw > 0)
		{
			renderDisplayBuffer(chip);
			SDL_UpdateWindowSurface(window);
		}

		// Add something that slows this down
	}

	destroyChip(chip);
	destroySDL();
	
	return 0;
}

void renderDisplayBuffer(Chip8* chip)
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
		SDL_Rect fillRect = { x * 10, y * 10, 10, 10 };
		SDL_FillRect(window_surface, &fillRect, color);
	}
}

void putPixel(int x, int y, Uint32 pixel_value)
{
	Uint32 *pixels = (Uint32*)window_surface->pixels;
	pixels[(y * window_surface->w) + x] = pixel_value;
}

int initializeSDL(int screen_width, int screen_height)
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

void destroySDL()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}
