#pragma once

#include <stdint.h>

typedef union cpu_status {
    uint32_t i;
    struct cpu_status_bits {
        uint8_t N : 1;
        uint8_t Z : 1;
        uint8_t C : 1;
        uint8_t V : 1;
        uint8_t Q : 1;
        uint8_t IT1_0 : 2;
        uint8_t _reserved : 5;
        uint8_t GE : 4;
        uint8_t IT7_2 : 6;
        uint8_t E : 1;
        uint8_t A : 1;
        uint8_t I : 1;
        uint8_t F : 1;
        uint8_t T : 1;
        uint8_t M : 5;
    } b;
} cpu_status_t;

typedef struct cpu_registers {
    uint32_t r[13];
    uint32_t r13_sp;
    uint32_t r14_lr;
    uint32_t r15_pc;
    cpu_status_t cpsr;
    cpu_status_t spsr;
} cpu_registers_t;

extern cpu_registers_t cpu_reg;

void cpu_init();
void cpu_exec_ins();
