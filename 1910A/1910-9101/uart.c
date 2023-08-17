// ---------------------------------------------------------------------------------
// Serial Command-line Utilities
// ---------------------------------------------------------------------------------
// Provides a simple command line interface over a UART
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// Standard Library Includes
#include <stdio.h>
#include <stdlib.h>

// Target Includes
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// Project Includes
#include "uart.h"

// Constants
#define COMMAND_STRING_MAX_LENGTH    32

// Globals
char collect_string[] = "00000000000000000000000000000000";
char command_string[] = "00000000000000000000000000000000";
uint8_t collect_string_pos = 0;
uint8_t volatile collect_active = 0;


// Private prototypes
void uart_rx_isr(void);

// =================================================================================

// Set up the UART
void uart_setup(void)
{
    uart_init(uart1, 9600);
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);

    // Disable flow control on the UART
    uart_set_hw_flow(uart1, false, false);

    // Disable FIFOs on the UART
    uart_set_fifo_enabled(uart1, false);

    collect_string[0] = 0;
    command_string[0] = 0;
    collect_active = 0;
    collect_string_pos = 0;

    // Set up UART interrupts
    irq_set_exclusive_handler(UART1_IRQ, uart_rx_isr);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(uart1, true, false);

    // Required to prevent the first written character from being lsot
    sleep_ms(10);
}

void uart_display_prompt(void)
{
    uart_puts(uart1, "\r\n1910A > ");    
}

// UART Interrupt Handler
void uart_rx_isr(void)
{
    uint8_t character = 0;
    uint8_t copy_counter = 0;

    while (uart_is_readable(uart1))
    {
        character = uart_getc(uart1);

        // Handle CR
        if(character == 0x0D)
        {
            if(collect_string_pos == 0)
            {
                uart_display_prompt();
            }
            else
            {
                copy_counter = 0;

                // Copy over to the command string
                while(collect_string[copy_counter] != 0)
                {
                    command_string[copy_counter] = collect_string[copy_counter];
                    copy_counter = copy_counter + 1;
                }

                command_string[copy_counter] = 0;

                // Clear the collections string
                collect_string_pos = 0;
                collect_string[0] = 0;
            }
        }
        else
        {
            if((character >= 0x20) && (character <= 0x7E))
            {
                if((collect_string_pos == 0) && (collect_active == 0))
                {
                    collect_active = 1;
                }
            }
        }

        // Only add printable characters to the command string
        if((character >= 0x20) && (character <= 0x7E))
        {
            if(collect_string_pos < COMMAND_STRING_MAX_LENGTH)
            {
                collect_string[collect_string_pos] = character;
                collect_string[collect_string_pos + 1] = 0;
                uart_putc(uart1, character);

                collect_string_pos = collect_string_pos + 1;
            }
        }

        // Handle backspace
        if(character == 0x08)
        {
            if(collect_string_pos != 0)
            {
                collect_string_pos = collect_string_pos - 1;
                collect_string[collect_string_pos] = 0;
                
                uart_puts(uart1, "\033[1D \033[1D");
            }
        }
    }
}

const char* uart_command_get(void)
{
    return((const char*) command_string);
}

void uart_command_clear(void)
{
    command_string[0] = 0;
    collect_active = 1;
    uart_display_prompt();
}