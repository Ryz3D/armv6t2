#pragma once

#include <stdint.h>

typedef struct mem_controller_config {
    uint32_t base_address;
    uint32_t size;
    uint8_t read_only;
    uint8_t executable;
    uint8_t only_callback;
    void (*callback_w)(uint32_t addr, uint32_t data);
    uint32_t (*callback_r)(uint32_t addr);
} mem_controller_config_t;

typedef struct mem_controller mem_controller_t;
typedef struct mem_controller {
    mem_controller_config_t config;
    mem_controller_t *next;
    uint32_t *buffer;
} mem_controller_t;

mem_controller_t *mem_add_controller(mem_controller_config_t controller_config);
void mem_remove_controller(mem_controller_t *controller);
uint8_t mem_is_executable(uint32_t addr);
void mem_debug(uint32_t base_addr, uint32_t size);

uint8_t  mem_read8(uint32_t addr);
uint16_t mem_read16(uint32_t addr);
uint32_t mem_read32(uint32_t addr);
void mem_write8(uint32_t addr,  uint8_t data);
void mem_write16(uint32_t addr, uint16_t data);
void mem_write32(uint32_t addr, uint32_t data);
