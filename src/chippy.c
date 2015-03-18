#include <stdio.h>

#include "chip8.h"

int main(int argc, char **argv)
{
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
	
	return 0;
}