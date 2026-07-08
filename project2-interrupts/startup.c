#include <stdint.h>

extern int main(void);

// declare ISR so vector table can reference it
extern void SysTick_Handler(void);

void Reset_Handler(void) {
    main();
    while(1);
}

// default handler for unused interrupts
void Default_Handler(void) {
    while(1);
}

// ─── Vector Table ─────────────────────────────────
// ARM Cortex-M vector table layout:
// position 0  → initial stack pointer
// position 1  → Reset_Handler
// position 2  → NMI_Handler
// position 3  → HardFault_Handler
// position 4  → MemManage_Handler
// position 5  → BusFault_Handler
// position 6  → UsageFault_Handler
// position 7-10 → reserved
// position 11 → SVC_Handler
// position 12 → DebugMon_Handler
// position 13 → reserved
// position 14 → PendSV_Handler
// position 15 → SysTick_Handler  ← this is what we're using

__attribute__((section(".isr_vector")))
uint32_t vector_table[] = {
    0x20040000,                  // initial stack pointer
    (uint32_t)Reset_Handler,     // position 1
    (uint32_t)Default_Handler,   // position 2  NMI
    (uint32_t)Default_Handler,   // position 3  HardFault
    (uint32_t)Default_Handler,   // position 4  MemManage
    (uint32_t)Default_Handler,   // position 5  BusFault
    (uint32_t)Default_Handler,   // position 6  UsageFault
    0,                           // position 7  reserved
    0,                           // position 8  reserved
    0,                           // position 9  reserved
    0,                           // position 10 reserved
    (uint32_t)Default_Handler,   // position 11 SVC
    (uint32_t)Default_Handler,   // position 12 DebugMon
    0,                           // position 13 reserved
    (uint32_t)Default_Handler,   // position 14 PendSV
    (uint32_t)SysTick_Handler,   // position 15 SysTick ← our handler
};