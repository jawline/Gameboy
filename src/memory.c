#include "memory.h"
#include "types.h"
#include "romloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_init(memory* mem, uint8_t* rom, uint8_t* bootrom) {
	memset(mem, 0, sizeof(memory));

	mem->bootrom = bootrom;
	mem->bootrom_enabled = 1;

	mem->rom = rom;

	mem->ram = malloc(8192);
	mem->vram = malloc(8192);
	mem->topram = malloc(0xFFFF - 0xFF80);
	mem->ioram = malloc(0xFFFF);
	mem->sprite_attrib = malloc(0xFEA1 - 0xFE00);
}

uint8_t* ptr(memory* mem, uint16_t off) {
	const uint16_t ROM_END = 0x8000;
	const uint16_t VRAM_END = 0xA000;
	const uint16_t SWITCH_RAM_END = 0xC000;
	const uint16_t MAIN_RAM_END = 0xE000;
	const uint16_t MAIN_RAM_ECHO = 0xFE00;
	const uint16_t SPRITE_ATTRIB = 0xFEA0;
	const uint16_t END_UNUSED_IO = 0xFF80;

	if (off < BIOS_SIZE && mem->bootrom_enabled) {
		return &mem->bootrom[off];
	} if (off < ROM_END) {
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
	} else if (off < SPRITE_ATTRIB) {
		return &mem->sprite_attrib[off - MAIN_RAM_ECHO];
	} else if (off < END_UNUSED_IO) {
		printf("Looking up IORAM %x\n", off - SPRITE_ATTRIB);
		return &mem->ioram[off - SPRITE_ATTRIB];
	} else if (off <= 0xFFFE) {
		return &mem->topram[off - END_UNUSED_IO];
	} else {
		return &mem->interrupts_enabled;
	}

	printf("unknown ram lookup %x", off);
	return 0;
}

uint8_t mem_get(memory* mem, uint16_t off) {
	MEMORY_DEBUG("Get 0x%x, 0x%x\n", off, *ptr(mem, off));
	return *ptr(mem, off);
}

uint16_t mem_get16(memory* mem, uint16_t off) {
	return mem_get(mem, off) + (mem_get(mem, off + 1) << 8);
}

void mem_set(memory* mem, uint16_t off, uint8_t v) {
	
	if ((off >= 0x2000) && (off <= 0x3FFF)) {
		printf("WARN: UNHANDLED RAM BANK SELECT %x", v);
		return;
	}

	if (off < 0x8000) {
		printf("Invalid write %x\n", off);
		exit(1);
	}

	MEMORY_DEBUG("Set 0x%x to 0x%i\n", off, v);
	*ptr(mem, off) = v;
}

void mem_set16(memory* mem, uint16_t off, uint16_t v) {
	mem_set(mem, off, v & 0xFF);
	mem_set(mem, off + 1, v >> 8);
}
