#ifndef USART_H
#define USART_H

#include "utils.h"
#include <stddef.h>
#include <stdint.h>

// TODO: do the struct fields need to public?
struct Usart {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
};

#define USART1 ((struct Usart *) 0x40011000)
#define USART2 ((struct Usart *) 0x40004400)
#define USART3 ((struct Usart *) 0x40004800)

// TODO: do these macros need to be public?

#define USART_CR1_UE BIT(13)
#define USART_CR1_RE BIT(2)
#define USART_CR1_TE BIT(3)
#define USART_CR1_IDLEIE BIT(4)
#define USART_CR1_RXNEIE BIT(5)

#define USART_SR_PE BIT(0)
#define USART_SR_FE BIT(1)
#define USART_SR_ORE BIT(3)
#define USART_SR_IDLE BIT(4)
#define USART_SR_RXNE BIT(5)
#define USART_SR_TC BIT(6)

void usart_init(struct Usart *usart, unsigned long usart_div);

void handle_usart_interrupt(struct Usart *usart);

void usart_write_buffer(struct Usart *usart, const char *buffer, size_t len);

#endif
