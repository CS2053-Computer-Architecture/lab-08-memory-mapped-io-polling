# CS2053 Lab 08: Memory-Mapped I/O & Polling — Practical Analysis & Implementation Notes

Course: CS2053 – Computer Architecture  
Department of Computer Science and Engineering, University of Moratuwa  

---

## Part 0: Architectural Foundations of Memory-Mapped I/O

The central premise of Memory-Mapped I/O (MMIO) is that peripheral hardware devices behave identically to memory locations from the perspective of the CPU:

```
CPU
 │
 │ standard LOAD / STORE instructions
 ▼
Memory Address Space
 ├── 0x00000000 - 0x10000000... : Standard RAM (Code, Data, Stack)
 ├── 0xF0000000                 : LED Matrix Output MMIO
 └── 0xF0000020+                : Switches / D-Pad Input MMIO
```

In MMIO:
- The CPU requires no special I/O instructions (unlike Port-Mapped I/O on x86 architectures with `IN`/`OUT`).
- Writing to a peripheral is accomplished with standard memory store instructions (`sw`, `sb`). In C:
  ```c
  *led_base = value; // Stores value to physical address 0xF0000000
  ```
- Reading from a peripheral is accomplished with standard memory load instructions (`lw`, `lb`). In C:
  ```c
  value = *switch_base; // Loads value from the switch register address
  ```
Because ordinary memory access operations are used, standard C pointers operate directly on Ripes I/O peripherals.

---

## Part 1: Exported Symbols in `ripes_system.h`

When a hardware device is instantiated in Ripes, the simulator dynamically generates a header file (`ripes_system.h`) containing architectural constants for physical memory boundaries and device parameters:
- `LED_MATRIX_0_BASE`: Base address where the first I/O device begins mapping in CPU address space (`0xf0000000`).
- `LED_MATRIX_0_SIZE`: Total address space occupied by the device in bytes.
- `LED_MATRIX_0_WIDTH` & `LED_MATRIX_0_HEIGHT`: Grid dimensions configured in the I/O tab.
- `SWITCHES_0_BASE`: Base address for the switches input register.
- `D_PAD_0_RIGHT`, `D_PAD_0_DOWN`, `D_PAD_0_LEFT`, `D_PAD_0_UP`: Individual directional input line addresses.

### Device Size Calculation Example
Suppose an LED Matrix is configured with:
```text
Height = 10
Width  = 5
```
The total number of LEDs is:
$$\text{Total LEDs} = 10 \times 5 = 50\text{ LEDs}$$
Because each RGB LED maps to a 32-bit register (4 bytes):
$$\text{Total Size} = 50 \times 4 = 200\text{ bytes}$$
Converting 200 decimal to hexadecimal:
$$200_{10} = \mathbf{0xC8}_{16}$$
Therefore, Ripes exports:
```c
#define LED_MATRIX_0_BASE (0xf0000000)
#define LED_MATRIX_0_SIZE (0xc8)
```

### Critical Distinction: Hardware Exports vs. User Shortcuts
Ripes exports full descriptive names:
- `#define LED_MATRIX_0_WIDTH 5`
- `#define LED_MATRIX_0_HEIGHT 10`

Ripes does **not** automatically export shorthand identifiers like `#define W` or `#define H`. 
Instead, the user C program explicitly creates these aliases:
```c
#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT
```
`W` and `H` are program-level shortcuts, not direct exports from `ripes_system.h`.

### Memory Hook
```text
BASE         -> Where the device begins in memory
SIZE         -> How many bytes of address space it occupies
WIDTH/HEIGHT -> Automatic hardware exports from Ripes
W/H          -> User-defined shortcuts declared in C code
```

Including `#include "ripes_system.h"` and defining:
```c
unsigned* led_base = LED_MATRIX_0_BASE;
```
is conceptually equivalent to binding a pointer directly to physical memory:
```c
unsigned* led_base = (unsigned*) 0xF0000000;
```


---

## Part 2: 24-Bit RGB Colour Packing Mechanics

The LED matrix controller packs color components into a single 32-bit word:

```
Bits 31..24 | Bits 23..16 | Bits 15..8 | Bits 7..0
  Unused    |     Red     |   Green    |   Blue
 00000000   |   RRRRRRRR  |  GGGGGGGG  | BBBBBBBB
```

To encode individual color channels:
```c
*(led_base + idx) = (r << 16) | (g << 8) | b;
```
- `r << 16`: Shifts the 8-bit red component into bits 23..16.
- `g << 8`: Shifts the 8-bit green component into bits 15..8.
- `b`: Occupies the least significant bits 7..0.
- Bitwise OR (`|`) combines them into a single integer.

Examples:
- **Red**: `0xFF << 16` $\rightarrow$ `0x00FF0000`
- **Green**: `0xFF << 8` $\rightarrow$ `0x0000FF00`
- **Orange**: `(0xFF << 16) | (0xFF << 8)` $\rightarrow$ `0x00FFFF00`
- **Black (Off)**: `0x0`

---

## Part 3: Pointer Arithmetic and Word Addressing

On RV32 systems, an `unsigned int` is 4 bytes wide. Pointer arithmetic on `unsigned*` automatically scales by the underlying type size:

$$\text{Physical Address} = \text{Base} + (\text{idx} \times 4)$$

