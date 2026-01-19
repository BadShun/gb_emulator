#include "cart.h"

#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define POSIX
#define DEBUG

static int cart_read(const char *cart_path);
static const char *get_cart_type(uint8_t type);
static const char *get_cart_ram_size(uint8_t ram_size_code);
static const char *get_cart_lic_code(uint8_t lic_code);

static uint8_t *rom_data = NULL;
static uint32_t rom_size;

void cart_init(const char *cart_path) {
    if (cart_read(cart_path)) {
        printf("rom load fail!\n");
        return ;
    }

    cart_header *header = (cart_header *)(rom_data + 0x0100);
    const char *cart_type     = get_cart_type(header->cart_type);
    const char *cart_ram_size = get_cart_ram_size(header->ram_size);
    const char *cart_lic_code = get_cart_lic_code(header->old_lic_code);

#ifdef DEBUG
    printf("CART NAME:%s\nCART TYPE:%s\nCART RAM SIZE:%s\nCART LIC CODE:%s\n",
           header->title, cart_type, cart_ram_size, cart_lic_code);
#endif
}

static int cart_read(const char *cart_path) {
#ifdef POSIX
    int fd = open(cart_path, O_RDONLY);
    if (fd < 0) {
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return 1;
    }

    rom_size = st.st_size;
    rom_data = (char *)malloc(rom_size * sizeof(char));
    read(fd, rom_data, rom_size);
    close(fd);
#endif
    return 0;
}

static const char *get_cart_type(uint8_t type) {
    if (type <= 0x22) {
        return ROM_TYPES[type];
    }

    return "UNKNOWN";
}

static const char *get_cart_ram_size(uint8_t ram_size_code) {
    if (ram_size_code <= 0x05) {
        return RAM_SIZE_TYPES[ram_size_code];
    }

    return "UNKNOWN";
}

static const char *get_cart_lic_code(uint8_t lic_code) {
    switch (lic_code) {
        case 0x00: return "None";
        case 0x01: return "Nintendo R&D1";
        case 0x08: return "Capcom";
        case 0x13: return "Electronic Arts";
        case 0x18: return "Hudson Soft";
        case 0x19: return "b-ai";
        case 0x20: return "kss";
        case 0x22: return "pow";
        case 0x24: return "PCM Complete";
        case 0x25: return "san-x";
        case 0x28: return "Kemco Japan";
        case 0x29: return "seta";
        case 0x30: return "Viacom";
        case 0x31: return "Nintendo";
        case 0x32: return "Bandai";
        case 0x33: return "Ocean/Acclaim";
        case 0x34: return "Konami";
        case 0x35: return "Hector";
        case 0x37: return "Taito";
        case 0x38: return "Hudson";
        case 0x39: return "Banpresto";
        case 0x41: return "Ubi Soft";
        case 0x42: return "Atlus";
        case 0x44: return "Malibu";
        case 0x46: return "angel";
        case 0x47: return "Bullet-Proof";
        case 0x49: return "irem";
        case 0x50: return "Absolute";
        case 0x51: return "Acclaim";
        case 0x52: return "Activision";
        case 0x53: return "American sammy";
        case 0x54: return "Konami";
        case 0x55: return "Hi tech entertainment";
        case 0x56: return "LJN";
        case 0x57: return "Matchbox";
        case 0x58: return "Mattel";
        case 0x59: return "Milton Bradley";
        case 0x60: return "Titus";
        case 0x61: return "Virgin";
        case 0x64: return "LucasArts";
        case 0x67: return "Ocean";
        case 0x69: return "Electronic Arts";
        case 0x70: return "Infogrames";
        case 0x71: return "Interplay";
        case 0x72: return "Broderbund";
        case 0x73: return "sculptured";
        case 0x75: return "sci";
        case 0x78: return "THQ";
        case 0x79: return "Accolade";
        case 0x80: return "misawa";
        case 0x83: return "lozc";
        case 0x86: return "Tokuma Shoten Intermedia";
        case 0x87: return "Tsukuda Original";
        case 0x91: return "Chunsoft";
        case 0x92: return "Video system";
        case 0x93: return "Ocean/Acclaim";
        case 0x95: return "Varie";
        case 0x96: return "Yonezawa/s’pal";
        case 0x97: return "Kaneko";
        case 0x99: return "Pack in soft";
        case 0xA4: return "Konami (Yu-Gi-Oh!)";
        default  : break;
    }

    return "UNKNOWN";
}

uint8_t cart_mem_read(uint16_t addr) {
    return rom_data[addr];
}

void cart_mem_write(uint16_t addr, uint8_t data) {
    printf("unsupport cart write\n");
}