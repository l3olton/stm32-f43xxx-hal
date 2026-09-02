#include "usart.h"
#include "rcc.h"
#include "gpio.h"
#include "nvic.h"
#include "ringbuffer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define USART_RING_BUF_SIZE 64

typedef struct {
    Usart *usart;
    RingBuffer tx_ring_buffer;
    RingBuffer rx_ring_buffer;
} UsartHandle;

static uint8_t usart_1_tx_buffer[USART_RING_BUF_SIZE];
static uint8_t usart_1_rx_buffer[USART_RING_BUF_SIZE];

static uint8_t usart_2_tx_buffer[USART_RING_BUF_SIZE];
static uint8_t usart_2_rx_buffer[USART_RING_BUF_SIZE];

static uint8_t usart_3_tx_buffer[USART_RING_BUF_SIZE];
static uint8_t usart_3_rx_buffer[USART_RING_BUF_SIZE];

static UsartHandle usart_1_handle;
static UsartHandle usart_2_handle;
static UsartHandle usart_3_handle;

static UsartHandle *get_usart_handle(const Usart *usart)
{
    if (usart == USART1) return &usart_1_handle;
    if (usart == USART2) return &usart_2_handle;
    if (usart == USART3) return &usart_3_handle;
    return NULL;
}

void usart_init(Usart *usart, const uint32_t usart_div)
{
    const uint8_t af = 7;
    uint8_t irq_handler;
    uint16_t rx = 0, tx = 0;

    if (usart == USART1) {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        tx = PIN('A', 9); // select transmission pin
        rx = PIN('A', 10); // select receiving pin
        irq_handler = 37; // TODO: create #defines for these or something
        usart_1_handle.usart = USART1;
        ring_buf_init(&usart_1_handle.tx_ring_buffer, usart_1_tx_buffer, USART_RING_BUF_SIZE);
        ring_buf_init(&usart_1_handle.rx_ring_buffer, usart_1_rx_buffer, USART_RING_BUF_SIZE);
    } else if (usart == USART2) {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        tx = PIN('A', 2);
        rx = PIN('A', 3);
        irq_handler = 38;
        usart_2_handle.usart = USART2;
        ring_buf_init(&usart_2_handle.tx_ring_buffer, usart_2_tx_buffer, USART_RING_BUF_SIZE);
        ring_buf_init(&usart_2_handle.rx_ring_buffer, usart_2_rx_buffer, USART_RING_BUF_SIZE);
    } else if (usart == USART3) {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        tx = PIN('D', 8);
        rx = PIN('D', 9);
        irq_handler = 39;
        usart_3_handle.usart = USART3;
        ring_buf_init(&usart_3_handle.tx_ring_buffer, usart_3_tx_buffer, USART_RING_BUF_SIZE);
        ring_buf_init(&usart_3_handle.rx_ring_buffer, usart_3_rx_buffer, USART_RING_BUF_SIZE);
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
}

// TODO: check if these get inlined
static uint32_t usart_has_overrun_err(const Usart *usart)
{
    return usart->SR & USART_SR_ORE;
}

static uint32_t usart_has_framing_err(const Usart *usart)
{
    return usart->SR & USART_SR_FE;
}

static uint32_t usart_has_parity_err(const Usart *usart)
{
    return usart->SR & USART_SR_PE;
}

static uint32_t usart_has_idle_line(const Usart *usart)
{
    return usart->SR & USART_SR_IDLE;
}

static uint32_t usart_read_ready(const Usart *usart) {
    return usart->SR & USART_SR_RXNE;
}

static uint32_t usart_transmission_data_reg_empty(const Usart *usart)
{
    return usart->SR & USART_SR_TXE;
}

static uint32_t usart_transmit_empty_interrupt_enabled(const Usart *usart)
{
    return usart->CR1 & USART_CR1_TXEIE;
}

static uint32_t usart_transmission_complete(const Usart *usart)
{
    return usart->SR & USART_SR_TC;
}

static uint32_t usart_transmission_complete_interrupt_enabled(const Usart *usart)
{
    return usart->CR1 & USART_CR1_TCIE;
}

static void usart_transmit_empty_interrupt_enable(Usart *usart)
{
    usart->CR1 |= USART_CR1_TXEIE;
}

static void usart_transmit_empty_interrupt_disable(Usart *usart)
{
    usart->CR1 &= ~USART_CR1_TXEIE;
}

static void usart_transmission_complete_interrupt_enable(Usart *usart)
{
    usart->CR1 |= USART_CR1_TCIE;
}

static void usart_transmission_complete_interrupt_disable(Usart *usart)
{
    usart->CR1 &= ~USART_CR1_TCIE;
}

static uint8_t usart_read_byte(const Usart *usart)
{
    return (uint8_t) (usart->DR & 255); // bottom 8 bits of DR register hold received value
}

void handle_usart_interrupt(Usart *usart, const UsartReceiveInterruptCallback callback)
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

    UsartHandle *usart_handle = get_usart_handle(usart);
    if (!usart_handle) return;

    if (usart_read_ready(usart)) {
        ring_buf_push(&usart_handle->rx_ring_buffer, usart_read_byte(usart));
        return;
    }

    if (usart_has_idle_line(usart)) {
        uint8_t out;
        while (ring_buf_pop(&usart_handle->rx_ring_buffer, &out))
            callback(out);
        printf("\r\n");
        usart->DR; // read to DR after read to SR clears SR IDLE bit
        return;
    }

    if (usart_transmission_data_reg_empty(usart) && usart_transmit_empty_interrupt_enabled(usart)) {
        if (!ring_buf_pop(&usart_handle->tx_ring_buffer, (uint8_t *) &usart->DR)) {
            usart_transmit_empty_interrupt_disable(usart);
            usart_transmission_complete_interrupt_enable(usart);
        }
        return;
    }

    if (usart_transmission_complete(usart) && usart_transmission_complete_interrupt_enabled(usart)) {
        usart_transmission_complete_interrupt_disable(usart);
        return;
    }
}

void usart_write_buffer(Usart *usart, const uint8_t *buffer, size_t len)
{
    UsartHandle *usart_handle = get_usart_handle(usart);
    if (!usart_handle) return;

    if (len > USART_RING_BUF_SIZE) len = USART_RING_BUF_SIZE;
    while (len-- > 0) ring_buf_push(&usart_handle->tx_ring_buffer, *buffer++);

    usart_transmit_empty_interrupt_enable(usart);
    while (usart_transmission_data_reg_empty(usart) == 0) {}
    ring_buf_pop(&usart_handle->tx_ring_buffer, (uint8_t *) &usart->DR);
}
