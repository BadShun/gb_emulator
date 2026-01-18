#include "cpu.h"
#include <stdio.h>

static emu_cpu cpu;

void cpu_init() {
    set_AF(0x01B0);
    set_BC(0x0013);
    set_DE(0x00D8);
    set_HL(0x014D);
    cpu.reg.sp = 0xFFFE;
    cpu.reg.pc = 0x0100;
}

void print_reg() {
    printf("Register State\n");
    printf("AF : %04X       A : %02X        F : %02X\n", get_AF(), cpu.reg.a, cpu.reg.f);
    printf("BC : %04X       B : %02X        C : %02X\n", get_BC(), cpu.reg.b, cpu.reg.c);
    printf("DE : %04X       D : %02X        E : %02X\n", get_DE(), cpu.reg.d, cpu.reg.e);
    printf("HL : %04X       H : %02X        L : %02X\n", get_HL(), cpu.reg.h, cpu.reg.l);
    printf("SP : %04X       PC : %04X\n", cpu.reg.sp, cpu.reg.pc);
    printf("Flags\n");
    printf("Z : %d    N : %d    H : %d    C : %d\n", flag_Z, flag_N, flag_H, flag_C);
}