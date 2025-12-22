#include <stdlib.h>
#include <stdio.h>

#include "mem.h"
#include "cpu.h"

const uint32_t test_ins[] = {
    /*
    0b0000000000000000, // mov
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

#define MEM_ROM_BASE (0x00000000)
#define MEM_RAM_BASE (0x10000000)
#define MEM_IO_BASE  (0x40000000)

// TODO: currently ignored
#define MEM_ROM_SIZE (0x00001000)
#define MEM_RAM_SIZE (0x00001000)
#define MEM_IO_SIZE  (0x00001000)

void io_w(uint32_t addr, uint32_t data) {
    if (addr - MEM_IO_BASE == 0x10) {
        printf("%c", (char)(data & 0xFF));
    }
}

uint32_t io_r(uint32_t addr) {
    return 0;
}

int main() {
    // MEM ROM
    mem_controller_t *mem_rom = mem_add_controller((mem_controller_config_t){
        .base_address = MEM_ROM_BASE,
        // .size = MEM_ROM_SIZE,
        .read_only = 1,
        .only_callback = 0,
        .callback_w = NULL,
        .callback_r = NULL,
    });
    mem_rom->buffer = (uint32_t *)test_ins;
    mem_rom->config.size = sizeof(test_ins);
    // MEM RAM
    mem_controller_t *mem_ram = mem_add_controller((mem_controller_config_t){
        .base_address = MEM_RAM_BASE,
        .size = MEM_RAM_SIZE,
        .read_only = 0,
        .only_callback = 0,
        .callback_w = NULL,
        .callback_r = NULL,
    });
    // MEM IO
    mem_controller_t *mem_io = mem_add_controller((mem_controller_config_t){
        .base_address = MEM_IO_BASE,
        .size = MEM_IO_SIZE,
        .read_only = 0,
        .only_callback = 1,
        .callback_w = io_w,
        .callback_r = io_r,
    });

    mem_write8(MEM_RAM_BASE + 0x00, 0xA0);
    mem_write8(MEM_RAM_BASE + 0x01, 0x01);
    mem_write8(MEM_RAM_BASE + 0x02, 0x01);
    mem_write8(MEM_RAM_BASE + 0x03, 0x01);
    mem_debug(MEM_RAM_BASE, 0x08);
    mem_write16(MEM_RAM_BASE + 0x02, 0xB0C0);
    mem_debug(MEM_RAM_BASE, 0x08);
    mem_write32(MEM_RAM_BASE + 0x00, 0xD0C0B0A0);
    mem_debug(MEM_RAM_BASE, 0x08);

    mem_write8(MEM_IO_BASE + 0x10, 'g');
    mem_write8(MEM_IO_BASE + 0x10, 'u');
    mem_write8(MEM_IO_BASE + 0x10, 'd');
    mem_write8(MEM_IO_BASE + 0x10, 'e');
    mem_write8(MEM_IO_BASE + 0x10, ',');
    mem_write8(MEM_IO_BASE + 0x10, ' ');
    mem_write8(MEM_IO_BASE + 0x10, 'w');
    mem_write8(MEM_IO_BASE + 0x10, 'e');
    mem_write8(MEM_IO_BASE + 0x10, 'l');
    mem_write8(MEM_IO_BASE + 0x10, 't');
    mem_write8(MEM_IO_BASE + 0x10, '\r');
    mem_write8(MEM_IO_BASE + 0x10, '\n');

    return 0;

    /*
    // initial sp
    MEM_WRITE32(CPU_VECTOR_TABLE + 0x0000, 0x1FFFFFFF);
    // reset pc
    MEM_WRITE32(CPU_VECTOR_TABLE + 0x0004, 0x20000000);
    // TODO: partial memory regions!
    cpu_init();

    for (uint32_t i = 0; i < sizeof(test_ins) / sizeof(*test_ins); i++) {
        if (test_ins[i] >> 16 == 0) {
            MEM_WRITE16(0x20000000, test_ins[i]);
        } else {
            MEM_WRITE32(0x20000000, test_ins[i]);
        }
        cpu_reg.r15_pc = 0;
        cpu_exec_ins();
    }

    return 0;
    */
}
