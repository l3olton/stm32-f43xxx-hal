#include "gpio.h"
#include "usart.h"
#include "systick.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define FREQ 16000000 // CPU frequency 16Mhz

void HardFault_Handler(void) {
    printf("Hard fault\r\n");
    while (1);
}

int main(void) {
    systick_init(16000000 / 1000);

    // usart_init(USART3, 0x008B); // divider for 115200 baud rate
    usart_init(USART3, 0x0683); // divider for 9600 baud rate

    printf("بسم الله\r\n"); // redirected to USART3

    gpio_set_mode(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(RED_LED_PIN, GPIO_MODE_OUTPUT);

    uint32_t led_timer = 0;
    uint32_t delay_timer = 0;

    while (1) {
        if (timer_expired(&led_timer, 1500, s_ticks)) {
            gpio_write(RED_LED_PIN, 1);
            DELAY(delay_timer, 250);
            gpio_write(BLUE_LED_PIN, 1);
            DELAY(delay_timer, 250);
            gpio_write(GREEN_LED_PIN, 1);
            DELAY(delay_timer, 250);
            gpio_write(RED_LED_PIN, 0);
            DELAY(delay_timer, 250);
            gpio_write(BLUE_LED_PIN, 0);
            DELAY(delay_timer, 250);
            gpio_write(GREEN_LED_PIN, 0);
            DELAY(delay_timer, 250);
        }
    }

    return 0;
}
