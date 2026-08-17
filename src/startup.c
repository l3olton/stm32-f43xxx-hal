__attribute__((naked, noreturn)) void _reset(void)
{
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

    extern int main(void);
    main();
    while (1) (void) 0;
}

extern void _estack(void);
extern void HardFault_Handler(void);
extern void SysTick_Handler(void);
extern void USART3_IRQHandler(void);

__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, // 0
    _reset, // 1
    0, // 2
    HardFault_Handler, // 3
    0, // 4
    0, // 5
    0, // 6
    0, // 7
    0, // 8
    0, // 9
    0, // 10
    0, // 11
    0, // 12
    0, // 13
    0, // 14
    SysTick_Handler, // 15 - end of system interrupts
    0, // 0
    0, // 1
    0, // 2
    0, // 3
    0, // 4
    0, // 5
    0, // 6
    0, // 7
    0, // 8
    0, // 9
    0, // 10
    0, // 11
    0, // 12
    0, // 13
    0, // 14
    0, // 15
    0, // 16
    0, // 17
    0, // 18
    0, // 19
    0, // 20
    0, // 21
    0, // 22
    0, // 23
    0, // 24
    0, // 25
    0, // 26
    0, // 27
    0, // 28
    0, // 29
    0, // 30
    0, // 31
    0, // 32
    0, // 33
    0, // 34
    0, // 35
    0, // 36
    0, // 37
    0, // 38
    USART3_IRQHandler // 39
};
