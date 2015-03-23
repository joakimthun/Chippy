#ifndef CHIP8_H
#define CHIP8_H

#include <SDL.h>

typedef struct {
	Uint8 display_buffer[64 * 32];	// W * H, 2048 pixels
	Uint8 key_state[16];			// 16 keys, 0 to F
											
	Uint16 program_counter;			// Program counter
	Uint16 opcode;					// Current opcode
	Uint16 I;						// Address register
	Uint16 stack_pointer;			// Stack pointer
											
	Uint8 V[16];					// 16 8-bit registers, V0 to VF
	Uint16 stack[16];
	Uint8 memory[4096];			
											
	Uint8 delay_timer;				// Delay timer
	Uint8 sound_timer;				// Sound timer
	Uint8 redraw;

} Chip8;					

Chip8* create_chip();
void destroy_chip(Chip8* chip);
int load_rom(Chip8* chip, const char* filename);
void emulate(Chip8* chip);
void handle_event(SDL_Event e, Chip8* chip);

#endif