# Embedded Systems Learning Log

Bare-metal ARM firmware built from scratch.
No HAL. No Arduino. No operating system.
Tools: arm-none-eabi-gcc, QEMU 11.0.0, GDB 10.2

## Projects

### Project 1 — Bare-Metal ARM Blink + UART + GDB Debugging
Folder: `project1-blink/`
- Custom linker script
- Vector table and Reset_Handler
- Bare-metal UART driver using direct register writes
- Live GDB debugging

Build:
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o blink.elf
```
Run:
```bash
qemu-system-arm -machine mps2-an385 -kernel blink.elf -nographic
```

### Project 2 — SysTick Timer Interrupt + NVIC
Folder: `project2-interrupts/`
- SysTick timer interrupt from scratch
- Full 16-entry vector table
- volatile shared variable between ISR and main
- GDB breakpoint inside live ISR

Build:
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o interrupts.elf
```
Run:
```bash
qemu-system-arm -machine mps2-an385 -kernel interrupts.elf -nographic
```