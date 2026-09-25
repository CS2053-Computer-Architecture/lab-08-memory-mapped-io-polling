#include "ripes_system.h"

/*
 * Memory-Mapped I/O Base Addresses
 * led_base: Pointer to LED matrix MMIO address (output)
 * switch_base: Pointer to switch MMIO address (input)
 *
 * Hardware configuration:
 * LED Matrix: Height = 3, Width = 1
 * Switches: 1 switch
 */
unsigned* led_base = LED_MATRIX_0_BASE;
unsigned* switch_base = SWITCHES_0_BASE;

void main() {
    unsigned state = 0;
    unsigned count = 1;

    while (1) {
        /*
         * Read Switch 0 using bitmasking:
         * *switch_base reads the 32-bit MMIO input register.
         * Masking with 0x1 isolates bit 0.
         */
        if ((*switch_base & 0x1) == 1) {
            /* State change occurs every 10 loop iterations */
            if (count % 10 == 0) {
                if (state == 2) {
                    state = 0;
                } else {
                    state++;
                }
            }

            if (state == 0) {
                /* GO state: Lower LED (Row 2) is Green, others Black */
                *(led_base + 0) = 0x0;
                *(led_base + 1) = 0x0;
                *(led_base + 2) = 0xFF << 8;
            } else if (state == 1) {
                /* GET READY state: Middle LED (Row 1) is Orange, others Black */
                *(led_base + 0) = 0x0;
                *(led_base + 1) = (0xFF << 16) | (0xFF << 8);
                *(led_base + 2) = 0x0;
            } else {
                /* STOP state: Upper LED (Row 0) is Red, others Black */
                *(led_base + 0) = 0xFF << 16;
                *(led_base + 1) = 0x0;
                *(led_base + 2) = 0x0;
            }

            count++;
        } else {
            /* Switch 0 is OFF: Turn off all LEDs */
            *(led_base + 0) = 0x0;
            *(led_base + 1) = 0x0;
            *(led_base + 2) = 0x0;
        }
    }
}
