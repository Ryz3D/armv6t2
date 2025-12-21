#pragma once

#include <stdint.h>

typedef struct cpu_registers {
    uint32_t pc;
    uint32_t cpsr;
    uint32_t spsr;
} cpu_registers_t;

extern cpu_registers_t cpu_reg;

void cpu_init();
void cpu_exec_ins();
