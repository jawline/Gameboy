#ifndef _ROM_LOADER_DEF_H_
#define _ROM_LOADER_DEF_H_
#include <stdint.h>

uint8_t* rom_load(char const* bios_path, char const* rom_path);
void extract_title(char* title, char const* rom);

#endif