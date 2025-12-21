#pragma once

#include <stdint.h>

uint8_t ram[1024];

#define MEM_READ8(a)  (ram[(a) % sizeof(ram)])

#define MEM_READ16(a) (MEM_READ8(a) \
                    | ((uint16_t)MEM_READ8((a) + 1) << 8))

#define MEM_READ32(a) (MEM_READ8(a) \
                    | ((uint16_t)MEM_READ8((a) + 1) << 8 ) \
                    | ((uint32_t)MEM_READ8((a) + 2) << 16) \
                    | ((uint32_t)MEM_READ8((a) + 3) << 24))

#define MEM_WRITE8(a, d) ram[(a) % sizeof(ram)] = ((uint8_t)(d))

#define MEM_WRITE16(a, d) MEM_WRITE8(a,       ((uint16_t)(d)      ) & 0xFF); \
                          MEM_WRITE8((a) + 1, ((uint16_t)(d) >> 8 ) & 0xFF)

#define MEM_WRITE32(a, d) MEM_WRITE8(a,       ((uint32_t)(d)      ) & 0xFF); \
                          MEM_WRITE8((a) + 1, ((uint32_t)(d) >> 8 ) & 0xFF); \
                          MEM_WRITE8((a) + 2, ((uint32_t)(d) >> 16) & 0xFF); \
                          MEM_WRITE8((a) + 3, ((uint32_t)(d) >> 24) & 0xFF)

void mem_init();
