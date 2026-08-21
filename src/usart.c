#include "usart.h"
#include "rcc.h"
#include "gpio.h"
#include "nvic.h"
#include "ringbuffer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define USART_RING_BUF_SIZE 64
static uint8_t buf[USART_RING_BUF_SIZE];
static RingBuffer usart_ring_buffer = {0};

void usart_init(Usart *usart, const uint32_t usart_div)
{
    const uint8_t af = 7;
    uint8_t irq_handler;
    uint16_t rx = 0, tx = 0;

    if (usart == USART1) {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        tx = PIN('A', 9); // select transmission pin
        rx = PIN('A', 10); // select receiving pin
        irq_handler = 37;
    } else if (usart == USART2) {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        tx = PIN('A', 2);
        rx = PIN('A', 3);
        irq_handler = 38;
    } else if (usart == USART3) {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        tx = PIN('D', 8);
        rx = PIN('D', 9);
        irq_handler = 39;
    } else {
        return; // TODO: maybe handle differently
    }

    gpio_set_mode(tx, GPIO_MODE_AF);
    gpio_set_af(tx, af);
    gpio_set_mode(rx, GPIO_MODE_AF);
    gpio_set_af(rx, af);

    usart->CR1 &= ~BIT(12) & ~BIT(15); // set data length to 8 bits and OVER8 to 0 (oversample by 16 bits)
    usart->CR2 &= ~BIT(12) & ~BIT(13); // set no. of stop bits to 1
    usart->BRR = usart_div;
    usart->CR1 |= USART_CR1_UE
        | USART_CR1_TE
        | USART_CR1_RE
        | USART_CR1_RXNEIE
        | USART_CR1_IDLEIE;

    nvic_enable_interrupt(irq_handler);
    ring_buf_init(&usart_ring_buffer, buf, USART_RING_BUF_SIZE);
}

static inline uint32_t usart_has_overrun_err(Usart *usart)
{
    return usart->SR & USART_SR_ORE;
}

static inline uint32_t usart_has_framing_err(Usart *usart)
{
    return usart->SR & USART_SR_FE;
}

static inline uint32_t usart_has_parity_err(Usart *usart)
{
    return usart->SR & USART_SR_PE;
}

static inline uint32_t usart_has_idle_line(Usart *usart)
{
    return usart->SR & USART_SR_IDLE;
}

static inline uint32_t usart_read_ready(Usart *usart) {
    return usart->SR & USART_SR_RXNE;
}

static inline uint32_t usart_transmission_complete(Usart *usart)
{
    return usart->SR & USART_SR_TC;
}

static inline uint8_t usart_read_byte(Usart *usart)
{
    return (uint8_t) (usart->DR & 255); // bottom 8 bits of DR register hold received value
}

void handle_usart_interrupt(Usart *usart)
{
    if (usart_has_overrun_err(usart)) {
        printf("Overrun error\r\n");
        return;
    }

    if (usart_has_framing_err(usart)) {
        printf("Framing error\r\n");
        return;
    }

    if (usart_has_parity_err(usart)) {
        printf("Parity error\r\n");
        return;
    }

    if (usart_read_ready(usart))
        ring_buf_push(&usart_ring_buffer, usart_read_byte(usart));

    if (usart_has_idle_line(usart)) {
        uint8_t out;
        while (ring_buf_pop(&usart_ring_buffer, &out))
            putchar(out);
        printf("\r\n");
        usart->DR; // read to DR after read to SR clears SR IDLE bit
    }
}

static inline void usart_write_byte(Usart *usart, const uint8_t data)
{
    // TODO: use transmission complete interrupt instead of blocking
    while (!(usart_transmission_complete(usart)));
    usart->DR = data;
}

void usart_write_buffer(Usart *usart, const char *buffer, size_t len)
{
    while (len-- > 0) usart_write_byte(usart, *(uint8_t *) buffer++);
}
