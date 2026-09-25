#include "ripes_system.h"

#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT

/*
 * Companion implementation for Exercise 2 with Boundary Checking.
 *
 * The standard lab code uses unconstrained pointer increments, which wrap across
 * rows or write beyond the matrix memory if a border button is pressed.
 * This version maintains (x, y) coordinates to enforce hard boundaries.
 */

unsigned* led_base   = LED_MATRIX_0_BASE;
unsigned* d_pad_right = D_PAD_0_RIGHT;
unsigned* d_pad_down  = D_PAD_0_DOWN;
unsigned* d_pad_left  = D_PAD_0_LEFT;
unsigned* d_pad_up    = D_PAD_0_UP;

void main() {
    unsigned x = 0;
    unsigned y = 0;

    unsigned* led_current = led_base;
    *(led_current) = 0xFF << 16;

    while (1) {
        if (((*d_pad_right & 0x1) == 1) && (x < W - 1)) {
            *(led_current) = 0;
            x++;
            led_current++;
            *(led_current) = 0xFF << 16;
        } else if (((*d_pad_down & 0x1) == 1) && (y < H - 1)) {
            *(led_current) = 0;
            y++;
            led_current += W;
            *(led_current) = 0xFF << 16;
        } else if (((*d_pad_left & 0x1) == 1) && (x > 0)) {
            *(led_current) = 0;
            x--;
            led_current--;
            *(led_current) = 0xFF << 16;
        } else if (((*d_pad_up & 0x1) == 1) && (y > 0)) {
            *(led_current) = 0;
            y--;
            led_current -= W;
            *(led_current) = 0xFF << 16;
        }
    }
}
