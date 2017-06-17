#include "romloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint16_t TITLE_START = 300;
const uint16_t TITLE_END = 334;
const uint16_t TITLE_SIZE = 34;
const uint16_t ROM_SIZE = 0x8000;

uint8_t* rom_load(char const* filename) {
	uint8_t* rom_mem = malloc(ROM_SIZE);
	FILE* dat = fopen(filename, "rb");
	
	if (!fread(rom_mem, ROM_SIZE, 1, dat)) {
		printf("Read error on %s\n", filename);
	}

	return rom_mem;
}

void extract_title(char* title, char const* rom) {
	memcpy(title, rom + TITLE_START, TITLE_SIZE);
}