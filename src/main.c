#include "cart.h"
#include "cpu.h"
#include "bus.h"

void emu_run() {
    while (1) {
        cpu_step();
    }
}

int main() {
    cart_init("pokemon.gbc");
    mem_init();
    cpu_init();

    emu_run();
    return 0;
}
