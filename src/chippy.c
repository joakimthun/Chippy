#include <SDL.h>
#include <stdio.h>

#include "chip8.h"

// Function prototypes
int initializeSDL(int screen_width, int screen_height);
void destroySDL();
void renderDisplayBuffer(Chip8* chip);

SDL_Window* window = NULL;
SDL_Surface* window_surface = NULL;
SDL_Surface* render_surface = NULL;

int main(int argc, char* args[])
{
	if (initializeSDL(640, 320) > 0)
	{
		return 1;
	}

	window_surface = SDL_GetWindowSurface(window);
	SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, 0xFF, 0xFF, 0xFF));
	SDL_UpdateWindowSurface(window);

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
		// Add something that slows this down
		emulate(chip);
	}

	destroyChip(chip);
	destroySDL();
	
	return 0;
}

void renderDisplayBuffer(Chip8* chip)
{

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

	return 0;
}

void destroySDL()
{
	SDL_FreeSurface(render_surface);
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
