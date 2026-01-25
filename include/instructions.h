#ifndef __INSTRUCTIONS_H
#define __INSTRUCTIONS_H

#include "cpu.h"

typedef void (*instruction_func_t)(emu_cpu *cpu);

extern const instruction_func_t instruction_set[16][16];

#endif