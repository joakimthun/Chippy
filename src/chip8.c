#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes
void fetch_opcode(Chip8*);
void next_opcode(Chip8* chip);
void skip_next_opcode(Chip8* chip);
void x_0(Chip8*);
void x_1(Chip8*);
void x_2(Chip8*);
void x_3(Chip8*);
void x_4(Chip8*);
void x_5(Chip8*);
void x_6(Chip8*);
void x_7(Chip8*);
void x_8(Chip8*);
void x_9(Chip8*);
void x_a(Chip8*);
void x_b(Chip8*);
void x_c(Chip8*);
void x_d(Chip8*);
void x_e(Chip8*);
void x_f(Chip8*);

Uint8 fontset[80] =
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
	x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7,
	x_8, x_9, x_a, x_b, x_c, x_d, x_e, x_f
};

void emulate(Chip8* chip)
{
	fetch_opcode(chip);
	
	/*
	Example:
		Opcode = FFFF
		(chip->opcode & 0xF000) --> F000 --> (F000 >> 12) --> 000F
		Invoke the function at index 15 in the handler array
	*/

	Uint8 index = (chip->opcode & 0xF000) >> 12;
	opcode_handlers[index](chip);

	// Update the sound and delay timer
	if (chip->sound_timer > 0)
	{
		if (chip->sound_timer == 1)
			// Play a sound

		chip->sound_timer--;
	}

	if (chip->delay_timer > 0)
		chip->delay_timer--;
}

