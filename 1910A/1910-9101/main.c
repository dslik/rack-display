// ---------------------------------------------------------------------------------
// 1910A 1U LED Sign Demo
// ---------------------------------------------------------------------------------
// Shows how to display an image on the 1910A LED sign
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// System Headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Pico Headers
#include "pico/stdlib.h"

// Local Headers
#include "ws2812.h"
#include "image.h"

// Local Constants
#define SIGN_LEFT_WS2812    4  // Left side of the sign
#define SIGN_RIGHT_WS2812   3  // Right side of the sign

// Local Functions
int main() {
    uint16_t    counter = 0;
    uint16_t    width = 100;
    uint16_t    height = 10;

    stdio_init_all();

    // Get first free state machine in PIO 1
    uint sm = pio_claim_unused_sm(pio1, true);
    uint offset = pio_add_program(pio1, &ws2812_program);

    ws2812_program_init(pio1, sm, offset, SIGN_RIGHT_WS2812, 1200000, false);

    while(counter < (width / 2) * height * 3)
    {
        put_pixel(urgb_u32((image[counter]) / 20,
                           (image[counter + 1]) / 20,
                           (image[counter + 2]) / 20));
        counter = counter + 3;
    }

    sleep_us(50);   // Wait for the PIO to finish writing out data

    ws2812_program_init(pio1, sm, offset, SIGN_LEFT_WS2812, 1200000, false);

    while(counter < (width) * height * 3)
    {
        put_pixel(urgb_u32((image[counter]) / 20,
                           (image[counter + 1]) / 20,
                           (image[counter + 2]) / 20));
        counter = counter + 3;
    }

    sleep_us(50);   // Wait for the PIO to finish writing out data

    while (true)
    {
        sleep_ms(500);
    }
}
