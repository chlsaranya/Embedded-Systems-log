#include <stdint.h>

extern int main(void);
extern void UART0_Handler(void);
extern void SysTick_Handler(void);

void Reset_Handler(void) {
    main();
    while(1);
}

void Default_Handler(void) {
    while(1);
}

// ARM Cortex-M vector table
// positions 0-15  → CPU exceptions
// positions 16+   → peripheral interrupts (chip specific)
__attribute__((section(".isr_vector")))
uint32_t vector_table[] = {
    0x20040000,                  // 0  initial stack pointer
    (uint32_t)Reset_Handler,     // 1  reset
    (uint32_t)Default_Handler,   // 2  NMI
    (uint32_t)Default_Handler,   // 3  HardFault
    (uint32_t)Default_Handler,   // 4  MemManage
    (uint32_t)Default_Handler,   // 5  BusFault
    (uint32_t)Default_Handler,   // 6  UsageFault
    0,                           // 7  reserved
    0,                           // 8  reserved
    0,                           // 9  reserved
    0,                           // 10 reserved
    (uint32_t)Default_Handler,   // 11 SVCall
    (uint32_t)Default_Handler,   // 12 DebugMon
    0,                           // 13 reserved
    (uint32_t)Default_Handler,   // 14 PendSV
    (uint32_t)Default_Handler,   // 15 SysTick
    (uint32_t)UART0_Handler,     // 16 UART0 ← peripheral interrupt
};