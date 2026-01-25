#include "cpu.h"
#include "bus.h"
#include "instructions.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG

static emu_cpu cpu;

void cpu_init() {
    cpu.cycles = 0;
    cpu.halted = false;

    set_AF(0x01B0);
    set_BC(0x0013);
    set_DE(0x00D8);
    set_HL(0x014D);
    cpu.reg.sp = 0xFFFE;
    cpu.reg.pc = 0x0100;
}

void cpu_step() {
    if (!cpu.halted) {
        uint8_t opcode = bus_read(cpu.reg.pc);
        cpu.reg.pc++;
        const instruction_func_t instruction = instruction_set[opcode >> 4][opcode & 0x0F];;
#ifdef DEBUG
        printf("OPERATION CODE:0x%02x\n", opcode);
        print_reg();
#endif        
        instruction(&cpu);
    } else {
        cpu.cycles += 1;
    }
}

void enable_interrupt() {

}

uint16_t get_AF() {
    return ((uint16_t)cpu.reg.a << 8) | (uint16_t)cpu.reg.f;
}

uint16_t get_BC() {
    return ((uint16_t)cpu.reg.b << 8) | (uint16_t)cpu.reg.c;
}

uint16_t get_DE() {
    return ((uint16_t)cpu.reg.d << 8) | (uint16_t)cpu.reg.e;
}

uint16_t get_HL() {
    return ((uint16_t)cpu.reg.h << 8) | (uint16_t)cpu.reg.l;
}

void set_AF(uint16_t v) {
    cpu.reg.a = v >> 8;
    cpu.reg.f = v & 0xF0;
}

void set_BC(uint16_t v) {
    cpu.reg.b = v >> 8;
    cpu.reg.c = v & 0xFF;
}

void set_DE(uint16_t v) {
    cpu.reg.d = v >> 8;
    cpu.reg.e = v & 0xFF;
}

void set_HL(uint16_t v) {
    cpu.reg.h = v >> 8;
    cpu.reg.l = v & 0xFF;
}

uint8_t flag_Z() {
    return !!(cpu.reg.f & 0x80);
}

uint8_t flag_N() {
    return !!(cpu.reg.f & 0x40);
}

uint8_t flag_H() {
    return !!(cpu.reg.f & 0x20);
}

uint8_t flag_C() {
    return !!(cpu.reg.f & 0x10);
}

void set_Z(uint8_t v) {
    cpu.reg.f = ((cpu.reg.f & 0x7F) | v << 7);
}

void set_N(uint8_t v) {
    cpu.reg.f = ((cpu.reg.f & 0xBF) | v << 6);
}

void set_H(uint8_t v) {
    cpu.reg.f = ((cpu.reg.f & 0xDF) | v << 8);
}

void set_C(uint8_t v) {
    cpu.reg.f = ((cpu.reg.f & 0xEF) | v << 4);
}

void print_reg() {
    printf("Register State\n");
    printf("AF : 0x%04X       A : 0x%02X        F : 0x%02X\n", get_AF(), cpu.reg.a, cpu.reg.f);
    printf("BC : 0x%04X       B : 0x%02X        C : 0x%02X\n", get_BC(), cpu.reg.b, cpu.reg.c);
    printf("DE : 0x%04X       D : 0x%02X        E : 0x%02X\n", get_DE(), cpu.reg.d, cpu.reg.e);
    printf("HL : 0x%04X       H : 0x%02X        L : 0x%02X\n", get_HL(), cpu.reg.h, cpu.reg.l);
    printf("SP : 0x%04X       PC : 0x%04X\n", cpu.reg.sp, cpu.reg.pc);
    printf("Flags\n");
    printf("Z : %d    N : %d    H : %d    C : %d\n", flag_Z(), flag_N(), flag_H(), flag_C());
}