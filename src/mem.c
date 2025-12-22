#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>

#include "mem.h"

mem_controller_t *mem_first_controller = NULL;

mem_controller_t *mem_add_controller(mem_controller_config_t controller_config) {
    mem_controller_t *new_controller = malloc(sizeof(mem_controller_t));
    if (new_controller == NULL) {
        printf("ERROR: out of memory for memory controller (ironic)\r\n");
        return NULL;
    }
    new_controller->config = controller_config;
    new_controller->next = NULL;
    if (controller_config.only_callback || controller_config.read_only) {
        new_controller->buffer = NULL;
    } else {
        uint32_t size_bytes = ((controller_config.size + 3) / 4) * 4;
        new_controller->buffer = malloc(size_bytes);
        if (new_controller->buffer == NULL) {
            printf("ERROR: out of memory for memory buffer of size %u (ironic)\r\n", controller_config.size);
            return NULL;
        }
        memset(new_controller->buffer, 0, size_bytes);
    }

    if (mem_first_controller == NULL) {
        mem_first_controller = new_controller;
    } else {
        mem_controller_t *last_controller;
        for (last_controller = mem_first_controller; last_controller->next != NULL; last_controller = last_controller->next)
            ;
        last_controller->next = new_controller;
    }

    return new_controller;
}

void mem_remove_controller(mem_controller_t *controller) {
    mem_controller_t *last_controller;
    for (last_controller = mem_first_controller; last_controller != NULL; last_controller = last_controller->next) {
        if (last_controller->next == controller) {
            break;
        }
    }
    if (last_controller->next != controller) {
        printf("WARNING: mem_remove_controller got non-registered controller %p\r\n", controller);
        return;
    }
    last_controller->next = controller->next;
    if (!controller->config.read_only) {
        free(controller->buffer);
    }
    free(controller);
}

uint8_t mem_is_executable(uint32_t addr) {
    for (mem_controller_t *ctrl = mem_first_controller; ctrl != NULL; ctrl = ctrl->next) {
        if (addr >= ctrl->config.base_address && addr < ctrl->config.base_address + ctrl->config.size) {
            return ctrl->config.executable;
        }
    }
    return 0;
}

uint8_t mem_is_writable(uint32_t addr) {
    for (mem_controller_t *ctrl = mem_first_controller; ctrl != NULL; ctrl = ctrl->next) {
        if (addr >= ctrl->config.base_address && addr < ctrl->config.base_address + ctrl->config.size) {
            return !ctrl->config.read_only;
        }
    }
    return 0;
}

void mem_debug(uint32_t base_addr, uint32_t bytes) {
    printf("[0x%04X %04X] mem_debug\r\n",
        base_addr >> 16, base_addr & 0xFFFF);
    for (uint32_t addr = base_addr; addr < base_addr + bytes; addr++) {
        printf("0x%02X ", mem_read8(addr));
    }
    printf("  [0x%04X %04X]\r\n",
        (base_addr + bytes - 1) >> 16, (base_addr + bytes - 1) & 0xFFFF);
}

uint32_t mem_internal_read32(uint32_t addr) {
    if (mem_first_controller == NULL) {
        printf("WARNING: read before any memory defined [0x%04X %04X]\r\n",
            addr >> 16, addr & 0xFFFF);
        return 0;
    }
    for (mem_controller_t *ctrl = mem_first_controller; ctrl != NULL; ctrl = ctrl->next) {
        if (addr >= ctrl->config.base_address && addr < ctrl->config.base_address + ctrl->config.size) {
            uint32_t addr_index = (addr - ctrl->config.base_address) / 4;
            if (addr_index * 4 != addr - ctrl->config.base_address) {
                printf("WARNING: unaligned 32-bit read [0x%04X %04X]\r\n",
                    addr >> 16, addr & 0xFFFF);
            }

            uint32_t ret = 0;
            if (ctrl->config.callback_r != NULL) {
                ret = ctrl->config.callback_r(addr);
            }
            if (ctrl->config.only_callback) {
                return ret;
            } else {
                return ctrl->buffer[addr_index];
            }
        }
    }
    printf("WARNING: read from undefined memory area [0x%04X %04X]\r\n",
        addr >> 16, addr & 0xFFFF);
    return 0;
}

