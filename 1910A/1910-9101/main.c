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
#include "hardware/adc.h"

// Local Headers
#include "ws2812.h"
#include "image.h"
#include "fonts.h"
#include "uart.h"
#include "sha1.h"
#include "snon_utils.h"

// Local Constants
#define SIGN_DEBUG_WS2812       11
#define SNPRINTF_BUFFER_SIZE    80

const double conversion_factor = 3.3f / (1 << 12);

// =================================================================================
// Local Functions

float rp2040_get_temp(void)
{
    float reading = adc_read() * conversion_factor;
    reading = 27 - (reading - 0.706) / 0.001721;

    return(reading);
}

static bool sensor_update(struct repeating_timer *t)
{
    char        snprintf_buffer[SNPRINTF_BUFFER_SIZE];

    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "[\"%4.2f\"]", rp2040_get_temp());
    entity_name_update("Device Temp", snprintf_buffer);
}

int main() {
    struct repeating_timer  sensor_timer;
    datetime_t  t;
    bool        get_time_valid = false;
    char        snprintf_buffer[SNPRINTF_BUFFER_SIZE];
    uint16_t    pixel_counter = 0;
    uint16_t    x_counter = 0;
    uint16_t    y_counter = 0;
    uint16_t    counter = 0;
    uint16_t    width = 100;
    uint16_t    height = 10;
    char*       json_output = NULL;

    stdio_init_all();

    printf("\n\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Protonema 1910 LED Rack Display Test, Firmware 0.1.0-alpha.2\n");
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
    printf("Initialize ADC...\n");

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // ===========================================================================================
    printf("Initializing SNON entities...\n");

    snon_initialize("1910A 1U Rack Display");

    // Add measurands
    entity_register("Time Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"iso8601\",\"meAq\":\"count\"}");
    entity_register("Seconds Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"numeric\",\"meAq\":\"count\",\"meUS\":{\"*\":\"s\"},\"meUSx\":{\"*\":\"seconds\"},\"meR\":\"1\",\"meAc\":\"1\"}");

    // Add sensors
    entity_register("Device Time Sensor", SNON_CLASS_SENSOR, NULL);
    entity_add_relationship("Device Time Sensor", SNON_REL_CHILD_OF, "Device");
    entity_add_relationship("Device Time Sensor", SNON_REL_MEASURAND, "Time Measurand");

    entity_register("Device Uptime Sensor", SNON_CLASS_SENSOR, NULL);
    entity_add_relationship("Device Uptime Sensor", SNON_REL_CHILD_OF, "Device");
    entity_add_relationship("Device Uptime Sensor", SNON_REL_MEASURAND, "Seconds Measurand");

    // Add series
    entity_register("Device Time Series", SNON_CLASS_SERIES, NULL);
    entity_add_relationship("Device Time Series", SNON_REL_MEASURAND, "Time Measurand");
    entity_add_relationship("Device Time Series", SNON_REL_CHILD_OF, "Device");
    entity_add_relationship("Device Time Series", SNON_REL_VALUES, "Device Time");

    entity_register("Device Uptime Series", SNON_CLASS_SERIES, NULL);
    entity_add_relationship("Device Uptime Series", SNON_REL_MEASURAND, "Seconds Measurand");
    entity_add_relationship("Device Uptime Series", SNON_REL_CHILD_OF, "Device");
    entity_add_relationship("Device Uptime Series", SNON_REL_VALUES, "Device Uptime");

    // Add special values
    entity_register("Device Time", SNON_CLASS_VALUE, NULL);
    entity_register("Device Uptime", SNON_CLASS_VALUE, NULL);

    // Add values
    entity_register("Device Temp", SNON_CLASS_VALUE, NULL);
    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "[\"%4.2f\"]", rp2040_get_temp());
    entity_name_update("Device Temp", snprintf_buffer);
    add_repeating_timer_ms(1000, sensor_update, NULL, &sensor_timer);
 
    entity_register("Debug LED RGB", SNON_CLASS_VALUE, NULL);
    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "[\"0A000A\"]");
    entity_name_update("Debug LED RGB", snprintf_buffer);

    entity_register("Display Text", SNON_CLASS_VALUE, NULL);
    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "[\"\"]");
    entity_name_update("Display Text", snprintf_buffer);
 
    // ===========================================================================================
    printf("Display startup image...\n");

    // Copy the image into the frame buffer
    while(x_counter < width)
    {
        while(y_counter < height)
        {
            fb_set_grb(99 - x_counter, y_counter, urgb_u32((image[pixel_counter]) / 20,
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

    printf("Ready for commands\n");
    while (true)
    {
        // Check if there are any commands pending
        const char* command = uart_command_get();
        if(strcmp(command, "") != 0)
        {
            if(strcmp(command, "help") == 0)
            {
                uart_puts(uart1, "\nCommands:");
                uart_puts(uart1, "\n\"help\"                - Displays list of commands");
                uart_puts(uart1, "\n\"clear\"               - Clear the serial terminal");
                uart_puts(uart1, "\n\"ls\"                  - List SNON entites");
                uart_puts(uart1, "\n\"cat <entity>\"        - Display the value of an SNON entity");
                uart_puts(uart1, "\n\"get time\"            - Get the current time");
                uart_puts(uart1, "\n\"set time\"            - Set the current time");
                uart_puts(uart1, "\n\"display refresh\"     - Refresh the display");
                uart_puts(uart1, "\n\"display clear\"       - Set the entire frame buffer to black");
                uart_puts(uart1, "\n\"display text <str>\"  - Display a string on the display ");
                uart_puts(uart1, "\n\"display dump\"        - Dump the contents of the display to the terminal");
                uart_puts(uart1, "\n\"set pixel x,y to r g b\"   - Set a pixel at x,y to r g b");
                uart_command_clear();
            }
            else if(strcmp(command, "clear") == 0)
            {
                uart_puts(uart1, "\033[2J");
                uart_command_clear();
            }
            else if(strcmp(command, "ls") == 0)
            {
                char* json_output = entity_name_to_values("Entities");

                if(json_output != NULL)
                {
                    uart_puts(uart1, "\n");
                    uart_puts(uart1, json_output);
                    uart_puts(uart1, "\n");
                    free(json_output);
                }

                uart_command_clear();
            }
            else if(command[0] == '{')
            {
                char*       json_output = NULL;
                char        uuid[37];

                if(command[1] == '}')
                {
                    // Display all entities
                    char*       entity_output = NULL;
                    uint16_t    counter = 0;
                    uint16_t    json_output_length = 0;

                    json_output = entity_name_to_values("Entities");

                    if(json_output != NULL)
                    {
                        json_output_length = strlen(json_output);
                        uart_puts(uart1, "\n[");

                        while(json_output[counter] != 0)
                        {
                            if(json_output[counter] == '"')
                            {
                                sscanf(&json_output[counter + 1], "%36s", &snprintf_buffer);
                                entity_output = entity_uuid_to_json(snprintf_buffer);
                                uart_puts(uart1, entity_output);
                                free(entity_output);
                                
                                if(counter + 50 < json_output_length)
                                {
                                    uart_puts(uart1, ", ");
                                }

                                counter = counter + 37;
                            }

                            counter = counter + 1;
                        }

                        uart_puts(uart1, "]");
                        free(json_output);
                    }
                }
                else
                {
                    if(entity_has_eID(command, snprintf_buffer) == true)
                    {
                        // Copy over the UUID
                        strncpy(uuid, snprintf_buffer, 36);
                        uuid[36] = 0;
                        
                        if(entity_has_value(command, snprintf_buffer) == true)
                        {
                            // Update the value
                            printf("New Value %s\n", snprintf_buffer);
                            entity_uuid_update(uuid, snprintf_buffer);
                        }

                        json_output = entity_uuid_to_json(uuid);

                        if(json_output != NULL)
                        {
                            uart_puts(uart1, "\n");
                            uart_puts(uart1, json_output);
                            free(json_output);
                        }
                        else
                        {
                            printf("Unable to find entity with UUID %s\n", uuid);
                            uart_puts(uart1, "\n{\n}");
                        }
                    }
                    else
                    {
                        printf("Unable to find UUID\n");
                        uart_puts(uart1, "\n{\n}");
                    }
                }

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
            else if(strncmp(command, "display text ", 13) == 0)
            {
                fb_display_clear();
                insert_string((char*) &command[13], GENEVA, 0);
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

        json_output = entity_name_to_values("Debug LED RGB");

        if(json_output)
        {
            uint8_t r_value = 0;
            uint8_t g_value = 0;
            uint8_t b_value = 0;

            sscanf(json_output, "[\"%2X%2X%2X\"]", &r_value, &g_value, &b_value);
            ws2812_program_init(pio1, display_sm, display_sm_offset, SIGN_DEBUG_WS2812, 1200000, false);
            put_pixel(urgb_u32(r_value, g_value, b_value));
            free(json_output);
        }

        json_output = entity_name_to_values("Display Text");

        if(json_output)
        {
            if(strlen(json_output) > 4)
            {
                json_output[strlen(json_output) - 2] = 0;
                fb_display_clear();
                insert_string(&snprintf_buffer[2], GENEVA, 0);
            }

            free(json_output);
        }

        fb_display();

        sleep_ms(100);

    }
}
