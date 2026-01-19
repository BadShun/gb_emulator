#ifndef __CART_H
#define __CART_H

#include <stdint.h>

typedef struct {
    uint8_t entry_point[4];
    uint8_t logo[0x30];
    char title[16];
    uint8_t new_lic_code[2];
    uint8_t sgb_flag;
    uint8_t cart_type;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t dest_code;
    uint8_t old_lic_code;
    uint8_t mask_rom_version_number;
    uint8_t header_checksum;
    uint8_t global_checksum[2];
} cart_header;

static const char *ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static const char *RAM_SIZE_TYPES[] = {
    "0",
    "-",
    "8 KB (1 bank)",
    "32 KB (4 banks of 8KB each)",
    "128 KB (16 banks of 8KB each)",
    "64 KB (8 banks of 8KB each)"
};

void cart_init(const char *cart_path);
uint8_t cart_mem_read(uint16_t addr);
void cart_mem_write(uint16_t addr, uint8_t data);

#endif