void mem_internal_write32(uint32_t addr, uint32_t data) {
    if (mem_first_controller == NULL) {
        printf("WARNING: write before any memory defined 0x%04X %04X -> [0x%04X %04X]\r\n",
            data >> 16, data & 0xFFFF,
            addr >> 16, addr & 0xFFFF);
        return;
    }
    for (mem_controller_t *ctrl = mem_first_controller; ctrl != NULL; ctrl = ctrl->next) {
        if (addr >= ctrl->config.base_address && addr < ctrl->config.base_address + ctrl->config.size) {
            uint32_t addr_index = (addr - ctrl->config.base_address) / 4;
            if (addr_index * 4 != addr - ctrl->config.base_address) {
                printf("WARNING: unaligned 32-bit write 0x%04X %04X -> [0x%04X %04X]\r\n",
                    data >> 16, data & 0xFFFF,
                    addr >> 16, addr & 0xFFFF);
            }

            if (ctrl->config.callback_w != NULL) {
                ctrl->config.callback_w(addr, data);
            }
            if (ctrl->config.read_only) {
                printf("WARNING: attempted write to read-only memory 0x%04X %04X -> [0x%04X %04X]\r\n",
                    data >> 16, data & 0xFFFF,
                    addr >> 16, addr & 0xFFFF);
                return;
            }
            if (!ctrl->config.only_callback) {
                ctrl->buffer[addr_index] = data;
            }
            return;
        }
    }
    printf("WARNING: write to undefined memory area 0x%04X %04X -> [0x%04X %04X]\r\n",
        data >> 16, data & 0xFFFF,
        addr >> 16, addr & 0xFFFF);
    return;
}

uint8_t mem_read8(uint32_t addr) {
    // addr = 5
    // 4    .
    // 0xAA 0xBB 0xCC 0xDD
    // 0xDDCCBBAA -> 0xBB
    uint32_t word_addr = (addr / 4) * 4;
    uint8_t byte_offset = addr - word_addr;
    return mem_internal_read32(word_addr) >> (byte_offset * 8);
}

uint16_t mem_read16(uint32_t addr) {
    // addr = 5
    // 4    .
    // 0xAA 0xBB 0xCC 0xDD
    // 0xDDCCBBAA -> 0xCCBB
    uint32_t word_addr = (addr / 4) * 4;
    uint8_t byte_offset = addr - word_addr;
    if (byte_offset & 0b1) {
        printf("WARNING: unaligned 16-bit read [0x%04X %04X]\r\n",
            addr >> 16, addr & 0xFFFF);
    }
    return mem_internal_read32(word_addr) >> (byte_offset * 8);
}

uint32_t mem_read32(uint32_t addr) {
    return mem_internal_read32(addr);
}

void mem_write8(uint32_t addr, uint8_t data) {
    // addr = 5
    // 4    .
    // 0xAA 0xBB 0xCC 0xDD
    // 0xDDCCBBAA -> 0xBB
    uint32_t word_addr = (addr / 4) * 4;
    uint8_t byte_offset = addr - word_addr;
    uint32_t temp = mem_internal_read32(word_addr);
    temp &= ~(0xFF << (byte_offset * 8));
    temp |= (data << (byte_offset * 8));
    mem_internal_write32(word_addr, temp);
}

void mem_write16(uint32_t addr, uint16_t data) {
    // addr = 5
    // 4    .
    // 0xAA 0xBB 0xCC 0xDD
    // 0xDDCCBBAA -> 0xCCBB
    uint32_t word_addr = (addr / 4) * 4;
    uint8_t byte_offset = addr - word_addr;
    if (byte_offset & 0b1) {
        printf("WARNING: unaligned 16-bit write 0x%04X -> [0x%04X %04X]\r\n",
            data,
            addr >> 16, addr & 0xFFFF);
    }
    uint32_t temp = mem_internal_read32(word_addr);
    temp &= ~(0xFFFF << (byte_offset * 8));
    temp |= (data << (byte_offset * 8));
    mem_internal_write32(word_addr, temp);
}

void mem_write32(uint32_t addr, uint32_t data) {
    mem_internal_write32(addr, data);
}
