#ifndef USART_H
#define USART_H

#include "utils.h"
#include <stddef.h>
#include <stdint.h>

// TODO: do the struct fields need to public?
typedef struct {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} Usart;

#define USART1 ((Usart *) 0x40011000)
#define USART2 ((Usart *) 0x40004400)
#define USART3 ((Usart *) 0x40004800)

// TODO: do these macros need to be public?

#define USART_CR1_RE BIT(2)
#define USART_CR1_TE BIT(3)
#define USART_CR1_IDLEIE BIT(4)
#define USART_CR1_RXNEIE BIT(5)
#define USART_CR1_TCIE BIT(6)
#define USART_CR1_TXEIE BIT(7)
#define USART_CR1_UE BIT(13)

#define USART_SR_PE BIT(0)
#define USART_SR_FE BIT(1)
#define USART_SR_ORE BIT(3)
#define USART_SR_IDLE BIT(4)
#define USART_SR_RXNE BIT(5)
#define USART_SR_TC BIT(6)
#define USART_SR_TXE BIT(7)

void usart_init(Usart *usart, uint32_t usart_div);

void handle_usart_interrupt(Usart *usart);

void usart_write_buffer(Usart *usart, const uint8_t *buffer, size_t len);

#endif
