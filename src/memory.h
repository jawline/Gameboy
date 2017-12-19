#ifndef _MEMORY_DEF_H_
#define _MEMORY_DEF_H_
#include <stdint.h>

#ifdef M_DEBUG 
#define MEMORY_DEBUG(...) printf(__VA_ARGS__)
#else
#define MEMORY_DEBUG(...)
#endif

#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LX 0xFF45

typedef struct {
	
	uint8_t* bootrom;
	uint8_t* rom;

	uint8_t* ram;
	uint8_t* vram;
	uint8_t* topram;

	uint8_t* sprite_attrib;

	uint8_t* ioram;

	uint8_t interrupts;
	uint8_t interrupts_enabled;

	unsigned bootrom_enabled:1;
} memory;

void memory_init(memory* mem, uint8_t* rom, uint8_t* bootrom);

uint8_t mem_get(memory* mem, uint16_t off);
uint16_t mem_get16(memory* mem, uint16_t off);

void mem_set(memory* mem, uint16_t off, uint8_t v);
void mem_set16(memory* mem, uint16_t off, uint16_t v);

#endif //_MEMORY_DEF_H_