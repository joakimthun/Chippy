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
	switch (chip->opcode & 0x000F)
	{
		case 0x0000: // 0x00E0: Clears the screen
			for (int i = 0; i < 2048; i++)
				chip->display_buffer[i] = 0x0;
			chip->redraw = 1;
			chip->program_counter += 2;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			if (chip->stack_pointer == 0)
				printf("Stack underflow!");

			chip->stack_pointer--;
			chip->program_counter = chip->stack[chip->stack_pointer];	// Set the address on the stack as the program counter					
			chip->program_counter += 2;
			break;

		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void X_1(Chip8* chip)
{
	// 0x1NNN: Jumps to address NNN
	// We don't need to increment the program counter here since where jumping to a specified address
	chip->program_counter = chip->opcode & 0x0FFF;
}

void X_2(Chip8* chip)
{
	// 0x2NNN: Calls subroutine at NNN
	chip->stack[chip->stack_pointer] = chip->program_counter;	// Pu the current address on the stack
	chip->stack_pointer++;										// Increment the stack pointer
	chip->program_counter = chip->opcode & 0x0FFF;				// Set the program counter to the address at NNN
}

void X_3(Chip8* chip)
{
	// 0x3XNN: Skips the next instruction if VX equals NN
	unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
	unsigned char nn = (chip->opcode & 0x00FF);
	if (chip->V[register_index] == nn)
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_4(Chip8* chip)
{
	// 0x4XNN: Skips the next instruction if VX doesn't equal NN
	unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
	unsigned char nn = (chip->opcode & 0x00FF);
	if (chip->V[register_index] != nn)
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_5(Chip8* chip)
{
	// 0x5XY0: Skips the next instruction if VX equals VY
	unsigned char register_index_x = (chip->opcode & 0x0F00) >> 8;
	unsigned char register_index_y = (chip->opcode & 0x00F0) >> 4;
	if (chip->V[register_index_x] == chip->V[register_index_y])
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_6(Chip8* chip)
{
	// 0x6XNN: Sets VX to NN
	unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
	unsigned char nn = chip->opcode & 0x00FF;
	chip->V[register_index] = nn;
	chip->program_counter += 2;
}

void X_7(Chip8* chip)
{
	// 0x7XNN: Adds NN to VX
	unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
	unsigned char nn = chip->opcode & 0x00FF;
	chip->V[register_index] += nn;
	chip->program_counter += 2;
}

void X_8(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_9(Chip8* chip)
{
	// 0x9XY0: Skips the next instruction if VX doesn't equal VY
	unsigned char register_index_x = (chip->opcode & 0x0F00) >> 8;
	unsigned char register_index_y = (chip->opcode & 0x00F0) >> 4;
	if (chip->V[register_index_x] != chip->V[register_index_y])
		chip->program_counter += 4;
	else
		chip->program_counter += 2;
}

void X_A(Chip8* chip)
{
	// ANNN: Sets I to the address NNN
	unsigned short nnn = chip->opcode & 0x0FFF;
	chip->I = nnn;
	chip->program_counter += 2;
}

void X_B(Chip8* chip)
{
	// BNNN: Jumps to the address NNN plus V0
	// We don't need to increment the program counter here since where jumping to a specified address
	unsigned short nnn = chip->opcode & 0x0FFF;
	chip->program_counter = nnn + chip->V[0];
}

void X_C(Chip8* chip)
{
	// CXNN: Sets VX to a random number, masked/"anded" by NN
	// Seed the rand() function
	srand(time(NULL));
	// Get a random value between 0 and 255
	unsigned char random_value = rand() % 0xFF;
	unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
	unsigned char nn = chip->opcode & 0x00FF;
	chip->V[register_index] = random_value & nn;

	chip->program_counter += 2;
}

void X_D(Chip8* chip)
{
	printf("Unsupported opcode: 0x%X\n", chip->opcode);
	chip->program_counter += 2;
}

void X_E(Chip8* chip)
{
	switch (chip->opcode & 0x00FF)
	{
		case 0x009E: { // EX9E: Skips the next instruction if the key stored in VX is pressed
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			unsigned char register_value = chip->V[register_index];
			if (chip->key_state[register_value] != 0)
				chip->program_counter += 4;
			else
				chip->program_counter += 2;
			break;
		}
		case 0x00A1: { // EXA1: Skips the next instruction if the key stored in VX isn't pressed
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			unsigned char register_value = chip->V[register_index];
			if (chip->key_state[register_value] == 0)
				chip->program_counter += 4;
			else
				chip->program_counter += 2;
			break;
		}
		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void X_F(Chip8* chip)
{
	switch (chip->opcode & 0x00FF)
	{
		case 0x0007: { // FX07: Sets VX to the value of the delay timer
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			chip->V[register_index] = chip->delay_timer;
			chip->program_counter += 2;
			break;
		}
		case 0x000A: { // FX0A: A key press is awaited, and then stored in VX		
			unsigned char key_was_pressed = 0;
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;

			while (1)
			{
				for (unsigned char i = 0; i < 16; i++)
				{
					if (chip->key_state[i] != 0)
					{
						chip->V[register_index] = i;
						key_was_pressed = 1;
					}
				}

				if (key_was_pressed != 0)
					break;
			}
			
			chip->program_counter += 2;
			break;
		}
		case 0x0015: { // FX15: Sets the delay timer to VX
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			chip->delay_timer = chip->V[register_index];
			chip->program_counter += 2;
			break;
		}
		case 0x0018: { // FX18: Sets the sound timer to VX
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			chip->sound_timer = chip->V[register_index];
			chip->program_counter += 2;
			break;
		}
		case 0x001E: { // FX1E: Adds VX to I
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			// VF is set to 1 when range overflow(I + VX > 0xFFF), and 0 when there isn't.
			// This is undocumented feature of the CHIP-8 and used by Spacefight 2091! game.
			if (chip->I + chip->V[register_index] > 0xFFF)	
				chip->V[0xF] = 1;
			else
				chip->V[0xF] = 0;

			chip->I += chip->V[register_index];
			chip->program_counter += 2;
			break;
		}
		case 0x0029: { // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
			chip->program_counter += 2;
			break;
		}
		case 0x0033: { // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			// Credit: http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;

			chip->memory[chip->I] = chip->V[register_index] / 100;
			chip->memory[chip->I + 1] = (chip->V[register_index] / 10) % 10;
			chip->memory[chip->I + 2] = (chip->V[register_index] % 100) % 10;
			chip->program_counter += 2;
			break;
		}
		case 0x0055: { // FX55: Stores V0 to VX in memory starting at address I					
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			for (unsigned char i = 0; i <= register_index; i++)
				chip->memory[chip->I + i] = chip->V[i];

			// On the original interpreter, when the operation is done, I = I + X + 1. On current implementations, I is left unchanged.
			// Include this?
			//chip->I += register_index + 1;

			chip->program_counter += 2;
			break;
		}
		case 0x0065: { // FX65: Fills V0 to VX with values from memory starting at address I
			unsigned char register_index = (chip->opcode & 0x0F00) >> 8;
			for (unsigned char i = 0; i <= register_index; i++)
				chip->V[i] = chip->memory[chip->I + i];

			// On the original interpreter, when the operation is done, I = I + X + 1. On current implementations, I is left unchanged.
			// Include this?
			//chip->I += register_index + 1;

			chip->program_counter += 2;
			break;
		}
		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void fetchOpcode(Chip8* chip)
{
	// Each opcode is 2 bytes long so we need to grab them from the memory byte by byte and then merge them together
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

	chip->program_counter = 0x200;		// Program counter starts at 0x200 (Program start adress)
	chip->opcode = 0;
	chip->I = 0;
	chip->stack_pointer = 0;

	for (int i = 0; i < 2048; i++)
		chip->display_buffer[i] = 0;

	for (int i = 0; i < 16; i++)
		chip->stack[i] = 0;

	for (int i = 0; i < 16; i++)
		chip->key_state[i] = chip->V[i] = 0;

	for (int i = 0; i < 4096; i++)
		chip->memory[i] = 0;

	for (int i = 0; i < 80; i++)
		chip->memory[i] = fontset[i];

	// Reset timers
	chip->delay_timer = 0;
	chip->sound_timer = 0;

	chip->redraw = 1;

	return chip;
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