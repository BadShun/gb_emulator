#include "instructions.h"
#include "bus.h"
#include "cpu.h"

#include <stdint.h>

static inline uint8_t read_d8(emu_cpu *cpu) {
    uint8_t d8 = bus_read(cpu->reg.pc);
    cpu->reg.pc++;
    return d8;
}

static inline uint16_t read_d16(emu_cpu *cpu) {
    uint8_t low = bus_read(cpu->reg.pc);
    cpu->reg.pc++;
    uint8_t high = bus_read(cpu->reg.pc);
    cpu->reg.pc++;
    uint16_t d16 = (uint16_t)low | ((uint16_t)high << 8);
    return d16;
}

static inline void cp_8(uint8_t v1, uint8_t v2) {
    set_N(1);
    set_Z(v1 == v2);
    set_H((v1 & 0x0F) < (v2 & 0x0F));
    set_C(v1 < v2);
}

static inline void push_16(emu_cpu *cpu, uint16_t v) {
    cpu->reg.sp -= 2;
    bus_write(cpu->reg.sp + 1, (uint8_t)(v >> 8) & 0xFF);
    bus_write(cpu->reg.sp, (uint8_t)(v & 0xFF));
}

static inline uint16_t pop_16(emu_cpu *cpu) {
    uint16_t u16 = (uint16_t)bus_read(cpu->reg.sp) | ((uint16_t)bus_read(cpu->reg.sp + 1) << 8);
    cpu->reg.sp += 2;
    return u16;
}

static inline void inc_8(emu_cpu *cpu, uint8_t *v) {
    (*v)++;
    set_Z(*v == 0);
    set_N(0);
    set_H((*v & 0x0F) == 0x00);
}

static inline void dec_8(emu_cpu *cpu, uint8_t *v) {
    (*v)--;
    set_Z(*v == 0);
    set_N(1);
    set_H((*v & 0x0F) == 0x0F);
}

static inline uint8_t add_8(uint16_t v1, uint16_t v2) {
    uint8_t r = (uint8_t)(v1 + v2); 
    set_Z(r == 0);
    set_N(0);
    set_H(((v1 & 0x0F) + (v2 & 0x0F)) > 0x0F);
    set_C((v1 + v2) > 0xFF);
    return r;
}

static inline uint16_t add_16(uint32_t v1, uint32_t v2) {
    uint16_t r = (uint16_t)(v1 + v2);
    set_N(0);
    set_H(((v1 & 0x0FFF) + (v2 & 0x0FFF)) > 0x0FFF);
    set_C((v1 + v2) > 0xFFFF);
    return r;
}

static inline uint8_t adc_8(uint16_t v1, uint16_t v2) {
    uint16_t c = flag_C();
    uint8_t  r = (uint8_t)(v1 + v2 + c);
    set_Z(r == 0);
    set_N(0);
    set_H(((v1 & 0x0F) + (v2 & 0x0F) + c) > 0x0F);
    set_C((v1 + v2 + c) > 0xFF);
    return r;
}

static inline uint8_t sub_8(uint16_t v1, uint16_t v2) {
    uint8_t r = (uint8_t)(v1 - v2);
    set_Z(r == 0);
    set_N(1);
    set_H((v1 & 0x0F) < (v2 & 0x0F));
    set_C(v1 < v2);
    return r;
}

static inline uint8_t sbc_8(uint16_t v1, uint16_t v2) {
    uint16_t c = flag_C();
    uint8_t  r = (uint8_t)(v1 - v2 - c);
    set_Z(r == 0);
    set_N(1);
    set_H((v1 & 0x0F) < ((v2 & 0x0F) + c));
    set_C(v1 < (v2 + c));
    return r;
}

static inline uint8_t and_8(uint8_t v1, uint8_t v2) {
    uint8_t r = v1 & v2;
    set_Z(r == 0);
    set_N(0);
    set_H(1);
    set_C(0);
    return r;
}

