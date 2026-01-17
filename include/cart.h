#ifndef __CART_H
#define __CART_H

#include "common.h"

void cart_init(const char *cart_path);
static const int cart_read(const char *cart_path);
static const char *get_cart_type(uint_8 type);
static const char *get_cart_ram_size(uint_8 ram_size_code);
static const char *get_cart_lic_code(uint_8 lic_code);

#endif