```
Physical Address | Pointer Expression | Target LED
0xF0000000       | led_base + 0       | LED 0 (Row 0)
0xF0000004       | led_base + 1       | LED 1 (Row 1)
0xF0000008       | led_base + 2       | LED 2 (Row 2)
```

Therefore, writing:
```c
*(led_base + 2) = 0xFF << 8;
```
emits a `sw` instruction that places green color data into byte offset 8 (`0xF0000008`), illuminating LED index 2.

---

## Part 4: Exercise 1 — Traffic Light Controller Implementation

### Hardware Specification
- LED Matrix: Height = 3, Width = 1
- Switches: 1 switch at `SWITCHES_0_BASE`

### Reading Switch 0 via Bitmasking
The switch peripheral presents its status as a word. To inspect only Switch 0, bit 0 must be isolated using a bitwise AND mask:
```c
if ((*switch_base & 0x1) == 1)
```
This isolates bit 0 and guards against unintended bits.

### Finite State Machine Flow
The controller cycles through three states:
- `State 0 (GO)`: Lower LED (index 2) Green, others Black.
- `State 1 (GET READY)`: Middle LED (index 1) Orange, others Black.
- `State 2 (STOP)`: Upper LED (index 0) Red, others Black.

State transitions occur when `count % 10 == 0`:
```c
if (state == 2) {
    state = 0;
} else {
    state++;
}
```

### Architectural Observation
When Switch 0 is turned OFF, all LEDs are set to black (`0x0`). However, the program maintains the internal values of `state` and `count`. When Switch 0 is switched back ON, execution resumes from the exact state and count it held prior to being turned off, rather than restarting at State 0.

---

## Part 5: Exercise 2 — D-Pad 2D Matrix Navigation

### Hardware Specification
- LED Matrix: Recommended $5 \times 5$ (or larger)
- D-Pad: Directional input registers (`D_PAD_0_RIGHT`, `D_PAD_0_DOWN`, `D_PAD_0_LEFT`, `D_PAD_0_UP`)

### Why Moving DOWN Uses `+W`
Matrix memory is arranged linearly in row-major order:
```
Row 0:  0   1   2   3   4
Row 1:  5   6   7   8   9
Row 2: 10  11  12  13  14
```
For a matrix of width $W = 5$:
- To move **Right**: next column $\rightarrow$ index increment $+1$.
- To move **Left**: previous column $\rightarrow$ index decrement $-1$.
- To move **Down**: same column in the next row requires skipping an entire row of $W$ LEDs $\rightarrow$ index increment $+W$.
- To move **Up**: same column in the previous row $\rightarrow$ index decrement $-W$.

### Directional Pointer Update Mechanics
When moving in any direction:
1. Clear the current LED: `*(led_current) = 0;`
2. Write the new LED: `*(led_current + offset) = 0xFF << 16;`
3. Advance the pointer: `led_current += offset;`

### Boundary Safety Analysis
The unconstrained pointer movement in the basic exercise does not check matrix borders:
- Pressing `RIGHT` on the rightmost column ($x = W - 1$) advances `led_current` into index $+1$, wrapping the LED into the first column of the next row.
- Pressing `UP` on the top row ($y = 0$) decrements the pointer before `LED_MATRIX_0_BASE`, writing to unmapped memory or corrupting preceding data.
- In practical use, boundaries must be guarded either by maintaining coordinate counters $(x, y)$ or checking address limits against `LED_MATRIX_0_BASE` and `LED_MATRIX_0_SIZE`.

---

## Part 6: I/O Strategies & Polling Latency Analysis

### Polling vs. Interrupt-Driven vs. DMA

1. **Programmed I/O (Polling)**:
   - The CPU repeatedly reads the peripheral status register in an infinite loop (`while (1)`).
   - Advantage: Extremely simple to implement with minimal hardware requirements.
   - Disadvantage: Wastes 100% of CPU instruction cycles and consumes significant power while waiting for input events.

2. **Interrupt-Driven I/O**:
   - The CPU initiates an operation or idles, and continues executing other useful instructions.
   - The peripheral hardware raises an external electrical interrupt line when data is ready.
   - The CPU suspends its current execution context, vectors to an Interrupt Service Routine (ISR), services the device, and restores context.
   - Advantage: Eliminates busy-waiting, maximizing CPU efficiency.

3. **Direct Memory Access (DMA)**:
   - A specialized hardware DMA controller transfers data directly between peripherals and main memory over the system bus without CPU involvement for individual bytes.
   - The CPU merely initializes the source address, destination address, and transfer size, and receives an interrupt when the block transfer completes.

### Why Input Latency Occurs in Polling
In the D-Pad navigation loop:
```c
while (1) {
    if ((*d_pad_right & 0x1) == 1) { ... }
    else if ((*d_pad_down & 0x1) == 1) { ... }
    else if ((*d_pad_left & 0x1) == 1) { ... }
    else if ((*d_pad_up & 0x1) == 1) { ... }
}
```
1. The hardware does not interrupt the CPU upon a button click.
2. The CPU discovers button state changes only when instruction execution reaches the corresponding `lw` from that specific MMIO address.
3. If the processor is executing other branches or instructions within the loop, the input is buffered in the hardware register until the CPU completes the iteration and re-reads the register.
4. In simulation, visual rendering overhead and lower simulated clock rates further amplify the visible delay between clicking the button in the UI and the LED changing state.
