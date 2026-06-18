#include <stdint.h>

// MPS2-AN385 UART0 register (we'll print via UART to confirm execution)
#define UART0_BASE    0x40004000
#define UART0_DATA    (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_STATE   (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART0_CTRL    (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART0_BAUDDIV (*(volatile uint32_t *)(UART0_BASE + 0x10))

void uart_init(void) {
    UART0_BAUDDIV = 16;
    UART0_CTRL    = 1;  // enable TX
}

void uart_putc(char c) {
    while (UART0_STATE & 1);  // wait until TX buffer empty
    UART0_DATA = c;
}

void uart_print(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    uart_init();

    while(1) {
        uart_print("LED ON\r\n");
        delay(500000);
        uart_print("LED OFF\r\n");
        delay(500000);
    }

    return 0;
}