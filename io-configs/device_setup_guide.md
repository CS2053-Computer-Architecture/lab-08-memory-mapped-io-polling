# Ripes Practical Setup & Device Configuration Guide

This guide details the practical environment setup, processor configuration, and peripheral instantiation for Lab 08 inside the Ripes simulator.

---

## 1. Processor & Simulator Optimization Settings

Ripes provides several processor architectures and graphical settings. For this lab, configure them as follows:

### Step 1: Select the Single-Cycle Processor
1. Go to the **Processor** tab in Ripes.
2. In the top toolbar, click the processor/chip icon (**Select Processor**).
3. Choose **RISC-V Single Cycle Processor** and confirm (Select / OK).
4. *Why this matters*: The Single Cycle model executes an instruction in one cycle without pipeline fill, hazard stalls, or branch flush penalties, making visual I/O updates significantly faster and easier to observe. It does not mean entering a number "1" into a cycle box.

### Step 2: Lower Cache Plot Cycles
1. Open the settings via **Edit -> Settings** (or the gear icon depending on the Ripes version).
2. Locate **Max. cache plot cycles**.
3. Change its value to `1`.
4. *Why this matters*: This is a GUI performance setting. It restricts how many past cycles of memory transactions Ripes keeps in memory to draw the cache visualization, reducing host CPU load during simulation.

### Step 3: Understanding the Execution Modes
Ripes provides three execution buttons on the main toolbar:
- **Clock / Step**: Advances the processor by one clock cycle per click. Use this to trace the exact assembly instructions (`lw` or `sw`) performing MMIO.
- **Auto-clock**: Continuously steps the clock at a configurable visual rate so you can watch registers and LEDs change in real time.
- **Run / Fast run**: Runs the program at maximum host speed without continuously repainting the datapath canvas on every cycle. Use this when testing the interactive traffic light or D-Pad navigation.

---

## 2. Part 1 Demonstration Setup (First Ripes Demo)

Before running the exercises, verify basic MMIO operation using the built-in example:
1. Open the example via **File -> Load Example -> C -> leds.c**.
2. Click the **I/O** tab icon on the left sidebar.
3. Under the **Devices** list, double-click **LED Matrix**.
4. Check the **Exported Symbols** pane. Note the generated constants:
   - `LED_MATRIX_0_BASE` (e.g. `0xf0000000`)
   - `LED_MATRIX_0_SIZE`
   - `LED_MATRIX_0_WIDTH`
   - `LED_MATRIX_0_HEIGHT`
5. Compile and run using Step or Auto-clock to observe the initial pixel lighting.

---

## 3. Exercise 1: Traffic Light Hardware Setup

1. In the **I/O** tab:
   - Remove any previous devices if necessary.
   - Double-click **LED Matrix**:
     - **Height** = `3`
     - **Width** = `1`
     - **LED size** = `24`
   - Double-click **Switches**:
     - Number of switches = `1` (or keep default 8; bit 0 will be read).
2. Verify the Exported Symbols table displays:
   - `LED_MATRIX_0_BASE`
   - `SWITCHES_0_BASE`
3. In the **Editor** tab (C mode):
   - Open or paste `src/exercise1_traffic_light.c`.
   - Compile and execute using **Run**.
4. Test by toggling Switch 0 in the I/O panel.

---

## 4. Exercise 2: D-Pad LED Navigation Hardware Setup

1. In the **I/O** tab:
   - Double-click **LED Matrix**:
     - **Height** = `5`
     - **Width** = `5` (a 5x5 grid provides ample room for 2D movement)
     - **LED size** = `24`
   - Double-click **D-Pad**.
2. Verify the Exported Symbols table displays:
   - `LED_MATRIX_0_BASE`
   - `D_PAD_0_BASE`
   - `D_PAD_0_UP`, `D_PAD_0_DOWN`, `D_PAD_0_LEFT`, `D_PAD_0_RIGHT`
   *(Always verify these symbol names against your Ripes build before compiling)*.
3. In the **Editor** tab (C mode):
   - Open or paste `src/exercise2_dpad_led.c`.
   - Compile and execute using **Run**.
4. Click directional buttons on the D-Pad widget to move the red LED across the grid.
