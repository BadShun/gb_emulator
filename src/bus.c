#include "bus.h"
#include "cart.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
Start   End     Description                     Note
------------------------------------------------------------------------------------------------------
0000	3FFF	16 KiB ROM bank 00	            From cartridge, usually a fixed bank
4000	7FFF	16 KiB ROM Bank 01–NN	        From cartridge, switchable bank via mapper (if any)
8000	9FFF	8 KiB Video RAM (VRAM)	        In CGB mode, switchable bank 0/1
A000	BFFF	8 KiB External RAM	            From cartridge, switchable bank if any
C000	CFFF	4 KiB Work RAM (WRAM)
D000	DFFF	4 KiB Work RAM (WRAM)	        In CGB mode, switchable bank 1–7
E000	FDFF	Echo RAM (mirror of C000–DDFF)	Nintendo says use of this area is prohibited.
FE00	FE9F	Object attribute memory (OAM)
FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
FF00	FF7F	I/O Registers
FF80	FFFE	High RAM (HRAM)
FFFF	FFFF	Interrupt Enable register (IE)
------------------------------------------------------------------------------------------------------
*/

// 8kB = 8192B

static uint8_t vram[8192];
static uint8_t wram[8192];
static uint8_t oam[160];
static uint8_t hram[128];

void mem_init() {
    memset(vram, 0, sizeof(uint8_t) * 8192);
    memset(wram, 0, sizeof(uint8_t) * 8192);
    memset(oam,  0, sizeof(uint8_t) * 160);
    memset(hram, 0, sizeof(uint8_t) * 128);
}

uint8_t bus_read(uint16_t addr) {
    if (addr <= 0x7FFF) { // cart rom
        return cart_mem_read(addr);
    }

    if (addr <= 0x9FFF) { // vram
        return vram[addr - 0x8000];
    }

    if (addr <= 0xBFFF) { // cart ram
        return cart_mem_read(addr);
    }

    if (addr <= 0xDFFF) { // work ram
        return wram[addr - 0xC000];
    }

    if (addr >= 0xFF80 && addr <= 0xFFFE) { // high ram
        return hram[addr - 0xFF80];
    }

    printf("unsupport bus read address 0x%04X\n", addr);
    return 0x00;
}

void bus_write(uint16_t addr, uint8_t data) {
    if (addr <= 0x7FFF) { // cart rom
        cart_mem_write(addr, data);
    }

    if (addr <= 0x9FFF) { // vram
        vram[addr - 0x8000] = data;
    }

    if (addr <= 0xBFFF) { // cart ram
        cart_mem_write(addr, data);
    }

    if (addr <= 0xDFFF) { // work ram
        wram[addr - 0xC000] = data;
    }

    if (addr >= 0xFF80 && addr <= 0xFFFE) { // high ram
        hram[addr - 0xFF80] = data;
    }

    printf("unsupport bus write address 0x%04X\n", addr);
}