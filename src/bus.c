#include "bus.h"
#include "cart.h"

#include <stdio.h>
#include <stdint.h>

uint8_t bus_read(uint16_t addr) {
    if (addr <= 0x7FFF) { // cart rom
        return cart_mem_read(addr);
    }

    if (addr <= 0x9FFF) { // vram
        printf("bus read address 0x%04X data not implement now\n", addr);
        return 0xFF;
    }

    if (addr <= 0xBFFF) { // cart ram
        return cart_mem_read(addr);
    }

    if (addr <= 0xDFFF) { // work ram
        printf("bus read address 0x%04X data not implement now\n", addr);
        return 0xFF;
    }

    if (addr >= 0xFF80 && addr <= 0xFFFE) { // high ram
        printf("bus read address 0x%04X data not implement now\n", addr);
        return 0xFF;
    }

    printf("unsupport bus read address 0x%04X\n", addr);
    return 0x00;
}

void bus_write(uint16_t addr, uint8_t data) {
    if (addr <= 0x7FFF) { // cart rom
        cart_mem_write(addr, data);
    }

    if (addr <= 0x9FFF) { // vram
        printf("bus read address 0x%04X data not implement now\n", addr);
    }

    if (addr <= 0xBFFF) { // cart ram
        cart_mem_write(addr, data);
    }

    if (addr <= 0xDFFF) { // work ram
        printf("bus read address 0x%04X data not implement now\n", addr);
    }

    if (addr >= 0xFF80 && addr <= 0xFFFE) { // high ram
        printf("bus read address 0x%04X data not implement now\n", addr);
    }

    printf("unsupport bus write address 0x%04X\n", addr);
}