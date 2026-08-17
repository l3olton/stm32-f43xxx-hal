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
#define USART1_CLOCK_ENABLE BIT(4)
#define USART2_CLOCK_ENABLE BIT(17)
#define USART3_CLOCK_ENABLE BIT(18)
#define USART_ENABLE BIT(13)
#define USART_TRANSMITTER_ENABLE BIT(3)
#define USART_TRANSMISSION_COMPLETE BIT(6)
#define USART_RECEIVER_ENABLE BIT(2)
#define USART_RECEIVE_INTERRUPT_ENABLE BIT(5)
#define USART_IDLE_INTERRUPT_ENABLE BIT(4)
#define USART_SR_PE BIT(0)
#define USART_SR_FE BIT(1)
#define USART_SR_ORE BIT(3)
#define USART_SR_IDLE BIT(4)
#define USART_SR_RXNE BIT(5)

void usart_init(struct Usart *usart, unsigned long usart_div);

void handle_usart_interrupt(struct Usart *usart);

void usart_write_buffer(struct Usart *usart, const char *buffer, size_t len);

#endif
