#ifndef _MEMORY_DEF_H_
#define _MEMORY_DEF_H_
#include <stdint.h>

#ifdef M_DEBUG 
#define MEMORY_DEBUG(...) printf(__VA_ARGS__)
#else
#define MEMORY_DEBUG(...)
#endif

typedef struct {
	
	uint8_t* bootrom;
	uint8_t* rom;

	uint8_t* ram;
	uint8_t* vram;
	uint8_t* topram;

	uint8_t* sprite_attrib;

	uint8_t p1;
	uint8_t sb;
	uint8_t sc;

	//Should be reset on write
	uint8_t div;

	uint8_t tima;
	uint8_t tma;
	uint8_t tac;

	uint8_t lcdc;
	uint8_t stat;
	uint8_t scy;
	uint8_t scx;

	uint8_t ly;
	uint8_t lyc;

	uint8_t bgp;
	uint8_t obp0;
	uint8_t obp1;


	//Sound mode
	uint8_t nr11;
	uint8_t nr12;

	uint8_t nr50;
	uint8_t nr51;
	uint8_t nr52;

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