#include "gpio.h"
#include "usart.h"
#include "systick.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define FREQ 16000000 // CPU frequency 16Mhz

extern void HardFault_Handler(void)
{
    printf("Hard fault\r\n");
    while (1);
}

// TODO: separate into 'application layer'
static void control_leds(RingBuffer *rb)
{
    uint8_t color, command, nl;

    if (!ring_buf_pop(rb, &color)) return;
    if (!ring_buf_pop(rb, &command)) return;
    if (!ring_buf_pop(rb, &nl)) return;

    if (nl != '\n') {
        ring_buf_reset(rb);
        return;
    }

    uint16_t pin;
    switch (color) {
        case 'R': pin = RED_LED_PIN; break;
        case 'G': pin = GREEN_LED_PIN; break;
        case 'B': pin = BLUE_LED_PIN; break;
        default: ring_buf_reset(rb); return;
    }

    bool value;
    switch (command) {
        case '1': value = true; break;
        case '0': value = false; break;
        default: ring_buf_reset(rb); return;
    }

    gpio_write(pin, value);
}

extern void USART3_IRQHandler(void)
{
    handle_usart_interrupt(USART3, control_leds);
}

int main(void)
{
    systick_init(16000000 / 1000);

    // usart_init(USART3, 0x008B); // divider for 115200 baud rate
    usart_init(USART3, 0x0683); // divider for 9600 baud rate
    printf("بسم الله\r\n"); // redirected to USART3

    gpio_set_mode(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(RED_LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {}

    return 0;
}
