#include "instructions.h"
#include "bus.h"
#include "cpu.h"

static inline void x00_nop(emu_cpu *cpu) {
    cpu->cycles += 1;
}

static inline void x0a_ld_a_mbc(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_BC());
    cpu->cycles += 2;
}

static inline void x1a_ld_a_mde(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_DE());
    cpu->cycles += 2;
}

static inline void x40_ld_b_b(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x41_ld_b_c(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x42_ld_b_d(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x43_ld_b_e(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x44_ld_b_h(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x45_ld_b_l(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x46_ld_b_mhl(emu_cpu *cpu) {
    cpu->reg.b = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x47_ld_b_a(emu_cpu *cpu) {
    cpu->reg.b = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x48_ld_c_b(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x49_ld_c_c(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x4a_ld_c_d(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x4b_ld_c_e(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x4c_ld_c_h(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x4d_ld_c_l(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x4e_ld_c_mhl(emu_cpu *cpu) {
    cpu->reg.c = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x4f_ld_c_a(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x50_ld_d_b(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x51_ld_d_c(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x52_ld_d_d(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x53_ld_d_e(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x54_ld_d_h(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x55_ld_d_l(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x56_ld_d_mhl(emu_cpu *cpu) {
    cpu->reg.d = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x57_ld_d_a(emu_cpu *cpu) {
    cpu->reg.d = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x58_ld_e_b(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x59_ld_e_c(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x5a_ld_e_d(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x5b_ld_e_e(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x5c_ld_e_h(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x5d_ld_e_l(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x5e_ld_e_mhl(emu_cpu *cpu) {
    cpu->reg.e = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x5f_ld_e_a(emu_cpu *cpu) {
    cpu->reg.e = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x60_ld_h_b(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x61_ld_h_c(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x62_ld_h_d(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x63_ld_h_e(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x64_ld_h_h(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x65_ld_h_l(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x66_ld_h_mhl(emu_cpu *cpu) {
    cpu->reg.h = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x67_ld_h_a(emu_cpu *cpu) {
    cpu->reg.h = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x68_ld_l_b(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x69_ld_l_c(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x6a_ld_l_d(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x6b_ld_l_e(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x6c_ld_l_h(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x6d_ld_l_l(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x6e_ld_l_mhl(emu_cpu *cpu) {
    cpu->reg.l = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x6f_ld_l_a(emu_cpu *cpu) {
    cpu->reg.l = cpu->reg.a;
    cpu->cycles += 1;
}

static inline void x78_ld_a_b(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.b;
    cpu->cycles += 1;
}

static inline void x79_ld_a_c(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.c;
    cpu->cycles += 1;
}

static inline void x7a_ld_a_d(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.d;
    cpu->cycles += 1;
}

static inline void x7b_ld_a_e(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.e;
    cpu->cycles += 1;
}

static inline void x7c_ld_a_h(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x7d_ld_a_l(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.l;
    cpu->cycles += 1;
}

static inline void x7e_ld_a_mhl(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_HL());
    cpu->cycles += 2;
}

static inline void x7f_ld_a_a(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.a;
    cpu->cycles += 1;
}

// clang-format off
static const instruction_func_t instruction_set[16][16] = {
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x0a_ld_a_mbc, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x1a_ld_a_mde, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x40_ld_b_b, x41_ld_b_c, x42_ld_b_d, x43_ld_b_e, x44_ld_b_h, x45_ld_b_l, x46_ld_b_mhl, x47_ld_b_a, x48_ld_c_b, x49_ld_c_c, x4a_ld_c_d, x4b_ld_c_e, x4c_ld_c_h, x4d_ld_c_l, x4e_ld_c_mhl, x4f_ld_c_a,
    x50_ld_d_b, x51_ld_d_c, x52_ld_d_d, x53_ld_d_e, x54_ld_d_h, x55_ld_d_l, x56_ld_d_mhl, x57_ld_d_a, x58_ld_e_b, x59_ld_e_c, x5a_ld_e_d, x5b_ld_e_e, x5c_ld_e_h, x5d_ld_e_l, x5e_ld_e_mhl, x5f_ld_e_a,
    x60_ld_h_b, x61_ld_h_c, x62_ld_h_d, x63_ld_h_e, x64_ld_h_h, x65_ld_h_l, x66_ld_h_mhl, x67_ld_h_a, x68_ld_l_b, x69_ld_l_c, x6a_ld_l_d, x6b_ld_l_e, x6c_ld_l_h, x6d_ld_l_l, x6e_ld_l_mhl, x6f_ld_l_a,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x78_ld_a_b, x79_ld_a_c, x7a_ld_a_d, x7b_ld_a_e, x7c_ld_a_h, x7d_ld_a_l, x7e_ld_a_mhl, x7f_ld_a_a,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
    x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop, x00_nop,
};
// clang-format on
