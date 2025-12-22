#include <stdio.h>
#include <memory.h>

#include "cpu.h"
#include "mem.h"

cpu_registers_t cpu_reg;
fpu_registers_t fpu_reg;

uint8_t cpu_in_it_block = 0;

void cpu_init() {
    memset(&cpu_reg, 0, sizeof(cpu_registers_t));
    cpu_reg.r13_sp = mem_read32(CPU_VECTOR_TABLE + 0x0000);
    cpu_reg.r15_pc = mem_read32(CPU_VECTOR_TABLE + 0x0004);
}

#define HW1_IMM5(hw1)  (((hw1) >> 6) & 0b11111)
#define HW1_IMM8(hw1)  (((hw1) >> 0) & 0b11111111)
#define HW1_IMM11(hw1) (((hw1) >> 0) & 0b11111111111)

#define INS_DEC_REG(data, offset, bits) (cpu_reg.r[((data) >> (offset)) & ((1 << bits) - 1)])

void cpu_invalid_instruction(uint16_t hw1) {
    printf("! INVALID INSTRUCTION: hw1 = 0x%04X\r\n", hw1);
}

void cpu_invalid_instruction_32bit(uint16_t hw1, uint16_t hw2) {
    printf("! INVALID INSTRUCTION: hw1 = 0x%04X  hw2 = 0x%04X\r\n", hw1, hw2);
}

// 1111 0xxx [...] 0xxx
void cpu_exec_ins_32bit_data_processing_imm(uint16_t hw1, uint16_t hw2) {
    // p. 3-13
    switch ((hw1 >> 8) & 0b11) {
        case 0b00:
        case 0b01:
            // data processing, modified 12-bit imm
            break;
        case 0b10:
            if ((hw1 >> 6) & 0b1) {
                // move, plain 16-bit imm
            } else {
                // add, subtract, plain 12-bit imm
            }
            break;
        case 0b11:
            // bit field operation, saturation with shift
            break;
        default:
            cpu_invalid_instruction_32bit(hw1, hw2);
            break;
    }
}

// 111x 101x
void cpu_exec_ins_32bit_data_processing_no_imm(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("data processing (no immediate operand)");
}

// 1111 100x
void cpu_exec_ins_32bit_load_store(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("load/store single data item, memory hints");
}

// 1110 100x x1xx
void cpu_exec_ins_32bit_load_store_double(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("load/store, double and exclusive and table branch");
}

// 1110 100x x0xx
void cpu_exec_ins_32bit_load_store_multi(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("load/store multiple, RFE, SRS");
}

// 1111 0xxx [...] 1xxx
void cpu_exec_ins_32bit_branch(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("branches, misc control");
}

// 111x 1111
void cpu_exec_ins_32bit_coprocessor(uint16_t hw1, uint16_t hw2) {
    // TODO
    printf("0x%04X    ", hw2);
    printf("coprocessor");
    /*
    0xE000EF34	FPCCR	RW	0xC0000000	Context Control Register
    0xE000EF38	FPCAR	RW	-	Context Address Register
    0xE000EF3C	FPDSCR	RW	0x00000000	Default Status Control Register
    0xE000EF40	MVFR0	RO	0x10110221	Media and VFP Feature Register 0
    0xE000EF44	MVFR1	RO	0x12000011[a]	Media and VFP Feature Register 1
    0xE000EF48	MVFR2	RO	0x00000040	Media and VFP Feature Register 2
    */
   // -> fpu_reg
}

