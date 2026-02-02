#include "interrupt.h"

static bool interrupt_master_enabled = false;
static uint8_t interrupt_master_enabling_countdown = 0;

void enable_interrupt_master() {
    interrupt_master_enabling_countdown = 2;
}

void disable_interrupt_master() {
    interrupt_master_enabled = false;
    interrupt_master_enabling_countdown = 0;
}
