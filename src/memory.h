#ifndef _MEMORY_DEF_H_
#define _MEMORY_DEF_H_
#include <stdint.h>

typedef struct {
	uint8_t* rom;
	uint8_t* ram;
	uint8_t* vram;
	uint8_t* topram;
} memory;

void memory_init(memory* mem, uint8_t* rom);

uint8_t mem_get(memory* mem, uint16_t off);
uint16_t mem_get16(memory* mem, uint16_t off);

void mem_set(memory* mem, uint16_t off, uint8_t v);
void mem_set16(memory* mem, uint16_t off, uint16_t v);

#endif //_MEMORY_DEF_H_