static inline uint8_t xor_8(uint8_t v1, uint8_t v2) {
    uint8_t r = v1 ^ v2;
    set_Z(r == 0);
    set_N(0);
    set_H(0);
    set_C(0);
    return r;
}

static inline uint8_t or_8(uint8_t v1, uint8_t v2) {
    uint8_t r = v1 | v2;
    set_Z(r == 0);
    set_N(0);
    set_H(0);
    set_C(0);
    return r;
}

static inline void x00_nop(emu_cpu *cpu) {
    cpu->cycles += 1;
}

static inline void x01_ld_bc_d16(emu_cpu *cpu) {
    set_BC(read_d16(cpu));
    cpu->cycles += 3;
}

static inline void x02_ld_mbc_a(emu_cpu *cpu) {
    bus_write(get_BC(), cpu->reg.a);
    cpu->cycles += 2;
}

static inline void x03_inc_bc(emu_cpu *cpu) {
    set_BC(get_BC() + 1);
    cpu->cycles += 2;
}

static inline void x04_inc_b(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x05_dec_b(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x06_ld_b_d8(emu_cpu *cpu) {
    cpu->reg.b = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x08_ld_a16_sp(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;
    bus_write(a16, (uint8_t)(cpu->reg.sp & 0xFF));
    cpu->cycles += 1;
    bus_write(a16 + 1, (uint8_t)(cpu->reg.sp >> 8));
    cpu->cycles += 2;
}

static inline void x09_add_hl_bc(emu_cpu *cpu) {
    set_HL(add_16(get_HL(), get_BC()));
    cpu->cycles += 2;
}

static inline void x0a_ld_a_mbc(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_BC());
    cpu->cycles += 2;
}

static inline void x0b_dec_bc(emu_cpu *cpu) {
    set_BC(get_BC() - 1);
    cpu->cycles += 2;
}

static inline void x0c_inc_c(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x0d_dec_c(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x0e_ld_c_d8(emu_cpu *cpu) {
    cpu->reg.c = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x11_ld_de_d16(emu_cpu *cpu) {
    set_DE(read_d16(cpu));
    cpu->cycles += 3;
}

static inline void x12_ld_mde_a(emu_cpu *cpu) {
    bus_write(get_DE(), cpu->reg.a);
    cpu->cycles += 2;
}

static inline void x13_inc_de(emu_cpu *cpu) {
    set_DE(get_DE() + 1);
    cpu->cycles += 2;
}

static inline void x14_inc_d(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x15_dec_d(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x16_ld_d_d8(emu_cpu *cpu) {
    cpu->reg.d = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x18_jr_r8(emu_cpu *cpu) {
    int8_t r8 = (int8_t)read_d8(cpu);
    cpu->reg.pc += (int16_t)r8;
    cpu->cycles += 3;
}

static inline void x19_add_hl_de(emu_cpu *cpu) {
    set_HL(add_16(get_HL(), get_DE()));
    cpu->cycles += 2;
}

static inline void x1a_ld_a_mde(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_DE());
    cpu->cycles += 2;
}

static inline void x1b_dec_de(emu_cpu *cpu) {
    set_DE(get_DE() - 1);
    cpu->cycles += 2;
}

static inline void x1c_inc_e(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x1d_dec_e(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x1e_ld_e_d8(emu_cpu *cpu) {
    cpu->reg.e = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x20_jr_nz_r8(emu_cpu *cpu) {
    int8_t r8 = (int8_t)read_d8(cpu);

    if (!flag_Z()) {
        cpu->reg.pc += (int16_t)r8;
        cpu->cycles += 3;
    } else {
        cpu->cycles += 2;
    }
}

static inline void x21_ld_hl_d16(emu_cpu *cpu) {
    set_HL(read_d16(cpu));
    cpu->cycles += 3;
}

static inline void x22_ldi_mhl_a(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.a);
    set_HL(get_HL() + 1);
    cpu->cycles += 2;
}

static inline void x23_inc_hl(emu_cpu *cpu) {
    set_HL(get_HL() + 1);
    cpu->cycles += 2;
}

static inline void x24_inc_h(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x25_dec_h(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x26_ld_h_d8(emu_cpu *cpu) {
    cpu->reg.h = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x27_daa(emu_cpu *cpu) {
    uint8_t a = cpu->reg.a;
    uint8_t adjust = 0;
    uint8_t c = flag_C();

    if (!flag_N()) {
        if (flag_H() || (a & 0x0F) > 9) {
            adjust |= 0x06;
        }

        if (flag_C() || a > 0x99) {
            adjust |= 0x60;
            c = 1;
        }

        a += adjust;
    } else {
        if (flag_H()) {
            adjust |= 0x06;
        }

        if (flag_C()) {
            adjust |= 0x60;
        }

        a -= adjust;
    }

    cpu->reg.a = a;

    set_Z(a == 0);
    set_H(0);
    set_C(c);

    cpu->cycles += 1;
}

static inline void x28_jr_z_r8(emu_cpu *cpu) {
    int8_t r8 = (int8_t)read_d8(cpu);

    if (flag_Z()) {
        cpu->reg.pc += (int16_t)r8;
        cpu->cycles += 3;
    } else {
        cpu->cycles += 2;
    }
}

static inline void x29_add_hl_hl(emu_cpu *cpu) {
    set_HL(add_16(get_HL(), get_HL()));
    cpu->cycles += 2;
}

static inline void x2a_ldi_a_mhl(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_HL());
    set_HL(get_HL() + 1);
    cpu->cycles +=2;
}

static inline void x2b_dec_hl(emu_cpu *cpu) {
    set_HL(get_HL() - 1);
    cpu->cycles += 2;
}

static inline void x2c_inc_l(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x2d_dec_l(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x2e_ld_l_d8(emu_cpu *cpu) {
    cpu->reg.l = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x2f_cpl(emu_cpu *cpu) {
    cpu->reg.a = cpu->reg.a ^ 0xFF;
    set_N(1);
    set_H(1);
    cpu->cycles += 1;
}

static inline void x30_jr_nc_r8(emu_cpu *cpu) {
    int8_t r8 = (int8_t)read_d8(cpu);

    if (!flag_C()) {
        cpu->reg.pc += (int16_t)r8;
        cpu->cycles += 3;
    } else {
        cpu->cycles += 2;
    }
}

static inline void x31_ld_sp_d16(emu_cpu *cpu) {
    cpu->reg.sp = read_d16(cpu);
    cpu->cycles += 3;
}

static inline void x32_ldd_mhl_a(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.a);
    set_HL(get_HL() - 1);
    cpu->cycles += 2;
}

static inline void x33_inc_sp(emu_cpu *cpu) {
    cpu->reg.sp++;
    cpu->cycles += 2;
}

static inline void x34_inc_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    inc_8(cpu, &data);
    bus_write(get_HL(), data);
    cpu->cycles += 2;
}

static inline void x35_dec_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    dec_8(cpu, &data);
    bus_write(get_HL(), data);
    cpu->cycles += 2;
}

static inline void x36_ld_mhl_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    bus_write(get_HL(), d8);
    cpu->cycles += 2;
}

static inline void x37_scf(emu_cpu *cpu) {
    set_N(0);
    set_H(0);
    set_C(1);
    cpu->cycles += 1;
}

static inline void x38_jr_c_r8(emu_cpu *cpu) {
    int8_t r8 = (int8_t)read_d8(cpu);

    if (flag_C()) {
        cpu->reg.pc += (int16_t)r8;
        cpu->cycles += 3;
    } else {
        cpu->cycles += 2;
    }
}

static inline void x39_add_hl_sp(emu_cpu *cpu) {
    set_HL(add_16(get_HL(), cpu->reg.sp));
    cpu->cycles += 2;
}

static inline void x3a_ldd_a_mhl(emu_cpu *cpu) {
    cpu->reg.a = bus_read(get_HL());
    set_HL(get_HL() - 1);
    cpu->cycles += 2;
}

static inline void x3b_dec_sp(emu_cpu *cpu) {
    cpu->reg.sp--;
    cpu->cycles += 2;
}

static inline void x3c_inc_a(emu_cpu *cpu) {
    inc_8(cpu, &cpu->reg.a);
    cpu->cycles += 1;
}

static inline void x3d_dec_a(emu_cpu *cpu) {
    dec_8(cpu, &cpu->reg.a);
    cpu->cycles += 1;
}

static inline void x3e_ld_a_d8(emu_cpu *cpu) {
    cpu->reg.a = read_d8(cpu);
    cpu->cycles += 2;
}

static inline void x3f_ccf(emu_cpu *cpu) {
    set_N(0);
    set_H(0);
    set_C(!flag_C());
    cpu->cycles += 1;
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
    cpu->reg.c = cpu->reg.h;
    cpu->cycles += 1;
}

static inline void x4d_ld_c_l(emu_cpu *cpu) {
    cpu->reg.c = cpu->reg.l;
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

static inline void x70_ld_mhl_b(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.b);
    cpu->cycles += 2;
}

static inline void x71_ld_mhl_c(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.c);
    cpu->cycles += 2;
}

static inline void x72_ld_mhl_d(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.d);
    cpu->cycles += 2;
}

static inline void x73_ld_mhl_e(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.e);
    cpu->cycles += 2;
}

static inline void x74_ld_mhl_h(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.h);
    cpu->cycles += 2;
}

static inline void x75_ld_mhl_l(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.l);
    cpu->cycles += 2;
}

static inline void x77_ld_mhl_a(emu_cpu *cpu) {
    bus_write(get_HL(), cpu->reg.a);
    cpu->cycles += 2;
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

static inline void x80_add_a_b(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x81_add_a_c(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x82_add_a_d(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x83_add_a_e(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x84_add_a_h(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x85_add_a_l(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x86_add_a_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = add_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void x87_add_a_a(emu_cpu *cpu) {
    cpu->reg.a = add_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void x88_adc_a_b(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x89_adc_a_c(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x8a_adc_a_d(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x8b_adc_a_e(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x8c_adc_a_h(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x8d_adc_a_l(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x8e_adc_a_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = adc_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void x8f_adc_a_a(emu_cpu *cpu) {
    cpu->reg.a = adc_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void x90_sub_b(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x91_sub_c(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x92_sub_d(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x93_sub_e(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x94_sub_h(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x95_sub_l(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x96_sub_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = sub_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void x97_sub_a(emu_cpu *cpu) {
    cpu->reg.a = sub_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void x98_sbc_a_b(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void x99_sbc_a_c(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void x9a_sbc_a_d(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void x9b_sbc_a_e(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void x9c_sbc_a_h(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void x9d_sbc_a_l(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void x9e_sbc_a_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = sbc_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void x9f_sbc_a_a(emu_cpu *cpu) {
    cpu->reg.a = sbc_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void xa0_and_b(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void xa1_and_c(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void xa2_and_d(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void xa3_and_e(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void xa4_and_h(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void xa5_and_l(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void xa6_and_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = and_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void xa7_and_a(emu_cpu *cpu) {
    cpu->reg.a = and_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void xa8_xor_b(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void xa9_xor_c(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void xaa_xor_d(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void xab_xor_e(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void xac_xor_h(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void xad_xor_l(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void xae_xor_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = xor_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void xaf_xor_a(emu_cpu *cpu) {
    cpu->reg.a = xor_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void xb0_or_b(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void xb1_or_c(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void xb2_or_d(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void xb3_or_e(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void xb4_or_h(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void xb5_or_l(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void xb6_or_mhl(emu_cpu *cpu) {
    uint8_t data = bus_read(get_HL());
    cpu->cycles += 1;
    cpu->reg.a = or_8(cpu->reg.a, data);
    cpu->cycles += 1;
}

static inline void xb7_or_a(emu_cpu *cpu) {
    cpu->reg.a = or_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void xb8_cp_b(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.b);
    cpu->cycles += 1;
}

static inline void xb9_cp_c(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.c);
    cpu->cycles += 1;
}

static inline void xba_cp_d(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.d);
    cpu->cycles += 1;
}

static inline void xbb_cp_e(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.e);
    cpu->cycles += 1;
}

static inline void xbc_cp_h(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.h);
    cpu->cycles += 1;
}

static inline void xbd_cp_l(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.l);
    cpu->cycles += 1;
}

static inline void xbe_cp_mhl(emu_cpu *cpu) {
    cp_8(cpu->reg.a, bus_read(get_HL()));
    cpu->cycles += 2;
}

static inline void xbf_cp_a(emu_cpu *cpu) {
    cp_8(cpu->reg.a, cpu->reg.a);
    cpu->cycles += 1;
}

static inline void xc0_ret_nz(emu_cpu *cpu) {
    if (!flag_Z()) {
        cpu->reg.pc = pop_16(cpu);
        cpu->cycles += 5;
    } else {
        cpu->cycles += 2;
    }
}

static inline void xc1_pop_bc(emu_cpu *cpu) {
    set_BC(pop_16(cpu));
    cpu->cycles += 3;
}

static inline void xc2_jp_nz_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);

    if (!flag_Z()) {
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 3;
    }
}

static inline void xc3_jp_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->reg.pc = a16;
    cpu->cycles += 4;
}

static inline void xc4_call_nz_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;

    if (!flag_Z()) {
        push_16(cpu, cpu->reg.pc);
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 1;
    }
}

static inline void xc5_push_bc(emu_cpu *cpu) {
    push_16(cpu, get_BC());
    cpu->cycles += 4;
}

static inline void xc6_add_a_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = add_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xc7_rst_00h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0000;
    cpu->cycles += 4;
}

static inline void xc8_ret_z(emu_cpu *cpu) {
    if (flag_Z()) {
        cpu->reg.pc = pop_16(cpu);
        cpu->cycles += 5;
    } else {
        cpu->cycles += 2;
    }
}

static inline void xc9_ret(emu_cpu *cpu) {
    cpu->reg.pc = pop_16(cpu);
    cpu->cycles += 4;
}

static inline void xca_jp_z_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);

    if (flag_Z()) {
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 3;
    }
}

static inline void xcc_call_z_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;

    if (flag_Z()) {
        push_16(cpu, cpu->reg.pc);
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 1;
    }
}

static inline void xcd_call_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = a16;
    cpu->cycles += 4;
}

static inline void xce_adc_a_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = adc_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xcf_rst_08h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0008;
    cpu->cycles += 4;
}

static inline void xd0_ret_nc(emu_cpu *cpu) {
    if (!flag_C()) {
        cpu->reg.pc = pop_16(cpu);
        cpu->cycles += 5;
    } else {
        cpu->cycles += 2;
    }
}

static inline void xd1_pop_de(emu_cpu *cpu) {
    set_DE(pop_16(cpu));
    cpu->cycles += 3;
}

static inline void xd2_jp_nc_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);

    if (!flag_C()) {
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 3;
    }
}

static inline void xd4_call_nc_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;

    if (!flag_C()) {
        push_16(cpu, cpu->reg.pc);
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 1;
    }
}

static inline void xd5_push_de(emu_cpu *cpu) {
    push_16(cpu, get_DE());
    cpu->cycles += 4;
}

static inline void xd6_sub_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = sub_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xd7_rst_10h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0010;
    cpu->cycles += 4;
}

static inline void xd8_ret_c(emu_cpu *cpu) {
    if (flag_C()) {
        cpu->reg.pc = pop_16(cpu);
        cpu->cycles += 5;
    } else {
        cpu->cycles += 2;
    }
}

static inline void xd9_reti(emu_cpu *cpu) {
    enable_interrupt();
    xc9_ret(cpu);
}

static inline void xda_jp_c_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);

    if (flag_C()) {
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 3;
    }
}

static inline void xdc_call_c_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;

    if (flag_C()) {
        push_16(cpu, cpu->reg.pc);
        cpu->reg.pc = a16;
        cpu->cycles += 4;
    } else {
        cpu->cycles += 1;
    }
}

static inline void xde_sbc_a_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = sbc_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xdf_rst_18h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0018;
    cpu->cycles += 4;
}

static inline void xe0_ldh_m8_a(emu_cpu *cpu) {
    uint8_t a8 = read_d8(cpu);
    cpu->cycles += 1;
    bus_write(0xFF00 + (uint16_t)a8, cpu->reg.a);
    cpu->cycles += 2;
}

static inline void xe1_pop_hl(emu_cpu *cpu) {
    set_HL(pop_16(cpu));
    cpu->cycles += 3;
}

static inline void xe5_push_hl(emu_cpu *cpu) {
    push_16(cpu, get_HL());
    cpu->cycles += 4;
}

static inline void xe6_and_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = and_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xe7_rst_20h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0020;
    cpu->cycles += 4;
}

static inline void xe8_add_sp_r8(emu_cpu *cpu) {
    int8_t   r8 = (int8_t)read_d8(cpu);
    uint16_t sp = cpu->reg.sp;
    uint16_t u8 = (uint16_t)(uint8_t)r8;
    cpu->cycles += 1;
    set_Z(0);
    set_N(0);
    set_H(((sp & 0x0F) + (u8 & 0x0F)) > 0x0F);
    set_C(((sp & 0xFF) + (u8 & 0xFF)) > 0xFF);
    cpu->reg.sp = sp + r8;
    cpu->cycles += 3;
}

static inline void xe9_jp_hl(emu_cpu *cpu) {
    cpu->reg.pc = get_HL();
    cpu->cycles += 1;
}

static inline void xea_ld_a16_a(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;
    bus_write(a16, cpu->reg.a);
    cpu->cycles += 2;
}

static inline void xe2_ld_mc_a(emu_cpu *cpu) {
    bus_write(0xFF00 + (uint16_t)cpu->reg.c, cpu->reg.a);
    cpu->cycles += 2;
}

static inline void xee_xor_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = xor_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xef_rst_28h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0028;
    cpu->cycles += 4;
}

static inline void xf0_ldh_a_m8(emu_cpu *cpu) {
    uint8_t a8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = bus_read(0xFF00 + (uint16_t)a8);
    cpu->cycles += 2;
}

static inline void xf1_pop_af(emu_cpu *cpu) {
    set_AF(pop_16(cpu));
    cpu->cycles += 3;
}

static inline void xf2_ld_a_mc(emu_cpu *cpu) {
    cpu->reg.a = bus_read(0xFF00 + (uint16_t)cpu->reg.c);
    cpu->cycles += 2;
}

static inline void xf5_push_af(emu_cpu *cpu) {
    push_16(cpu, get_AF());
    cpu->cycles += 4;
}

static inline void xf6_or_d8(emu_cpu *cpu) {
    uint8_t d8 = read_d8(cpu);
    cpu->cycles += 1;
    cpu->reg.a = or_8(cpu->reg.a, d8);
    cpu->cycles += 1;
}

static inline void xf7_rst_30h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0030;
    cpu->cycles += 4;
}

static inline void xf8_ld_hl_sp_r8(emu_cpu *cpu) {
    uint16_t sp = cpu->reg.sp;
    int8_t   r8 = (int8_t)read_d8(cpu);
    uint16_t u8 = (uint16_t)(uint8_t)r8;
    cpu->cycles += 1;
    set_Z(0);
    set_N(0);
    set_H(((sp & 0x0F) + (u8 & 0x0F)) > 0x0F);
    set_C(((sp & 0xFF) + u8) > 0xFF);
    set_HL(sp + r8);
    cpu->cycles += 2;
}

static inline void xf9_ld_sp_hl(emu_cpu *cpu) {
    cpu->reg.sp = get_HL();
    cpu->cycles += 2;
}

static inline void xfa_ld_a_a16(emu_cpu *cpu) {
    uint16_t a16 = read_d16(cpu);
    cpu->cycles += 2;
    cpu->reg.a = bus_read(a16);
    cpu->cycles += 2;
}

static inline void xfe_cp_d8(emu_cpu *cpu) {
    cp_8(cpu->reg.a, read_d8(cpu));
    cpu->cycles += 2;
}

static inline void xff_rst_38h(emu_cpu *cpu) {
    push_16(cpu, cpu->reg.pc);
    cpu->reg.pc = 0x0038;
    cpu->cycles += 4;
}

// clang-format off
const instruction_func_t instruction_set[16][16] = {
    x00_nop, x01_ld_bc_d16, x02_ld_mbc_a, x03_inc_bc, x04_inc_b, x05_dec_b, x06_ld_b_d8, x00_nop, x08_ld_a16_sp, x09_add_hl_bc, x0a_ld_a_mbc, x0b_dec_bc, x0c_inc_c, x0d_dec_c, x0e_ld_c_d8, x00_nop,
    x00_nop, x11_ld_de_d16, x12_ld_mde_a, x13_inc_de, x14_inc_d, x15_dec_d, x16_ld_d_d8, x00_nop, x18_jr_r8, x19_add_hl_de, x1a_ld_a_mde, x1b_dec_de, x1c_inc_e, x1d_dec_e, x1e_ld_e_d8, x00_nop,
    x20_jr_nz_r8, x21_ld_hl_d16, x22_ldi_mhl_a, x23_inc_hl, x24_inc_h, x25_dec_h, x26_ld_h_d8, x27_daa, x28_jr_z_r8, x29_add_hl_hl, x2a_ldi_a_mhl, x2b_dec_hl, x2c_inc_l, x2d_dec_l, x2e_ld_l_d8, x2f_cpl,
    x30_jr_nc_r8, x31_ld_sp_d16, x32_ldd_mhl_a, x33_inc_sp, x34_inc_mhl, x35_dec_mhl, x36_ld_mhl_d8, x37_scf, x38_jr_c_r8, x39_add_hl_sp, x3a_ldd_a_mhl, x3b_dec_sp, x3c_inc_a, x3d_dec_a, x3e_ld_a_d8, x3f_ccf,
    x40_ld_b_b, x41_ld_b_c, x42_ld_b_d, x43_ld_b_e, x44_ld_b_h, x45_ld_b_l, x46_ld_b_mhl, x47_ld_b_a, x48_ld_c_b, x49_ld_c_c, x4a_ld_c_d, x4b_ld_c_e, x4c_ld_c_h, x4d_ld_c_l, x4e_ld_c_mhl, x4f_ld_c_a,
    x50_ld_d_b, x51_ld_d_c, x52_ld_d_d, x53_ld_d_e, x54_ld_d_h, x55_ld_d_l, x56_ld_d_mhl, x57_ld_d_a, x58_ld_e_b, x59_ld_e_c, x5a_ld_e_d, x5b_ld_e_e, x5c_ld_e_h, x5d_ld_e_l, x5e_ld_e_mhl, x5f_ld_e_a,
    x60_ld_h_b, x61_ld_h_c, x62_ld_h_d, x63_ld_h_e, x64_ld_h_h, x65_ld_h_l, x66_ld_h_mhl, x67_ld_h_a, x68_ld_l_b, x69_ld_l_c, x6a_ld_l_d, x6b_ld_l_e, x6c_ld_l_h, x6d_ld_l_l, x6e_ld_l_mhl, x6f_ld_l_a,
    x70_ld_mhl_b, x71_ld_mhl_c, x72_ld_mhl_d, x73_ld_mhl_e, x74_ld_mhl_h, x75_ld_mhl_l, x00_nop, x77_ld_mhl_a, x78_ld_a_b, x79_ld_a_c, x7a_ld_a_d, x7b_ld_a_e, x7c_ld_a_h, x7d_ld_a_l, x7e_ld_a_mhl, x7f_ld_a_a,
    x80_add_a_b, x81_add_a_c, x82_add_a_d, x83_add_a_e, x84_add_a_h, x85_add_a_l, x86_add_a_mhl, x87_add_a_a, x88_adc_a_b, x89_adc_a_c, x8a_adc_a_d, x8b_adc_a_e, x8c_adc_a_h, x8d_adc_a_l, x8e_adc_a_mhl, x8f_adc_a_a,
    x90_sub_b, x91_sub_c, x92_sub_d, x93_sub_e, x94_sub_h, x95_sub_l, x96_sub_mhl, x97_sub_a, x98_sbc_a_b, x99_sbc_a_c, x9a_sbc_a_d, x9b_sbc_a_e, x9c_sbc_a_h, x9d_sbc_a_l, x9e_sbc_a_mhl, x9f_sbc_a_a,
    xa0_and_b, xa1_and_c, xa2_and_d, xa3_and_e, xa4_and_h, xa5_and_l, xa6_and_mhl, xa7_and_a, xa8_xor_b, xa9_xor_c, xaa_xor_d, xab_xor_e, xac_xor_h, xad_xor_l, xae_xor_mhl, xaf_xor_a,
    xb0_or_b, xb1_or_c, xb2_or_d, xb3_or_e, xb4_or_h, xb5_or_l, xb6_or_mhl, xb7_or_a, xb8_cp_b, xb9_cp_c, xba_cp_d, xbb_cp_e, xbc_cp_h, xbd_cp_l, xbe_cp_mhl, xbf_cp_a,
    xc0_ret_nz, xc1_pop_bc, xc2_jp_nz_a16, xc3_jp_a16, xc4_call_nz_a16, xc5_push_bc, xc6_add_a_d8, xc7_rst_00h, xc8_ret_z, xc9_ret, xca_jp_z_a16, x00_nop, xcc_call_z_a16, xcd_call_a16, xce_adc_a_d8, xcf_rst_08h,
    xd0_ret_nc, xd1_pop_de, xd2_jp_nc_a16, x00_nop, xd4_call_nc_a16, xd5_push_de, xd6_sub_d8, xd7_rst_10h, xd8_ret_c, xd9_reti, xda_jp_c_a16, x00_nop, xdc_call_c_a16, x00_nop, xde_sbc_a_d8, xdf_rst_18h,
    xe0_ldh_m8_a, xe1_pop_hl, xe2_ld_mc_a, x00_nop, x00_nop, xe5_push_hl, xe6_and_d8, xe7_rst_20h, xe8_add_sp_r8, xe9_jp_hl, xea_ld_a16_a, x00_nop, x00_nop, x00_nop, xee_xor_d8, xef_rst_28h,
    xf0_ldh_a_m8, xf1_pop_af, xf2_ld_a_mc, x00_nop, x00_nop, xf5_push_af, xf6_or_d8, xf7_rst_30h, xf8_ld_hl_sp_r8, xf9_ld_sp_hl, xfa_ld_a_a16, x00_nop, x00_nop, x00_nop, xfe_cp_d8, xff_rst_38h,
};
// clang-format on
