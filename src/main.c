#include "cart.h"
#include "cpu.h"

int main() {
    cart_init("pokemon.gbc");
    cpu_init();
    print_reg();
    return 0;
}
