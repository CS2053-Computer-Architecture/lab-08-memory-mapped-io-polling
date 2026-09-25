#include "ripes_system.h"

#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT

/*
 * Memory-Mapped I/O Device Pointers:
 * led_current: points to the current active LED location
 * d_pad_*: point to the MMIO input registers for each directional button
 */
unsigned* led_current = LED_MATRIX_0_BASE;
unsigned* d_pad_right = D_PAD_0_RIGHT;
unsigned* d_pad_down  = D_PAD_0_DOWN;
unsigned* d_pad_left  = D_PAD_0_LEFT;
unsigned* d_pad_up    = D_PAD_0_UP;

void main() {
    /* Set initial starting LED to Red */
    *(led_current) = 0xFF << 16;

    while (1) {
        if ((*d_pad_right & 0x1) == 1) {
            /* Move Right: clear current LED, advance pointer by +1 */
            *(led_current) = 0;
            *(led_current + 1) = 0xFF << 16;
            led_current++;
        } else if ((*d_pad_down & 0x1) == 1) {
            /* Move Down: clear current LED, advance pointer by +W (next row) */
            *(led_current) = 0;
            *(led_current + W) = 0xFF << 16;
            led_current += W;
        } else if ((*d_pad_left & 0x1) == 1) {
            /* Move Left: clear current LED, advance pointer by -1 */
            *(led_current) = 0;
            *(led_current - 1) = 0xFF << 16;
            led_current--;
        } else if ((*d_pad_up & 0x1) == 1) {
            /* Move Up: clear current LED, advance pointer by -W (previous row) */
            *(led_current) = 0;
            *(led_current - W) = 0xFF << 16;
            led_current -= W;
        }
    }
}
