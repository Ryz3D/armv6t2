#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "mem.h"
#include "cpu.h"

#define MEM_ROM_BASE (0x00000000)
#define MEM_RAM_BASE (0x10000000)
#define MEM_IO_BASE  (0x40000000)

// TODO: currently ignored
#define MEM_ROM_SIZE (0x00001000)
#define MEM_RAM_SIZE (0x00001000)
#define MEM_IO_SIZE  (0x00001000)

const uint32_t rom[] = {
    // VTOR
    // initial sp
    0x1FFFFFFF, // 0x00
    // reset pc
    MEM_RAM_BASE, // 0x04
};

const uint8_t test_ins[] = {
    // strncat
	0x88, 0xb0, 0x68, 0x23, 0x8d, 0xf8, 0x10, 0x30, 
	0x61, 0x23, 0x8d, 0xf8, 0x11, 0x30, 0x6c, 0x23, 
	0x8d, 0xf8, 0x12, 0x30, 0x8d, 0xf8, 0x13, 0x30, 
	0x6f, 0x22, 0x8d, 0xf8, 0x14, 0x20, 0x20, 0x22, 
	0x8d, 0xf8, 0x15, 0x20, 0x00, 0x22, 0x8d, 0xf8, 
	0x16, 0x20, 0x77, 0x21, 0x8d, 0xf8, 0x00, 0x10, 
	0x65, 0x21, 0x8d, 0xf8, 0x01, 0x10, 0x8d, 0xf8, 
	0x02, 0x30, 0x74, 0x23, 0x8d, 0xf8, 0x03, 0x30, 
	0x8d, 0xf8, 0x04, 0x20, 0x0d, 0xf1, 0x0f, 0x03, 
	0x13, 0xf8, 0x01, 0x2f, 0x00, 0x2a, 0xfb, 0xd1, 
	0x69, 0x46, 0x5a, 0x1e, 0x0a, 0x33, 0x08, 0x78, 
	0x02, 0xf8, 0x01, 0x0f, 0x10, 0xb1, 0x01, 0x31, 
	0x9a, 0x42, 0xf8, 0xd1, 0x00, 0x20, 0x08, 0xb0, 
	0x70, 0x47, 
};

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
        .executable = 1,
        .only_callback = 0,
        .callback_w = NULL,
        .callback_r = NULL,
    });
    mem_rom->buffer = (uint32_t *)rom;
    mem_rom->config.size = sizeof(rom);

    // MEM RAM
    mem_controller_t *mem_ram = mem_add_controller((mem_controller_config_t){
        .base_address = MEM_RAM_BASE,
        .size = MEM_RAM_SIZE,
        .read_only = 0,
        .executable = 1,
        .only_callback = 0,
        .callback_w = NULL,
        .callback_r = NULL,
    });
    memcpy(mem_ram->buffer, test_ins, sizeof(test_ins));

    // MEM IO
    mem_controller_t *mem_io = mem_add_controller((mem_controller_config_t){
        .base_address = MEM_IO_BASE,
        .size = MEM_IO_SIZE,
        .read_only = 0,
        .executable = 0,
        .only_callback = 1,
        .callback_w = io_w,
        .callback_r = io_r,
    });

    cpu_init();

    while (cpu_reg.r15_pc < MEM_RAM_BASE + sizeof(test_ins)) {
        cpu_exec_ins();
    }

    mem_remove_controller(mem_rom);
    mem_remove_controller(mem_ram);
    mem_remove_controller(mem_io);

    return 0;
}
