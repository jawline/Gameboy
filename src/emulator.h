#ifndef _EMULATOR_DEF_H_
#define _EMULATOR_DEF_H_
#include "cpu.h"
#include "gpu.h"

char emu_init(cpu_state* state, gpu_state* gpu, char const* bios, char const* rom);

#endif