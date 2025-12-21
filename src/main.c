#include "main.h"

#include "mem.h"
#include "cpu.h"

// TODO: uint8
extern uint8_t ram[];

const uint32_t test_ins[] = {
    /*
    0b0000000000000000, // move
    0b0000010100000000, // lsl
    0b0000100000000000, // lsr
    0b0001000000000000, // asr
    0b0001100000000000, // add reg
    0b0001101000000000, // sub reg
    0b0001110000000000, // add imm
    0b0001111000000000, // sub imm
    0b0010000000000000, // mov imm
    0b0010100000000000, // cmp imm
    0b0011000000000000, // add imm
    0b0011100000000000, // sub imm
    */
   0xEF88 | (0xABCD << 16),
};

int main() {
    mem_init();
    cpu_init();

    for (uint32_t i = 0; i < sizeof(test_ins) / sizeof(*test_ins); i++) {
        if (test_ins[i] >> 16 == 0) {
            MEM_WRITE16(0, test_ins[i]);
        } else {
            MEM_WRITE32(0, test_ins[i]);
        }
        cpu_reg.pc = 0;
        cpu_exec_ins();
    }

    return 0;
}
