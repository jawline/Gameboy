#include "romloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint16_t TITLE_START = 300;
const uint16_t TITLE_END = 334;
const uint16_t TITLE_SIZE = 34;
const uint16_t ROM_SIZE = 0x8000;
const uint16_t BIOS_SIZE = 0x100;

uint8_t* rom_load(char const* bios_path, char const* rom_path) {
	uint8_t* rom_mem = malloc(ROM_SIZE);
	
	/*FILE* dat = fopen(rom_path, "rb");
	
	if (!fread(rom_mem, ROM_SIZE, 1, dat)) {
		printf("Read error (ROM) %s\n", rom_path);
	}

	fclose(dat);*/

	FILE* dat = fopen(bios_path, "rb");

	if (!fread(rom_mem, BIOS_SIZE, 1, dat)) {
		printf("Read error (BIOS) %s\n", bios_path);
	}

	fclose(dat);

	return rom_mem;
}

void extract_title(char* title, char const* rom) {
	memcpy(title, rom + TITLE_START, TITLE_SIZE);
}