#include <stdint.h>

// ─── UART0 registers (mps2-an385) ─────────────────
#define UART0_BASE      0x40004000
#define UART0_DATA      (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_STATE     (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART0_CTRL      (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART0_INTSTATUS (*(volatile uint32_t *)(UART0_BASE + 0x0C))
#define UART0_BAUDDIV   (*(volatile uint32_t *)(UART0_BASE + 0x10))

// UART0 CTRL bits
#define UART_TX_EN      (1 << 0)
#define UART_RX_EN      (1 << 1)
#define UART_TX_INT_EN  (1 << 2)
#define UART_RX_INT_EN  (1 << 3)

// UART0 STATE bits
#define UART_TX_FULL    (1 << 0)
#define UART_RX_FULL    (1 << 1)

// UART0 INTSTATUS bits
#define UART_TX_INT     (1 << 0)
#define UART_RX_INT     (1 << 1)

// ─── NVIC registers ───────────────────────────────
#define NVIC_ISER0      (*(volatile uint32_t *)(0xE000E100))

// ─── Circular buffer ──────────────────────────────
#define BUFFER_SIZE 64

volatile char rx_buffer[BUFFER_SIZE];
volatile uint32_t rx_read  = 0;
volatile uint32_t rx_write = 0;

// check if buffer has data
uint32_t buffer_available(void) {
    return rx_write != rx_read;
}

// read one byte from buffer
char buffer_read(void) {
    char c = rx_buffer[rx_read];
    rx_read = (rx_read + 1) % BUFFER_SIZE;
    return c;
}

// write one byte to buffer (called from ISR)
void buffer_write(char c) {
    uint32_t next = (rx_write + 1) % BUFFER_SIZE;
    if (next != rx_read) {  // only write if not full
        rx_buffer[rx_write] = c;
        rx_write = next;
    }
}

// ─── UART functions ───────────────────────────────
void uart_init(void) {
    UART0_BAUDDIV = 16;
    // enable TX, RX, and RX interrupt
    UART0_CTRL = UART_TX_EN | UART_RX_EN | UART_RX_INT_EN;
}

void uart_putc(char c) {
    while (UART0_STATE & UART_TX_FULL);
    UART0_DATA = c;
}

void uart_print(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

// ─── UART ISR ─────────────────────────────────────
// fires automatically when a byte arrives on UART RX
void UART0_Handler(void) {
    // check if RX interrupt fired
    if (UART0_INTSTATUS & UART_RX_INT) {
        char received = UART0_DATA;    // read the byte
        buffer_write(received);         // store in circular buffer
        UART0_INTSTATUS = UART_RX_INT; // clear the interrupt flag
    }
}

// ─── Main ─────────────────────────────────────────
int main(void) {
    uart_init();

    // enable UART0 interrupt in NVIC (UART0 is IRQ 0 on mps2-an385)
    NVIC_ISER0 |= (1 << 0);

    uart_print("UART Interrupt Driver Ready\r\n");
    uart_print("Waiting for input...\r\n");

    while(1) {
        // main loop does other work here
        // ISR handles incoming bytes in background

        if (buffer_available()) {
            char c = buffer_read();

            // echo back what was received
            uart_print("Received: ");
            uart_putc(c);
            uart_print("\r\n");

            // detect end of message
            if (c == '\n' || c == '\r') {
                uart_print("--- message complete ---\r\n");
            }
        }
    }

    return 0;
}