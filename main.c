#include <stdint.h>

// ─── UART registers (mps2-an385) ──────────────────
#define UART0_BASE    0x40004000
#define UART0_DATA    (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_STATE   (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART0_CTRL    (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART0_BAUDDIV (*(volatile uint32_t *)(UART0_BASE + 0x10))

// ─── SysTick registers (built into ARM Cortex-M) ──
#define SYSTICK_BASE  0xE000E010
#define SYSTICK_CTRL  (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD  (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL   (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

// ─── NVIC registers ───────────────────────────────
#define NVIC_ISER0    (*(volatile uint32_t *)(0xE000E100))

// ─── Global counter (shared between main and ISR) ─
volatile uint32_t tick_count = 0;

// ─── UART functions ───────────────────────────────
void uart_init(void) {
    UART0_BAUDDIV = 16;
    UART0_CTRL    = 1;
}

void uart_putc(char c) {
    while (UART0_STATE & 1);
    UART0_DATA = c;
}

void uart_print(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

// simple number printer
void uart_print_num(uint32_t num) {
    if (num == 0) {
        uart_putc('0');
        return;
    }
    char buf[10];
    int i = 0;
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    // print in reverse
    for (int j = i - 1; j >= 0; j--) {
        uart_putc(buf[j]);
    }
}

// ─── SysTick setup ────────────────────────────────
void systick_init(uint32_t ticks) {
    SYSTICK_LOAD = ticks - 1;  // reload value
    SYSTICK_VAL  = 0;          // clear current value
    // CTRL bits:
    // bit 0 = enable counter
    // bit 1 = enable interrupt
    // bit 2 = use processor clock
    SYSTICK_CTRL = (1 << 0) | (1 << 1) | (1 << 2);
}

// ─── SysTick ISR ──────────────────────────────────
// This function runs automatically every time
// SysTick counts down to zero
void SysTick_Handler(void) {
    tick_count++;  // increment counter every tick
}

// ─── Main ─────────────────────────────────────────
int main(void) {
    uart_init();
    uart_print("Starting SysTick interrupt demo\r\n");

    // fire interrupt every 1,000,000 clock cycles
    systick_init(1000000);

    uint32_t last_tick = 0;

    while(1) {
        // main loop does NOT poll hardware
        // it just checks if ISR updated tick_count
        if (tick_count != last_tick) {
            last_tick = tick_count;
            uart_print("Tick: ");
            uart_print_num(tick_count);
            uart_print("\r\n");
        }
    }

    return 0;
}