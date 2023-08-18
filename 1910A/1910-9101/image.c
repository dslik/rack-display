// ---------------------------------------------------------------------------------
// Display Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist in programming the 1910A display.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "hardware/sync.h"

#include "image.h"
#include "ws2812.h"

// Local Globals
uint8_t 	buf1[FB_WIDTH * FB_HEIGHT * 3 * sizeof(uint8_t)];
uint8_t		buf2[FB_WIDTH * FB_HEIGHT * 3 * sizeof(uint8_t)];
uint8_t*	current_buffer = buf1;
int 		display_sm;
int 		display_sm_offset;

// =================================================================================

uint32_t fb_get_grb(uint8_t x, uint8_t y)
{
	// Bounds checking
	if(x > 99) x = 99;
	if(y > 9) y = 9;

	return urgb_u32(current_buffer[0 + (x * 3) + (y * 3 * FB_WIDTH)],
		   		    current_buffer[1 + (x * 3) + (y * 3 * FB_WIDTH)],
		     		current_buffer[2 + (x * 3) + (y * 3 * FB_WIDTH)]);
}

void fb_set_grb(uint8_t x, uint8_t y, uint32_t grb_colour)
{
	// Bounds checking
	if(x > 99) x = 99;
	if(y > 9) y = 9;

	current_buffer[0 + (x * 3) + (y * 3 * FB_WIDTH)] = (grb_colour >> 8) & 0xFF;
	current_buffer[1 + (x * 3) + (y * 3 * FB_WIDTH)] = (grb_colour >> 16 ) & 0xFF;
	current_buffer[2 + (x * 3) + (y * 3 * FB_WIDTH)] = grb_colour & 0xFF;
}

void fb_display(void)
{
    uint16_t    counter = 0;
    uint16_t    x_counter = 0;
    uint16_t    y_counter = 0;
    uint16_t    size = FB_WIDTH * FB_HEIGHT * 3;
	uint8_t*	display_buffer = current_buffer;

	// swap buffers
	if(current_buffer == buf1)
	{
		current_buffer = buf2;
	}
	else
	{
		current_buffer = buf1;
	}

	// Copy old buffer into new buffer
    while(counter < size)
    {
        current_buffer[counter] = display_buffer[counter];
        counter = counter + 1;
    }

    uint32_t interrupts = save_and_disable_interrupts();

	// Populate the right side of the display
    ws2812_program_init(pio1, display_sm, display_sm_offset, SIGN_RIGHT_WS2812, 1200000, false);

    while(x_counter < FB_WIDTH / 2)
    {
        while(y_counter < FB_HEIGHT)
        {
            put_pixel(fb_get_grb(x_counter, y_counter));
    		
            y_counter = y_counter + 1;
        }

        y_counter = 0;
        x_counter = x_counter + 1;
    }

    restore_interrupts(interrupts);

    // Wait for the PIO to finish writing out data
    sleep_ms(1);

    interrupts = save_and_disable_interrupts();

    // Populate the left side of the display
    ws2812_program_init(pio1, display_sm, display_sm_offset, SIGN_LEFT_WS2812, 1200000, false);

    while(x_counter < FB_WIDTH)
    {
        while(y_counter < FB_HEIGHT)
        {
            put_pixel(fb_get_grb(x_counter, y_counter));

            y_counter = y_counter + 1;
        }

        y_counter = 0;
        x_counter = x_counter + 1;
    }

    restore_interrupts(interrupts);

    // Wait for the PIO to finish writing out data
    sleep_ms(1);    
}

void fb_display_clear(void)
{
    uint16_t    x_counter = 0;
    uint16_t    y_counter = 0;

    while(x_counter < FB_WIDTH)
    {
        while(y_counter < FB_HEIGHT)
        {
            fb_set_grb(x_counter, y_counter, 0);
    		
            y_counter = y_counter + 1;
        }

        y_counter = 0;
        x_counter = x_counter + 1;
    }
}
