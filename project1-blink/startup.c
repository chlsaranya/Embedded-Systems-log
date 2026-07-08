#include <stdint.h>

extern int main(void);

void Reset_Handler(void) {
    main();
    while(1);
}

__attribute__((section(".isr_vector")))
uint32_t vector_table[] = {
    0x20040000,              // top of SRAM (256KB from 0x20000000)
    (uint32_t)Reset_Handler  // reset handler address
};