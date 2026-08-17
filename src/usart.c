#include "usart.h"
#include "rcc.h"
#include "gpio.h"
#include "nvic.h"
#include "ringbuffer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void usart_init(struct Usart *usart, unsigned long usart_div) {
    uint8_t af = 7;
    uint16_t rx = 0, tx = 0;

    if (usart == USART1) {
        RCC->APB2ENR |= USART1_CLOCK_ENABLE;
        tx = PIN('A', 9); // select transmission pin
        rx = PIN('A', 10); // select receiving pin
    } else if (usart == USART2) {
        RCC->APB1ENR |= USART2_CLOCK_ENABLE;
        tx = PIN('A', 2);
        rx = PIN('A', 3);
    } else if (usart == USART3) {
        RCC->APB1ENR |= USART3_CLOCK_ENABLE;
        tx = PIN('D', 8);
        rx = PIN('D', 9);
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
    usart->CR1 |= USART_ENABLE
        | USART_TRANSMITTER_ENABLE
        | USART_RECEIVER_ENABLE
        | USART_RECEIVE_INTERRUPT_ENABLE
        | USART_IDLE_INTERRUPT_ENABLE;

    // TODO: make generic
    NVIC->ISER[1] |= BIT(7); // enable USART3 interrupt handler in NVIC
}

static inline int usart_read_ready(struct Usart *usart) {
    return usart->SR & USART_RECEIVE_READY;
}

static inline uint8_t usart_read_byte(struct Usart *usart) {
    return (uint8_t) (usart->DR & 255); // bottom 8 bits of DR register hold received value
}

void usart_read_buffer(struct Usart *usart, uint8_t *buffer, size_t len) { // maybe change to char *buffer
    // while (len-- > 0) *buffer++ = usart_read_byte(usart);
    for (size_t i = 0; i < len; ++i) {
        if (usart_read_ready(usart)) {
            buffer[i] = usart_read_byte(usart);
        }
    }
}

// TODO: separate handler logic from specific usart peripheral (usart3)
void USART3_IRQHandler(void) {
    // TODO: move direct bit operations into functions
    if (USART3->SR & BIT(3)) {
        // ORE bit set (overrun error)
        printf("Overrun error\r\n");
        return;
    }

    if (USART3->SR & BIT(1)) {
        // FE bit set (framing error)
        printf("Framing error\r\n");
        return;
    }

    if (USART3->SR & BIT(0)) {
        // PE bit set (parity error)
        printf("Parity error\r\n");
        return;
    }

    if (USART3->SR & BIT(5))
        ring_buf_push(USART3->DR & 255);

    if (USART3->SR & USART_SR_IDLE) {
        // IDLE bit set (idle line detected)
        uint8_t out;
        while (ring_buf_pop(&out))
            putchar(out);
        printf("\r\n");
        USART3->SR;
        USART3->DR;
    }
}

static inline void usart_write_byte(struct Usart *usart, uint8_t data) {
    while (!(usart->SR & USART_TRANSMISSION_COMPLETE));
    usart->DR = data;
}

void usart_write_buffer(struct Usart *usart, const char *buffer, size_t len) {
    while (len-- > 0) usart_write_byte(usart, *(uint8_t *) buffer++);
}
