# Project 1 — Bare-Metal ARM Blink on QEMU

## What This Is
A bare-metal ARM Cortex-M3 firmware project built completely from scratch.
No HAL, no Arduino libraries, no operating system.
Runs on QEMU emulating an mps2-an385 ARM machine.

## What I Built
- `startup.c` — vector table and Reset_Handler (runs before main)
- `stm32f4.ld` — custom linker script defining Flash and SRAM memory layout
- `main.c` — bare-metal UART driver using direct register writes

## Memory Layout
| Region | Start Address | Size |
|--------|--------------|------|
| Flash  | 0x00000000   | 256KB |
| SRAM   | 0x20000000   | 256KB |

## How to Build
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o blink.elf
```

## How to Run on QEMU
```bash
qemu-system-arm -machine mps2-an385 -kernel blink.elf -nographic
```

## How to Debug with GDB
Terminal 1:
```bash
qemu-system-arm -machine mps2-an385 -kernel blink.elf -nographic -S -gdb tcp::3333
```
Terminal 2:
```bash
arm-none-eabi-gdb blink.elf
target remote localhost:3333
break main
continue
info registers
```

## Output
LED ON

LED OFF

LED ON

LED OFF

## Tools Used
- arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3)
- QEMU 11.0.0
- GDB 10.2

  ## Key Concepts Learned
- ARM Cortex-M3 boot sequence and vector table
- Memory-mapped peripheral register control
- Linker scripts and memory section placement
- Bare-metal UART driver implementation
- Live GDB debugging of firmware with no OS
