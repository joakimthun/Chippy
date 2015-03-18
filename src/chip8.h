#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
	unsigned char  display_buffer[64 * 32];	// W * H, 2048 pixels
	unsigned char  key_state[16];			// 16 keys which range from 0 to F
											
	unsigned short program_counter;			// Program counter
	unsigned short opcode;					// Current opcode
	unsigned short I;						// Address register
	unsigned short stack_pointer;			// Stack pointer
											
	unsigned char  V[16];					// 16 8-bit registers, V0 to VF
	unsigned short stack[16];				
	unsigned char  memory[4096];			
											
	unsigned char  delay_timer;				// Delay timer
	unsigned char  sound_timer;				// Sound timer		

} Chip8;					

Chip8* createChip();
void destroyChip(Chip8* chip);
int loadROM(Chip8* chip, const char* filename);
void emulate(Chip8* chip);

#endif