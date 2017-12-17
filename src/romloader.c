#include "romloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint16_t ROM_SIZE = 0x8000;
const uint16_t BIOS_SIZE = 0x100;

const uint16_t TITLE_START = 0x134;
const uint16_t TITLE_END = 0x142;
const uint16_t TITLE_SIZE = TITLE_END - TITLE_START;

uint8_t* rom_load(char const* rom_path, const uint16_t size) {
	
	uint8_t* rom_mem = malloc(size);
	
	FILE* dat = fopen(rom_path, "rb");
	
	if (!fread(rom_mem, size, 1, dat)) {
		printf("Read error (ROM) %s\n", rom_path);
	}

	fclose(dat);

	return rom_mem;
}

void extract_title(char* title, const uint8_t* rom) {
	memcpy(title, rom + TITLE_START, TITLE_SIZE);
	title[TITLE_SIZE] = 0x0;
}