void x_0(Chip8* chip)
{
	switch (chip->opcode & 0x000F)
	{
		case 0x0000: // 0x00E0: Clears the screen
			for (int i = 0; i < 2048; i++)
				chip->display_buffer[i] = 0x0;
			chip->redraw = 1;
			//next_opcode(chip);
			next_opcode(chip);
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			if (chip->stack_pointer == 0)
				printf("Stack underflow!");

			chip->stack_pointer--;
			chip->program_counter = chip->stack[chip->stack_pointer];	// Set the address on the stack as the program counter					
			next_opcode(chip);
			break;

		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void x_1(Chip8* chip)
{
	// 0x1NNN: Jumps to address NNN
	// We don't need to increment the program counter here since where jumping to a specified address
	chip->program_counter = chip->opcode & 0x0FFF;
}

void x_2(Chip8* chip)
{
	// 0x2NNN: Calls subroutine at NNN
	chip->stack[chip->stack_pointer] = chip->program_counter;	// Pu the current address on the stack
	chip->stack_pointer++;										// Increment the stack pointer
	chip->program_counter = chip->opcode & 0x0FFF;				// Set the program counter to the address at NNN
}

void x_3(Chip8* chip)
{
	// 0x3XNN: Skips the next instruction if VX equals NN
	Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
	Uint8 nn = (chip->opcode & 0x00FF);
	if (chip->V[register_index] == nn)
		skip_next_opcode(chip);
	else
		next_opcode(chip);
}

void x_4(Chip8* chip)
{
	// 0x4XNN: Skips the next instruction if VX doesn't equal NN
	Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
	Uint8 nn = (chip->opcode & 0x00FF);
	if (chip->V[register_index] != nn)
		skip_next_opcode(chip);
	else
		next_opcode(chip);
}

void x_5(Chip8* chip)
{
	// 0x5XY0: Skips the next instruction if VX equals VY
	Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
	Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;
	if (chip->V[register_index_x] == chip->V[register_index_y])
		skip_next_opcode(chip);
	else
		next_opcode(chip);
}

void x_6(Chip8* chip)
{
	// 0x6XNN: Sets VX to NN
	Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
	Uint8 nn = chip->opcode & 0x00FF;
	chip->V[register_index] = nn;
	next_opcode(chip);
}

void x_7(Chip8* chip)
{
	// 0x7XNN: Adds NN to VX
	Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
	Uint8 nn = chip->opcode & 0x00FF;
	chip->V[register_index] += nn;
	next_opcode(chip);
}

void x_8(Chip8* chip)
{
	switch (chip->opcode & 0x000F)
	{
		case 0x0000: { // 0x8XY0: Sets VX to the value of VY
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			chip->V[register_index_x] = chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0001: { // 0x8XY1: Sets VX to "VX OR VY"
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			chip->V[register_index_x] |= chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0002: { // 0x8XY2: Sets VX to "VX AND VY"
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			chip->V[register_index_x] &= chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0003: { // 0x8XY3: Sets VX to "VX XOR VY"
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			chip->V[register_index_x] ^= chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0004: { // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			if (chip->V[register_index_y] > (0xFF - chip->V[register_index_x]))
				chip->V[0xF] = 1; // There is a carry
			else
				chip->V[0xF] = 0;

			chip->V[register_index_x] += chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0005: { // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			if (chip->V[register_index_y] > chip->V[register_index_x])
				chip->V[0xF] = 0; // There is a borrow
			else
				chip->V[0xF] = 1;

			chip->V[register_index_x] -= chip->V[register_index_y];

			next_opcode(chip);
			break;
		}
		case 0x0006: { // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;

			chip->V[0xF] = chip->V[register_index_x] & 0x1;
			chip->V[register_index_x] >>= 1;

			next_opcode(chip);
			break;
		}
		case 0x0007: { // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;

			if (chip->V[register_index_x] > chip->V[register_index_y])
				chip->V[0xF] = 0; // There is a borrow
			else
				chip->V[0xF] = 1;

			chip->V[register_index_x] = chip->V[register_index_y] - chip->V[register_index_x];

			next_opcode(chip);
			break;
		}
		case 0x000E: { // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
			Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;

			chip->V[0xF] = chip->V[register_index_x] >> 7;
			chip->V[register_index_x] <<= 1;

			next_opcode(chip);
			break;
		}
		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void x_9(Chip8* chip)
{
	// 0x9XY0: Skips the next instruction if VX doesn't equal VY
	Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
	Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;
	if (chip->V[register_index_x] != chip->V[register_index_y])
		skip_next_opcode(chip);
	else
		next_opcode(chip);
}

void x_a(Chip8* chip)
{
	// ANNN: Sets I to the address NNN
	Uint16 nnn = chip->opcode & 0x0FFF;
	chip->I = nnn;
	next_opcode(chip);
}

void x_b(Chip8* chip)
{
	// BNNN: Jumps to the address NNN plus V0
	// We don't need to increment the program counter here since where jumping to a specified address
	Uint16 nnn = chip->opcode & 0x0FFF;
	chip->program_counter = nnn + chip->V[0];
}

void x_c(Chip8* chip)
{
	// CXNN: Sets VX to a random number, masked/"anded" by NN
	// Seed the rand() function
	srand(time(NULL));
	// Get a random value between 0 and 255
	Uint8 random_value = rand() % 0xFF;
	Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
	Uint8 nn = chip->opcode & 0x00FF;
	chip->V[register_index] = random_value & nn;

	next_opcode(chip);
}

void x_d(Chip8* chip)
{
	// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
	// Each row of 8 pixels is read as bit-coded starting from memory location I; 
	// I value doesn't change after the execution of this instruction. 
	// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
	// and to 0 if that doesn't happen

	// Credit: http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml

	Uint8 register_index_x = (chip->opcode & 0x0F00) >> 8;
	Uint8 register_index_y = (chip->opcode & 0x00F0) >> 4;
	Uint16 x = chip->V[register_index_x];
	Uint16 y = chip->V[register_index_y];
	Uint16 height = chip->opcode & 0x000F;
	Uint16 pixel;

	chip->V[0xF] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = chip->memory[chip->I + yline];
		for (int xline = 0; xline < 8; xline++)
		{
			if ((pixel & (0x80 >> xline)) != 0)
			{
				if (chip->display_buffer[(x + xline + ((y + yline) * 64))] == 1)
				{
					chip->V[0xF] = 1;
				}
				chip->display_buffer[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	chip->redraw = 1;
	next_opcode(chip);
}

void x_e(Chip8* chip)
{
	switch (chip->opcode & 0x00FF)
	{
		case 0x009E: { // EX9E: Skips the next instruction if the key stored in VX is pressed
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_value = chip->V[register_index];
			if (chip->key_state[register_value] != 0)
				skip_next_opcode(chip);
			else
				next_opcode(chip);
			break;
		}
		case 0x00A1: { // EXA1: Skips the next instruction if the key stored in VX isn't pressed
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			Uint8 register_value = chip->V[register_index];
			if (chip->key_state[register_value] == 0)
				skip_next_opcode(chip);
			else
				next_opcode(chip);
			break;
		}
		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void x_f(Chip8* chip)
{
	switch (chip->opcode & 0x00FF)
	{
		case 0x0007: { // FX07: Sets VX to the value of the delay timer
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			chip->V[register_index] = chip->delay_timer;
			next_opcode(chip);
			break;
		}
		case 0x000A: { // FX0A: A key press is awaited, and then stored in VX		
			Uint8 key_was_pressed = 0;
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;

			while (1)
			{
				for (Uint8 i = 0; i < 16; i++)
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
			
			next_opcode(chip);
			break;
		}
		case 0x0015: { // FX15: Sets the delay timer to VX
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			chip->delay_timer = chip->V[register_index];
			next_opcode(chip);
			break;
		}
		case 0x0018: { // FX18: Sets the sound timer to VX
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			chip->sound_timer = chip->V[register_index];
			next_opcode(chip);
			break;
		}
		case 0x001E: { // FX1E: Adds VX to I
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			// VF is set to 1 when range overflow(I + VX > 0xFFF), and 0 when there isn't.
			// This is undocumented feature of the CHIP-8 and used by Spacefight 2091! game.
			if (chip->I + chip->V[register_index] > 0xFFF)	
				chip->V[0xF] = 1;
			else
				chip->V[0xF] = 0;

			chip->I += chip->V[register_index];
			next_opcode(chip);
			break;
		}
		case 0x0029: { // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			chip->I = chip->V[register_index] * 5;
			next_opcode(chip);
			break;
		}
		case 0x0033: { // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			// Credit: http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;

			chip->memory[chip->I] = chip->V[register_index] / 100;
			chip->memory[chip->I + 1] = (chip->V[register_index] / 10) % 10;
			chip->memory[chip->I + 2] = (chip->V[register_index] % 100) % 10;
			next_opcode(chip);
			break;
		}
		case 0x0055: { // FX55: Stores V0 to VX in memory starting at address I					
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			for (Uint8 i = 0; i <= register_index; i++)
				chip->memory[chip->I + i] = chip->V[i];

			// On the original interpreter, when the operation is done, I = I + X + 1. On current implementations, I is left unchanged.
			// Include this?
			//chip->I += register_index + 1;

			next_opcode(chip);
			break;
		}
		case 0x0065: { // FX65: Fills V0 to VX with values from memory starting at address I
			Uint8 register_index = (chip->opcode & 0x0F00) >> 8;
			for (Uint8 i = 0; i <= register_index; i++)
				chip->V[i] = chip->memory[chip->I + i];

			// On the original interpreter, when the operation is done, I = I + X + 1. On current implementations, I is left unchanged.
			// Include this?
			//chip->I += register_index + 1;

			next_opcode(chip);
			break;
		}
		default:
			printf("Unsupported opcode: 0x%X\n", chip->opcode);
	}
}

void fetch_opcode(Chip8* chip)
{
	// Each opcode is 2 bytes long so we need to grab them from the memory byte by byte and then merge them together
	chip->opcode = chip->memory[chip->program_counter] << 8 | chip->memory[chip->program_counter + 1];
	printf("Opcode: 0x%X\n", chip->opcode);
}

Chip8* create_chip()
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

void destroy_chip(Chip8* chip)
{
	free(chip);
}

void next_opcode(Chip8* chip)
{
	chip->program_counter += 2;
}

void skip_next_opcode(Chip8* chip)
{
	chip->program_counter += 4;
}

void handle_event(SDL_Event e, Chip8* chip)
{
	// A key was pushed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_1:
				chip->key_state[0] = 1;
				break;
			case SDLK_2:
				chip->key_state[1] = 1;
				break;
			case SDLK_3:
				chip->key_state[2] = 1;
				break;
			case SDLK_4:
				chip->key_state[3] = 1;
				break;
			case SDLK_5:
				chip->key_state[4] = 1;
				break;
			case SDLK_6:
				chip->key_state[5] = 1;
				break;
			case SDLK_7:
				chip->key_state[6] = 1;
				break;
			case SDLK_8:
				chip->key_state[7] = 1;
				break;
			case SDLK_q:
				chip->key_state[8] = 1;
				break;
			case SDLK_w:
				chip->key_state[9] = 1;
				break;
			case SDLK_e:
				chip->key_state[10] = 1;
				break;
			case SDLK_r:
				chip->key_state[11] = 1;
				break;
			case SDLK_s:
				chip->key_state[12] = 1;
				break;
			case SDLK_t:
				chip->key_state[13] = 1;
				break;
			case SDLK_u:
				chip->key_state[14] = 1;
				break;
			case SDLK_y:
				chip->key_state[15] = 1;
				break;
			default:
				break;
		}
	}

	// A key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_1:
				chip->key_state[0] = 0;
				break;
			case SDLK_2:
				chip->key_state[1] = 0;
				break;
			case SDLK_3:
				chip->key_state[2] = 0;
				break;
			case SDLK_4:
				chip->key_state[3] = 0;
				break;
			case SDLK_5:
				chip->key_state[4] = 0;
				break;
			case SDLK_6:
				chip->key_state[5] = 0;
				break;
			case SDLK_7:
				chip->key_state[6] = 0;
				break;
			case SDLK_8:
				chip->key_state[7] = 0;
				break;
			case SDLK_q:
				chip->key_state[8] = 0;
				break;
			case SDLK_w:
				chip->key_state[9] = 0;
				break;
			case SDLK_e:
				chip->key_state[10] = 0;
				break;
			case SDLK_r:
				chip->key_state[11] = 0;
				break;
			case SDLK_s:
				chip->key_state[12] = 0;
				break;
			case SDLK_t:
				chip->key_state[13] = 0;
				break;
			case SDLK_u:
				chip->key_state[14] = 0;
				break;
			case SDLK_y:
				chip->key_state[15] = 0;
				break;
			default:
				break;
		}
	}
}

int load_rom(Chip8* chip, const char* filename)
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