void cpu_exec_ins() {
    uint16_t hw1 = mem_read16(cpu_reg.r15_pc);
    uint16_t hw2 = 0;
    printf("(cpu_exec_ins) [0x%04X %04X] 0x%04X\r\n",
        cpu_reg.r15_pc >> 16,
        cpu_reg.r15_pc & 0xFFFF,
        hw1);
    cpu_reg.r15_pc += 2;

    printf("0x%04X    ", hw1);
    // thumb instructions
    // p. 3-3
    switch ((hw1 >> 13) & 0b111) {
        case 0b000:
            // shift by immediate and move (register)
            // p. 3-4
            switch ((hw1 >> 10) & 0b111) {
                case 0b000:
                case 0b001: {
                    // logical shift left
                    // if (INS_IMM5(ins) == 0) this instruction is (mis?)used as MOV
                    // shift by n bits, shift in zeros, may set carry to last out-shifted bit
                    uint32_t operand = INS_DEC_REG(hw1, 3, 3);
                    uint8_t shift = HW1_IMM5(hw1);
                    if (!cpu_in_it_block || shift == 0) {
                        if (shift != 0) {
                            cpu_reg.cpsr.b.C = (operand >> (32 - shift)) & 0b1;
                        }
                        cpu_reg.cpsr.b.N = (operand >> 31) & 0b1;
                        cpu_reg.cpsr.b.Z = operand == 0;
                        cpu_reg.cpsr.b.V = 0;
                        // TODO: flags
                    }
                    INS_DEC_REG(hw1, 0, 3) = operand << shift;
                    break;
                }
                case 0b010:
                case 0b011:
                    // logical shift right
                    printf("logical shift right");
                    break;
                case 0b100:
                case 0b101:
                    // arithmetic shift right
                    // copies of the leftmost bit are shifted in at the left
                    printf("arithmetic shift right");
                    break;
                case 0b110:
                    if ((hw1 >> 9) & 0b1) {
                        // subtract register
                        // carry flag = !borrow
                        printf("subtract register");
                    } else {
                        // add register
                        printf("add register");
                    }
                    break;
                case 0b111:
                    if ((hw1 >> 9) & 0b1) {
                        // subtract immediate
                        printf("subtract immediate");
                    } else {
                        // add immediate
                        printf("add immediate");
                    }
                    break;
                default:
                    cpu_invalid_instruction(hw1);
                    break;
            }
            break;
        case 0b001:
            // add, subtract, compare, move (8-bit imm)
            // p. 3-5
            switch ((hw1 >> 11) & 0b11) {
                case 0b00:
                    // move immediate
                    printf("move immediate");
                    break;
                case 0b01:
                    // compare immediate
                    printf("compare immediate");
                    break;
                case 0b10:
                    // add immediate
                    printf("add immediate");
                    break;
                case 0b11:
                    // subtract immediate
                    printf("subtract immediate");
                    break;
                default:
                    cpu_invalid_instruction(hw1);
                    break;
            }
            break;
        case 0b010:
            switch ((hw1 >> 9) & 0b1111) {
                case 0b0000:
                case 0b0001:
                    // data-processing (register)
                    // p. 3-6
                    switch ((hw1 >> 6) & 0b1111) {
                        case 0b0000:
                            // and
                            printf("register and");
                            break;
                        case 0b0001:
                            // XOR
                            printf("register xor");
                            break;
                        case 0b0010:
                            // LSL
                            printf("register logical shift left");
                            break;
                        case 0b0011:
                            // LSR
                            printf("register logical shift right");
                            break;
                        case 0b0100:
                            // ASR
                            printf("register arithmetic shift right");
                            break;
                        case 0b0101:
                            // ADC
                            printf("register add with carry");
                            break;
                        case 0b0110:
                            // SBC
                            printf("register subtract with carry");
                            break;
                        case 0b0111:
                            // ROR
                            printf("register rotate right");
                            break;
                        case 0b1000:
                            // TST
                            printf("register test");
                            break;
                        case 0b1001:
                            // RSB
                            printf("register reverse subtract");
                            break;
                        case 0b1010:
                            // CMP
                            printf("register compare");
                            break;
                        case 0b1011:
                            // CMN
                            printf("register compare negative");
                            break;
                        case 0b1100:
                            // ORR
                            printf("register or");
                            break;
                        case 0b1101:
                            // MUL
                            printf("register multiply");
                            break;
                        case 0b1110:
                            // BIC
                            printf("register bit clear");
                            break;
                        case 0b1111:
                            // MVN
                            printf("register move negative");
                            break;
                        default:
                            cpu_invalid_instruction(hw1);
                            break;
                    }
                    break;
                case 0b0010:
                    // special data processing
                    // p. 3-6
                    if ((hw1 >> 8) & 0b1) {
                        // compare register incl. high registers
                        printf("compare register incl. high registers");
                    } else {
                        // add register incl. high registers
                        printf("add register incl. high registers");
                    }
                    break;
                case 0b0011:
                    // special data processing
                    // p. 3-6...3-7
                    if ((hw1 >> 8) & 0b1) {
                        // branch/exchange ISA
                        // LSB signals thumb, but is being ignored for actual PC address
                        printf("branch/exchange ISA");
                        if ((hw1 >> 7) & 0b1) {
                            printf(" and link");
                        }
                    } else {
                        // move register incl. high registers
                        printf("move register incl. high registers");
                    }
                    break;
                case 0b0100:
                case 0b0101:
                case 0b0110:
                case 0b0111:
                    // load from literal pool
                    printf("load from literal pool");
                    break;
                case 0b1000:
                case 0b1001:
                case 0b1010:
                    // store (register offset)
                    // p. 3-7
                    switch ((hw1 >> 9) & 0b11) {
                        case 0b00:
                            printf("store word (register offset)");
                            break;
                        case 0b01:
                            printf("store halfword (register offset)");
                            break;
                        case 0b10:
                            printf("store byte (register offset)");
                            break;
                        default:
                            cpu_invalid_instruction(hw1);
                            break;
                    }
                    break;
                case 0b1011:
                case 0b1100:
                case 0b1101:
                case 0b1110:
                case 0b1111:
                    // load (register offset)
                    // p. 3-7
                    switch ((hw1 >> 9) & 0b11) {
                        case 0b00:
                            printf("load word (register offset)");
                            break;
                        case 0b01:
                            printf("load unsigned halfword (register offset)");
                            break;
                        case 0b10:
                            printf("load unsigned byte (register offset)");
                            break;
                        case 0b11:
                            if ((hw1 >> 11) & 0b1) {
                                printf("load signed halfword (register offset)");
                            } else {
                                printf("load signed byte (register offset)");
                            }
                            break;
                        default:
                            cpu_invalid_instruction(hw1);
                            break;
                    }
                    break;
                default:
                    cpu_invalid_instruction(hw1);
                    break;
            }
            break;
        case 0b011:
            // load/store word/byte immediate offset
            if ((hw1 >> 11) & 0b1) {
                printf("load ");
            } else {
                printf("store ");
            }
            if ((hw1 >> 12) & 0b1) {
                printf("byte ");
            } else {
                printf("word ");
            }
            printf("(immediate offset)");
            break;
        case 0b100:
            // load/store halfword immediate offset
            // load/store stack
            if ((hw1 >> 11) & 0b1) {
                printf("load ");
            } else {
                printf("store ");
            }
            if ((hw1 >> 12) & 0b1) {
                printf("stack (word)");
            } else {
                printf("halfword (immediate offset)");
            }
            break;
        case 0b101:
            if ((hw1 >> 12) & 0b1) {
                // miscellaneous instructions
                // p. 3-9
                switch ((hw1 >> 8) & 0b1111) {
                    case 0b0000:
                        // adjust stack pointer
                        if ((hw1 >> 7) & 0b1) {
                            printf("increment stack pointer by immediate");
                        } else {
                            printf("decrement stack pointer by immediate");
                        }
                        break;
                    case 0b0010:
                        // sign/zero extend
                        if ((hw1 >> 7) & 0b1) {
                            printf("unsigned ");
                        } else {
                            printf("signed ");
                        }
                        printf("extend ");
                        if ((hw1 >> 6) & 0b1) {
                            printf("byte");
                        } else {
                            printf("halfword");
                        }
                        break;
                    case 0b0001:
                    case 0b0011:
                        // compare/branch on zero
                        printf("compare/branch on zero");
                        break;
                    case 0b1001:
                    case 0b1011:
                        // compare/branch on non-zero
                        printf("compare/branch on non-zero");
                        break;
                    case 0b0100:
                    case 0b0101:
                        // push register list
                        printf("push register list");
                        break;
                    case 0b1100:
                    case 0b1101:
                        // pop register list
                        printf("pop register list");
                        break;
                    case 0b0110:
                        switch ((hw1 >> 4) & 0b1111) {
                            case 0b0101:
                                // set endianness
                                printf("endianness");
                                break;
                            case 0b0110:
                            case 0b0111:
                                if ((hw1 >> 3) & 0b1) {
                                    cpu_invalid_instruction(hw1);
                                } else {
                                    // change processor state
                                    printf("change processor state");
                                }
                                break;
                            case 0b0100:
                            default:
                                cpu_invalid_instruction(hw1);
                                break;
                        }
                        break;
                    case 0b1010:
                        // reverse bytes
                        printf("reverse bytes");
                        break;
                    case 0b1110:
                        // software breakpoint
                        printf("software breakpoint");
                        break;
                    case 0b1111:
                        if ((hw1 >> 0) & 0b1111 == 0b0000) {
                            // nop-compatible hints
                            printf("nop");
                        } else {
                            // if-then instructions
                            printf("if-then instructions");
                        }
                        break;
                    default:
                        cpu_invalid_instruction(hw1);
                        break;
                }
            } else {
                // add to sp/pc
                if ((hw1 >> 11) & 0b1) {
                    printf("add to sp");
                } else {
                    printf("add to pc");
                }
            }
            break;
        case 0b110:
            if ((hw1 >> 12) & 0b1) {
                switch ((hw1 >> 8) & 0b1111) {
                    case 0b0000:
                    case 0b0001:
                    case 0b0010:
                    case 0b0011:
                    case 0b0100:
                    case 0b0101:
                    case 0b0110:
                    case 0b0111:
                    case 0b1000:
                    case 0b1001:
                    case 0b1010:
                    case 0b1011:
                    case 0b1100:
                    case 0b1101:
                        // conditional branch
                        printf("conditional branch");
                        break;
                    case 0b1111:
                        // syscall
                        printf("syscall");
                        break;
                    case 0b1110:
                    default:
                        cpu_invalid_instruction(hw1);
                        break;
                }
            } else {
                // load/store multiple
                if ((hw1 >> 11) & 0b1) {
                    printf("load ");
                } else {
                    printf("store ");
                }
                printf("multiple");
            }
            break;
        case 0b111:
            switch ((hw1 >> 11) & 0b11) {
                case 0b00:
                    // unconditional branch
                    printf("unconditional branch");
                    break;
                case 0b01:
                case 0b11:
                    // 32-bit instruction
                    // p. 3-12
                    hw2 = mem_read16(cpu_reg.r15_pc);
                    cpu_reg.r15_pc += 2;
                    switch ((hw1 >> 9) & 0b11) {
                        case 0b00:
                            if ((hw1 >> 12) & 0b1) {
                                // load/store single data item, memory hints
                                cpu_exec_ins_32bit_load_store(hw1, hw2);
                            } else {
                                if ((hw1 >> 6) & 0b1) {
                                    // load/store, double and exclusive
                                    // table branch
                                    cpu_exec_ins_32bit_load_store_double(hw1, hw2);
                                } else {
                                    // load/store multiple
                                    // RFE
                                    // SRS
                                    cpu_exec_ins_32bit_load_store_multi(hw1, hw2);
                                }
                            }
                            break;
                        case 0b01:
                            // data processing (no immediate operand)
                            cpu_exec_ins_32bit_data_processing_no_imm(hw1, hw2);
                            break;
                        case 0b11:
                            // coprocessor
                            cpu_exec_ins_32bit_coprocessor(hw1, hw2);
                            break;
                        default:
                            cpu_invalid_instruction(hw1);
                            break;
                    }
                    break;
                case 0b10:
                    // 32-bit instruction
                    hw2 = mem_read16(cpu_reg.r15_pc);
                    cpu_reg.r15_pc += 2;
                    if ((hw2 >> 15) & 0b1) {
                        // branches, misc control
                        cpu_exec_ins_32bit_branch(hw1, hw2);
                    } else {
                        // data processing immediate
                        cpu_exec_ins_32bit_data_processing_imm(hw1, hw2);
                    }
                    break;
                default:
                    cpu_invalid_instruction(hw1);
                    break;
            }
            break;
        default:
            cpu_invalid_instruction(hw1);
            break;
    }
    printf("\r\n");
}
