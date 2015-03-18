#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes
void fetchOpcode(Chip8*);
void X_0(Chip8*);
void X_1(Chip8*);
void X_2(Chip8*);
void X_3(Chip8*);
void X_4(Chip8*);
void X_5(Chip8*);
void X_6(Chip8*);
void X_7(Chip8*);
void X_8(Chip8*);
void X_9(Chip8*);
void X_A(Chip8*);
void X_B(Chip8*);
void X_C(Chip8*);
void X_D(Chip8*);
void X_E(Chip8*);
void X_F(Chip8*);

unsigned char fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void (*opcode_handlers[16])(Chip8*) = 
{ 
	X_0, X_1, X_2, X_3, X_4, X_5, X_6, X_7,
	X_8, X_9, X_A, X_B, X_C, X_D, X_E, X_F
};

void emulate(Chip8* chip)
{
	fetchOpcode(chip);
	
	/*
	Example:
		Opcode = FFFF
		(chip->opcode & 0xF000) --> F000 --> (F000 >> 12) --> 000F
		Invoke the function at index 15 in the handler array
	*/

	unsigned char index = (chip->opcode & 0xF000) >> 12;
	opcode_handlers[index](chip);
}

void X_0(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_1(Chip8* chip)
{
	// 0x1NNN: Jumps to address NNN
	chip->program_counter = chip->opcode & 0x0FFF;
}

void X_2(Chip8* chip)
{
	// 0x2NNN: Calls subroutine at NNN.
	chip->stack[chip->stack_pointer] = chip->program_counter;	// Store the current address on the stack
	chip->stack_pointer++;										// Increment the stack pointer
	chip->program_counter = chip->opcode & 0x0FFF;				// Set the program counter to the address at NNN
}

void X_3(Chip8* chip)
{
	// 0x3XNN: Skips the next instruction if VX equals NN
	if (chip->V[(chip->opcode & 0x0F00) >> 8] == (chip->opcode & 0x00FF))
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_4(Chip8* chip)
{
	// 0x4XNN: Skips the next instruction if VX doesn't equal NN
	if (chip->V[(chip->opcode & 0x0F00) >> 8] != (chip->opcode & 0x00FF))
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_5(Chip8* chip)
{
	// 0x5XY0: Skips the next instruction if VX equals VY.
	if (chip->V[(chip->opcode & 0x0F00) >> 8] == chip->V[(chip->opcode & 0x00F0) >> 4])
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_6(Chip8* chip)
{
	// 0x6XNN: Sets VX to NN.
	chip->V[(chip->opcode & 0x0F00) >> 8] = chip->opcode & 0x00FF;
	chip->program_counter += 2;
}

void X_7(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_8(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_9(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_A(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_B(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_C(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_D(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_E(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_F(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void fetchOpcode(Chip8* chip)
{
	chip->opcode = chip->memory[chip->program_counter] << 8 | chip->memory[chip->program_counter + 1];
	printf("Opcode: 0x%X\n", chip->opcode);
}

Chip8* createChip()
{
	Chip8* chip = malloc(sizeof(Chip8));
	if (chip == NULL)
	{
		printf("Could not create chip :(");
		return NULL;
	}

	chip->program_counter = 0x200;		// Program counter starts at 0x200 (Start adress program)
	chip->opcode = 0;
	chip->I = 0;
	chip->stack_pointer = 0;

	for (int i = 0; i < 2048; ++i)
		chip->display_buffer[i] = 0;

	for (int i = 0; i < 16; ++i)
		chip->stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		chip->key_state[i] = chip->V[i] = 0;

	for (int i = 0; i < 4096; ++i)
		chip->memory[i] = 0;

	for (int i = 0; i < 80; ++i)
		chip->memory[i] = fontset[i];

	// Reset timers
	chip->delay_timer = 0;
	chip->sound_timer = 0;

	// Clear screen once
	//drawFlag = true;

	//srand(time(NULL));
}

void destroyChip(Chip8* chip)
{
	free(chip);
}

int loadROM(Chip8* chip, const char* filename)
{
	FILE* pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		fputs("Could not open file \n", stderr);
		return 1;
	}

	// Check file size
	fseek(pFile, 0, SEEK_END);
	long file_size = ftell(pFile);
	rewind(pFile);
	printf("Loading ROM: %s \nFilesize: %d \n", filename, (int)file_size);

	// Allocate memory to contain the whole file
	char* file_buffer = (char*)malloc(sizeof(char) * file_size);
	if (file_buffer == NULL)
	{
		fputs("Could not create the file buffer \n", stderr);
		return 1;
	}

	// Copy the file to the file buffer
	size_t result = fread(file_buffer, 1, file_size, pFile);
	if (result != file_size)
	{
		fputs("The ROM did not load correctly \n", stderr);
		return 1;
	}

	// Copy the file buffer into the memory array
	if ((4096 - 512) > file_size)
	{
		for (int i = 0; i < file_size; ++i)
			chip->memory[i + 512] = file_buffer[i];
	}
	else
		printf("The ROM can not be larger then 4096 bytes \n");

	fclose(pFile);
	free(file_buffer);

	printf("ROM successfully loaded \n");

	return 0;
}