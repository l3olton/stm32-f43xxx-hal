#include "gpio.h"
#include "usart.h"
#include "systick.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define FREQ 16000000 // CPU frequency 16Mhz

int main(void) {
    systick_init(16000000 / 1000);

    // usart_init(USART3, 0x008B); // divider for 115200 baud rate
    usart_init(USART3, 0x0683); // divider for 9600 baud rate

    usart_write_buffer(USART3, "بسم الله\n", 16);

    gpio_set_mode(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(RED_LED_PIN, GPIO_MODE_OUTPUT);

    uint32_t led_timer = 0;
    uint32_t usart_timer = 0;
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

        if (timer_expired(&usart_timer, 1000, s_ticks)) {
            usart_write_buffer(USART3, "لا إله إلا الله\n", 28);
        }
    }

    return 0;
}
