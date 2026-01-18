#ifndef __CPU_H
#define __CPU_H

#include "common.h"
#include <stdbool.h>

typedef struct {
    uint_8 a, f;
    uint_8 b, c;
    uint_8 d, e;
    uint_8 h, l;
    uint_16 sp, pc;
} cpu_register;

typedef struct {
    cpu_register reg;
    uint_8 opcode;
    uint_32 cycles;
    bool halted;
} emu_cpu;

#define get_AF() ((cpu.reg.a << 8) | cpu.reg.f)
#define get_BC() ((cpu.reg.b << 8) | cpu.reg.c)
#define get_DE() ((cpu.reg.d << 8) | cpu.reg.e)
#define get_HL() ((cpu.reg.h << 8) | cpu.reg.l)

#define set_AF(v) do { cpu.reg.a = (uint_16)(v) >> 8; cpu.reg.f = (uint_16)(v) & 0xF0; } while(0)
#define set_BC(v) do { cpu.reg.b = (uint_16)(v) >> 8; cpu.reg.c = (uint_16)(v) & 0xFF; } while(0)
#define set_DE(v) do { cpu.reg.d = (uint_16)(v) >> 8; cpu.reg.e = (uint_16)(v) & 0xFF; } while(0)
#define set_HL(v) do { cpu.reg.h = (uint_16)(v) >> 8; cpu.reg.l = (uint_16)(v) & 0xFF; } while(0)

#define flag_Z !!(cpu.reg.f & 0x80)
#define flag_N !!(cpu.reg.f & 0x40)
#define flag_H !!(cpu.reg.f & 0x20)
#define flag_C !!(cpu.reg.f & 0x10)

#define set_Z(v) do { cpu.reg.f = ((cpu.reg.f & 0x7F) | (uint_8)(v) << 7); } while(0)
#define set_N(v) do { cpu.reg.f = ((cpu.reg.f & 0xBF) | (uint_8)(v) << 6); } while(0)
#define set_H(v) do { cpu.reg.f = ((cpu.reg.f & 0xDF) | (uint_8)(v) << 8); } while(0)
#define set_C(v) do { cpu.reg.f = ((cpu.reg.f & 0xEF) | (uint_8)(v) << 4); } while(0)

void cpu_init();
void print_reg();

#endif