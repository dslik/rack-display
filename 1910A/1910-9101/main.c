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
#include "hardware/rtc.h"

// Local Headers
#include "ws2812.h"
#include "image.h"
#include "uart.h"
#include "sha1.h"
#include "snon_utils.h"

// Local Constants
#define SIGN_DEBUG_WS2812       11
#define SNPRINTF_BUFFER_SIZE    80

// =================================================================================

// Local Functions
int main() {
    datetime_t  t;
    bool        get_time_valid = false;
    char        snprintf_buffer[SNPRINTF_BUFFER_SIZE];
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
    printf("Testing SHA-1: ");

    SHA1_CTX   sha1_context;
    uint8_t    sha1_result[20];

    SHA1Init(&sha1_context);
    SHA1Update(&sha1_context, "abc", 3);
    SHA1Final(&sha1_result, &sha1_context);

    while(counter != 20)
    {
        printf("%02X", sha1_result[counter]);

        counter = counter + 1;
    }
    printf("\n");

    entity_get_uuid("device", snprintf_buffer);
    printf("Device GUID:   %s\n", snprintf_buffer);

    // ===========================================================================================
    printf("Initializing SNON entities...\n");

    snon_initialize("1910A 1U Rack Display");

    // Add measurands
    entity_register("Time Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"iso8601\",\"meAq\":\"count\"}");
    entity_register("Seconds Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"numeric\",\"meAq\":\"count\",\"meUS\":{\"*\":\"s\"},\"meUSx\":{\"*\":\"seconds\"},\"meR\":\"1\",\"meAc\":\"1\"}");

    // Add sensors
    entity_register("Device Time", SNON_CLASS_VALUE, NULL);
    entity_add_relationship("Device Time", SNON_REL_MEASURAND, "Time Measurand");
    entity_add_relationship("Device Time", SNON_REL_CHILD_OF, "Device");


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
                uart_puts(uart1, "\nCommands:");
                uart_puts(uart1, "\n\"help\"            - Displays list of commands");
                uart_puts(uart1, "\n\"clear\"           - Clear the serial terminal");
                uart_puts(uart1, "\n\"cat <entity>\"    - Display the value of a registered entity");
                uart_puts(uart1, "\n\"get time\"        - Get the current time");
                uart_puts(uart1, "\n\"set time\"        - Set the current time");
                uart_puts(uart1, "\n\"display refresh\" - Refresh the display");
                uart_puts(uart1, "\n\"display clear\"   - Set the entire frame buffer to black");
                uart_puts(uart1, "\n\"display dump\"    - Dump the contents of the display to the terminal");
                uart_puts(uart1, "\n\"set pixel x,y to r g b\"   - Set a pixel at x,y to r g b");
                uart_command_clear();
            }
            else if(strcmp(command, "clear") == 0)
            {
                uart_puts(uart1, "\033[2J");
                uart_command_clear();
            }
            else if(strncmp(command, "cat ", 4) == 0)
            {
                char* json_output = entity_name_to_json((char*) &(command[4]));

                if(json_output == NULL)
                {
                    json_output = entity_uuid_to_json((char*) &(command[4]));
                }

                if(json_output != NULL)
                {
                    uart_puts(uart1, "\n");
                    uart_puts(uart1, json_output);
                    uart_puts(uart1, "\n");
                    free(json_output);
                }
                else
                {
                    uart_puts(uart1, "\nEntity not found\n");
                }

                uart_command_clear();
            }
            else if(strcmp(command, "get time") == 0)
            {
                get_time_valid = rtc_counter_to_iso8601(snprintf_buffer, time_us_64());

                if(get_time_valid == true)
                {
                    uart_puts(uart1, "\nThe time is: ");
                    uart_puts(uart1, snprintf_buffer);
                }
                else
                {
                    uart_puts(uart1, "\nRTC not running. Use the \"set time\" command to set the current time.");
                }
                uart_command_clear();
            }
            else if(strncmp(command, "set time ", 9) == 0)
            {
                unsigned int    year = 0;
                unsigned int    month = 0;
                unsigned int    day = 0;
                unsigned int    hours = 0;
                unsigned int    minutes = 0;
                unsigned int    seconds = 0;
                bool            set_valid = false;

                sscanf(command, "set time %4u-%2u-%2uT%2u:%2u:%2uZ", &year, &month, &day, &hours, &minutes, &seconds);
                if(year < 2022 || year > 2055)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid year %04d. Must be between 2022 and 2055", year);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(month < 1 || month > 12)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid month %02d. Must be between 1 and 12", month);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(day < 1 || day > 31)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid day %02d. Must be between 1 and 31", day);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(hours < 0 || hours > 23)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid hour %02d. Must be between 0 and 23", hours);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(minutes < 0 || minutes > 59)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid minute %02d. Must be between 0 and 59", minutes);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(seconds < 0 || seconds > 59)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\nInvalid second %02d. Must be between 0 and 59", hours);
                    uart_puts(uart1, snprintf_buffer);
                }
                else
                {
                    t.year = year;
                    t.month = month;
                    t.day = day;
                    t.hour = hours;
                    t.min = minutes;
                    t.sec = seconds;
                    t.dotw = 0;

                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "%04d-%02d-%02dT%02d:%02d:%02dZ", t.year, t.month, t.day, t.hour, t.min, t.sec);

                    set_valid = rtc_set_time(snprintf_buffer);
                    sleep_us(64);

                    if(set_valid == true)
                    {
                        uart_puts(uart1, "\nTime set to ");
                        uart_puts(uart1, snprintf_buffer);
                    }
                    else
                    {
                        uart_puts(uart1, "\nError setting time to ");
                        uart_puts(uart1, snprintf_buffer);
                    }
                }

                uart_command_clear();
            }
            else if(strcmp(command, "display refresh") == 0)
            {
                uart_puts(uart1, "\nRefreshing display...");
                fb_display();
                uart_puts(uart1, "\nDone.");
                uart_command_clear();
            }
            else if(strcmp(command, "display clear") == 0)
            {
                uart_puts(uart1, "\nClearing display...");
                fb_display_clear();
                uart_puts(uart1, "\nDone.");
                uart_command_clear();
            }
            else if(strcmp(command, "display dump") == 0)
            {
                get_time_valid = rtc_get_datetime(&t);

                if(get_time_valid == true)
                {
                    uart_puts(uart1, "\n{\"eID\" : \"542E71CE-8376-485E-B571-222F54A6A6D0\",");
                    uart_puts(uart1, "\n  \"eC\" : \"value\",");
                    uart_puts(uart1, "\n  \"eN\" : { \"en\" : \"Framebuffer contents\" },");
                    uart_puts(uart1, "\n  \"v\"  : [ \"");

                    uint16_t    x_counter = 0;
                    uint16_t    y_counter = 0;
                    uint32_t    grbValue = 0;
                    char        hex_value[7];

                    while(x_counter < FB_WIDTH)
                    {
                        while(y_counter < FB_HEIGHT)
                        {
                            grbValue = fb_get_grb(x_counter, y_counter);
                            snprintf(hex_value, 6, "%06X", grbValue);
                            uart_puts(uart1, hex_value);
                            
                            y_counter = y_counter + 1;
                        }

                        y_counter = 0;
                        x_counter = x_counter + 1;
                    }

                    uart_puts(uart1, "\" ],");
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\n  \"vT\" : [ \"%04d-%02d-%02dT%02d:%02d:%02d.%06lluZ\" ]", t.year, t.month, t.day, t.hour, t.min, t.sec, (time_us_64() % 1000000));
                    uart_puts(uart1, snprintf_buffer);
                    uart_puts(uart1, "\n}");
                }
                else
                {
                    uart_puts(uart1, "\nRTC not running. Use the \"set time\" command to set the current time.");
                }

                uart_command_clear();
            }
            else if(strncmp(command, "set pixel ", 10) == 0)
            {
                int x = 0;
                int y = 0;
                int r = 0;
                int g = 0;
                int b = 0;

                sscanf(command, "set pixel %i,%i to %i %i %i", &x, &y, &r, &g, &b);
                uart_puts(uart1, "\nSetting pixel...");
                printf("\nSetting pixel at %i, %i to R: %i, G: %i, B: %i", x, y, r, g, b);
                fb_set_grb(x, y, urgb_u32(r, g, b));
                uart_command_clear();
            }
            else
            {
                uart_puts(uart1, "\nUnknown command\n");
                uart_command_clear();
            }
        }

        fb_display();
        sleep_ms(100);
    }
}
