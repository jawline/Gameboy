#ifndef _ROM_LOADER_DEF_H_
#define _ROM_LOADER_DEF_H_
#include <stdint.h>

const uint16_t ROM_SIZE;
const uint16_t BIOS_SIZE;

uint8_t* rom_load(char const* rom_path, const uint16_t size);
void extract_title(char* title, char const* rom);

#endif