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
#include <string.h>
#include <math.h>

// Pico Headers
#include "pico/stdlib.h"

// Local Headers
#include "ws2812.h"
#include "image.h"
#include "uart.h"

// Local Constants
#define SIGN_DEBUG_WS2812   11 // On controller

// =================================================================================

// Local Functions
int main() {
    uint16_t    pixel_counter = 0;
    uint16_t    x_counter = 0;
    uint16_t    y_counter = 0;
    uint16_t    counter = 0;
    uint16_t    width = 100;
    uint16_t    height = 10;

    stdio_init_all();

    printf("\n\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Protonema 1910 LED Rack Display Test, Firmware 0.1.0-alpha.1\n");
    printf("CERN-OHL-S v2 (https://github.com/dslik/rack-display/blob/main/license.md)\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Enable debug LED...\n");

    // Get first free state machine in PIO 1
    display_sm = pio_claim_unused_sm(pio1, true);
    display_sm_offset = pio_add_program(pio1, &ws2812_program);

    // Set the debug LED to purple
    ws2812_program_init(pio1, display_sm, display_sm_offset, SIGN_DEBUG_WS2812, 1200000, false);
    put_pixel(urgb_u32(10, 0, 10));

    // Wait for the PIO to finish writing out data
    sleep_ms(1);

    // ===========================================================================================
    printf("Display startup image...\n");

    // Copy the image into the frame buffer
    while(x_counter < width)
    {
        while(y_counter < height)
        {
            fb_set_grb(x_counter, y_counter, urgb_u32((image[pixel_counter]) / 20,
                                                      (image[pixel_counter + 1]) / 20,
                                                      (image[pixel_counter + 2]) / 20));

            pixel_counter = pixel_counter + 3;
            y_counter = y_counter + 1;
        }

        y_counter = 0;
        x_counter = x_counter + 1;
    }

    printf("Initializing Serial I/O...\n");
    uart_setup();

    while (true)
    {
        // Check if there are any commands pending
        const char* command = uart_command_get();
        if(strcmp(command, "") != 0)
        {
            if(strcmp(command, "help") == 0)
            {
                uart_puts(uart1, "\r\nCommands:");
                uart_puts(uart1, "\r\n\"help\"           - Displays list of commands");
                uart_puts(uart1, "\r\n\"clear\"          - Clear the serial terminal");
                uart_puts(uart1, "\r\n\"display\"        - Refresh the display");
                uart_puts(uart1, "\r\n\"display clear\"  - Set the entire frame buffer to black");
                uart_puts(uart1, "\r\n\"set x,y to r g b\"  - Set a pixel at x,y to r g b");
                uart_command_clear();
            }
            else if(strcmp(command, "clear") == 0)
            {
                uart_puts(uart1, "\033[2J");
                uart_command_clear();
            }
            else if(strcmp(command, "display") == 0)
            {
                uart_puts(uart1, "\r\nRefreshing display...");
                fb_display();
                uart_puts(uart1, "\r\nDone.");
                uart_command_clear();
            }
            else if(strncmp(command, "set ", 4) == 0)
            {
                int x = 0;
                int y = 0;
                int r = 0;
                int g = 0;
                int b = 0;

                sscanf(command, "set %i,%i to %i %i %i", &x, &y, &r, &g, &b);
                uart_puts(uart1, "\r\nSetting pixel...");
                printf("\r\nSetting pixel at %i, %i to R: %i, G: %i, B: %i", x, y, r, g, b);
                fb_set_grb(x, y, urgb_u32(r, g, b));
                uart_command_clear();
            }
            else if(strcmp(command, "display clear") == 0)
            {
                uart_puts(uart1, "\r\nClearing display...");
                fb_display_clear();
                uart_puts(uart1, "\r\nDone.");
                uart_command_clear();
            }
            else
            {
                uart_puts(uart1, "\r\nUnknown command\r\n");
                uart_command_clear();
            }
        }

        fb_display();
        sleep_ms(100);
    }
}
