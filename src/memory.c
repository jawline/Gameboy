#include "memory.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_init(memory* mem, uint8_t* rom) {
	memset(mem, 0, sizeof(memory));
	mem->rom = rom;
	
	for (unsigned int i = 0; i < 256; i++) {
		printf("%i:%02X ", i, mem->rom[i]);
	}
	printf("\n");

	mem->ram = malloc(8192);
	mem->vram = malloc(8192);
	mem->topram = malloc(0xFFFF - 0xFF80);
}

uint8_t* ptr(memory* mem, uint16_t off) {
	const uint16_t ROM_END = 0x8000;
	const uint16_t VRAM_END = 0xA000;
	const uint16_t SWITCH_RAM_END = 0xC000;
	const uint16_t MAIN_RAM_END = 0xE000;
	const uint16_t MAIN_RAM_ECHO = 0xFE00;
	const uint16_t END_UNUSED_IO = 0xFF80;

	if (off < ROM_END) {
		return &mem->rom[off];
	} else if (off < VRAM_END) {
		return &mem->vram[off - ROM_END];
	} else if (off < SWITCH_RAM_END) {
		printf("I don't know how switch ram works yet %x\n", off);
		return 0;
	} else if (off < MAIN_RAM_END) {
		return &mem->ram[off - SWITCH_RAM_END];
	} else if (off < MAIN_RAM_ECHO) {
		return &mem->ram[off - MAIN_RAM_END];
	} else if (off < END_UNUSED_IO) {

		switch (off) {
			case 0xFF00:
				return &mem->p1;
			
			/**
			 * Serial Data Registers
			 */
			case 0xFF01:
				return &mem->sb;
			case 0xFF02:
				return &mem->sc;

			/**
			 * Interrupts Flag
			 */
			case 0xFF0F:
				return &mem->intflag;

			case 0xFF24:
				return &mem->nr50;
			case 0xFF25:
				return &mem->nr51;
			case 0xFF26:
				return &mem->nr52;

			case 0xFF40:
				return &mem->lcdc;
			case 0xFF41:
				return &mem->stat;
			case 0xFF42:
				return &mem->scy;
			case 0xFF43:
				return &mem->scx;
			case 0xFF44:
				return &mem->ly;
			case 0xFF45:
				return &mem->lyc;
			case 0xFF47:
				return &mem->bgp;
			case 0xFF48:
				return &mem->obp0;
			case 0xFF49:
				return &mem->obp1;
			default:
				printf("Bad Interrupt Register: %x\n", off);
				return 0;
		}

	} else if (off <= 0xFFFF) {
		printf("TopRam Access %x\n", off);
		return &mem->topram[off - END_UNUSED_IO];
	}

	printf("unknown ram lookup %x", off);
	return 0;
}

uint8_t mem_get(memory* mem, uint16_t off) {
	return *ptr(mem, off);
}

uint16_t mem_get16(memory* mem, uint16_t off) {
	uint16_builder a;
	a.low = mem_get(mem, off);
	a.high = mem_get(mem, off + 1);
	return a.full;
}

void mem_set(memory* mem, uint16_t off, uint8_t v) {
	printf("Set %x to %i", off, v);
	*ptr(mem, off) = v;
}

void mem_set16(memory* mem, uint16_t off, uint16_t v) {
	uint16_builder a;
	a.full = v;
	mem_set(mem, off, a.low);
	mem_set(mem, off + 1, a.high);
}