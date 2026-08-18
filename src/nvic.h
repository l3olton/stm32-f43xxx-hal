#ifndef NVIC_H
#define NVIC_H

#include "utils.h"
#include <stdint.h>

struct Nvic {
  volatile uint32_t ISER[8];
  uint32_t RESERVED0[24];
  volatile uint32_t ICER[8];
  uint32_t RSERVED1[24];
  volatile uint32_t ISPR[8];
  uint32_t RESERVED2[24];
  volatile uint32_t ICPR[8];
  uint32_t RESERVED3[24];
  volatile uint32_t IABR[8];
  uint32_t RESERVED4[56];
  volatile uint8_t  IP[240];
  uint32_t RESERVED5[644];
  volatile  uint32_t STIR;
};
#define NVIC ((struct Nvic *) 0xe000e100)

static inline void nvic_enable_interrupt(const uint8_t vector_position)
{
    if (vector_position <= 239) // NVIC supports 240 interrupts 0-239
        NVIC->ISER[vector_position / 32] |= BIT(vector_position % 32);
}

#endif
