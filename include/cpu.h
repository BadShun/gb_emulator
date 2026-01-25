#ifndef __CPU_H
#define __CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t a, f;
    uint8_t b, c;
    uint8_t d, e;
    uint8_t h, l;
    uint16_t sp, pc;
} cpu_register;

typedef struct {
    cpu_register reg;
    uint8_t opcode;
    uint32_t cycles;
    bool halted;
} emu_cpu;

uint16_t get_AF();
uint16_t get_BC();
uint16_t get_DE();
uint16_t get_HL();

void set_AF(uint16_t v);
void set_BC(uint16_t v);
void set_DE(uint16_t v);
void set_HL(uint16_t v);

uint8_t flag_Z();
uint8_t flag_N();
uint8_t flag_H();
uint8_t flag_C();

void set_Z(uint8_t v);
void set_N(uint8_t v);
void set_H(uint8_t v);
void set_C(uint8_t v);

void cpu_init();
void cpu_step();
void enable_interrupt();
void print_reg();

#endif