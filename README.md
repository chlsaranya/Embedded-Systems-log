# Embedded Systems Learning Log

Bare-metal ARM firmware built completely from scratch — no HAL, no Arduino, no operating system.
Every project runs on a virtual ARM Cortex-M3 using QEMU simulation without any physical hardware.

**Tools Used:**
- arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3)
- QEMU 11.0.0 (ARM Cortex-M3 emulation)
- GDB 10.2 (remote debugging)
- VS Code

---

## Project 1 — Bare-Metal ARM + UART + GDB Debugging
**Folder:** `project1-blink/`

### What This Is
The foundation of everything. Built a bare-metal ARM program from scratch using only three files — no libraries, no HAL, no OS. Direct register-level control of hardware.

### What I Built
- **Linker script** (`stm32f4.ld`) — defines exact memory layout. Flash at `0x00000000` (256KB), SRAM at `0x20000000` (256KB). Tells the compiler where to place `.text`, `.data`, and `.bss` sections.
- **Startup file** (`startup.c`) — vector table sitting at address `0x00000000`. First entry sets the stack pointer, second entry jumps to `Reset_Handler`. This is what runs before `main()` — most tutorials never show this.
- **UART driver** (`main.c`) — bare-metal UART communication using direct writes to memory-mapped registers at `0x40004000`. No library calls anywhere.

### Key Concepts
- ARM Cortex-M3 boot sequence — CPU reads vector table at power on, loads stack pointer, jumps to Reset_Handler
- Memory-mapped I/O — every peripheral is just a memory address
- volatile keyword — prevents compiler from caching hardware register values
- Cross compilation — arm-none-eabi-gcc running on Windows producing ARM machine code

### How to Build
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o blink.elf
```

### How to Run
```bash
qemu-system-arm -machine mps2-an385 -kernel blink.elf -nographic
```

### How to Debug with GDB
```bash
# Terminal 1 — start QEMU paused
qemu-system-arm -machine mps2-an385 -kernel blink.elf -nographic -S -gdb tcp::3333

# Terminal 2 — connect GDB
arm-none-eabi-gdb blink.elf
(gdb) file blink.elf
(gdb) target remote localhost:3333
(gdb) break main
(gdb) continue
(gdb) info registers
```

### Output
LED ON
LED OFF
LED ON
LED OFF

### GDB Register Output
pc  = 0xb6 <main+8>   → CPU executing 8 bytes into main()
sp  = 0x2003fff0      → stack pointer, matches vector table exactly
r3  = 0x40004008      → CPU loaded UART control register address
xpsr= 0x01000000      → Thumb mode, Thread mode, privileged access
---

## Project 2 — SysTick Timer Interrupt + NVIC
**Folder:** `project2-interrupts/`

### What This Is
First interrupt-driven project. Instead of polling hardware, the CPU responds to timer events automatically through the NVIC and vector table.

### What I Built
- **Full 16-entry vector table** — all ARM Cortex-M exceptions mapped. Position 15 is SysTick_Handler — the CPU jumps here automatically when the timer fires, no function call needed.
- **SysTick driver** — configured the built-in ARM timer to fire every 1,000,000 clock cycles by writing directly to SysTick registers at `0xE000E010`.
- **ISR** (`SysTick_Handler`) — increments a volatile counter every interrupt. Runs completely automatically, triggered by hardware.
- **Non-blocking main loop** — main() never checks the timer. It just reads the counter when it changes.

### Key Concepts
- Interrupts vs polling — CPU responds to hardware events automatically instead of constantly checking
- NVIC (Nested Vectored Interrupt Controller) — ARM hardware block that manages all interrupts and their priorities
- volatile shared variable — tick_count written in ISR, read in main(). Without volatile the compiler caches the value and main() never sees it update
- Vector table position 15 — SysTick is a CPU internal exception, no NVIC enable needed

### How to Build
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o interrupts.elf
```

### How to Run
```bash
qemu-system-arm -machine mps2-an385 -kernel interrupts.elf -nographic
```

### GDB — Catching the Interrupt Live
```bash
# Terminal 1
qemu-system-arm -machine mps2-an385 -kernel interrupts.elf -nographic -S -gdb tcp::3333

# Terminal 2
arm-none-eabi-gdb interrupts.elf
(gdb) file interrupts.elf
(gdb) target remote localhost:3333
(gdb) break SysTick_Handler
(gdb) continue
(gdb) print tick_count   # watch it increment by 1 each interrupt
(gdb) continue
(gdb) print tick_count
```

### Output
Starting SysTick interrupt demo
Tick: 1
Tick: 2
Tick: 3
---

## Project 3 — UART Interrupt Driver + Circular Buffer
**Folder:** `project3-uart-interrupt/`

### What This Is
A complete non-blocking UART receive driver — the standard pattern used in real production firmware. Incoming bytes are handled by an ISR and stored in a circular buffer without blocking the CPU.

### What I Built
- **UART RX interrupt** — enabled UART receive interrupt via UART0_CTRL register. ISR fires automatically when a byte arrives on the virtual serial port.
- **Circular buffer** — fixed size 64-byte ring buffer. ISR writes incoming bytes at the write pointer, main() reads from the read pointer. Pointers wrap around using modulo arithmetic. Standard pattern in every production UART driver.
- **NVIC setup** — UART0 is peripheral IRQ0 on mps2-an385, enabled via `NVIC_ISER0 |= (1 << 0)`. Different from SysTick which is a CPU exception — peripheral IRQs must be enabled manually.
- **Interrupt flag clearing** — after reading the byte, interrupt flag cleared via UART0_INTSTATUS. Forgetting this causes the interrupt to re-trigger immediately in an infinite loop.

### Key Concepts
- Peripheral IRQ vs CPU exception — UART0 at vector position 16 requires manual NVIC enable, unlike SysTick at position 15
- Circular ring buffer — non-blocking data storage between ISR and main(), handles bursts of incoming data
- Interrupt flag clearing — mandatory after servicing, prevents infinite re-triggering
- Non-blocking architecture — ISR and main() run independently, CPU never waits for data

### How to Build
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -g -T stm32f4.ld startup.c main.c -o uart_interrupt.elf
```

### How to Run
```bash
qemu-system-arm -machine mps2-an385 -kernel uart_interrupt.elf -nographic
```
Type characters in the terminal — each keystroke triggers a UART RX interrupt.

### Output
UART Interrupt Driver Ready
Waiting for input...
Received: h
Received: i
Received: !
--- message complete ---
