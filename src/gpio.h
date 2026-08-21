#ifndef GPIO_H
#define GPIO_H

#include "rcc.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>

#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

#define BLUE_LED_PIN PIN('B', 7)
#define GREEN_LED_PIN PIN('B', 0)
#define RED_LED_PIN PIN('B', 14)

typedef struct {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
} Gpio;
#define GPIO(bank) ((Gpio *) (0x40020000 + 0x400 * (bank)))

typedef enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG } GpioMode;

// TODO: possibly rewrite for readability
static inline void gpio_set_mode(const uint16_t pin, const GpioMode mode)
{
    Gpio *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);
    RCC->AHB1ENR |= BIT(PINBANK(pin)); // enable clock for pin
    gpio->MODER &= ~(3U << (n * 2)); // clear mode register
    gpio->MODER |= (mode & 3U) << (n * 2); // set mode register
}

// TODO: possibly rewrite for readability
static inline void gpio_write(const uint16_t pin, const bool val)
{
    Gpio *gpio = GPIO(PINBANK(pin));
    gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

// TODO: possibly rewrite for readability
static inline void gpio_set_af(const uint16_t pin, const uint8_t af_num)
{
    Gpio *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);
    gpio->AFR[n >> 3] &= ~(15UL << ((n & 7) * 4)); // clearing AFR bits (?)
    gpio->AFR[n >> 3] |= ((uint32_t) af_num) << ((n & 7) * 4); // setting AFR bits
}

#endif
