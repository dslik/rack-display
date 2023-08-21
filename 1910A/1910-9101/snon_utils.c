// ---------------------------------------------------------------------------------
// SNON Utilities
// ---------------------------------------------------------------------------------
// Provides standard routines to register and manipulate SNON entities
// See https://www.snon.org/ for more details
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/snon-utils/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// System Headers
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Pico Headers
#include "pico/unique_id.h"
#include "hardware/rtc.h"

// Local Headers
#include "snon_utils.h"
#include "sha1.h"
#include "cJSON.h"

// Local Constants
#define SNON_UUID   "C3A4DD12-EFD4-537A-885C-50EC74A2CB12"

// Local Prototypes
bool rtc_now_to_counter(char* buffer);
bool rtc_now_to_iso8601(char* buffer);
bool rtc_counter_to_iso8601(char* buffer, uint64_t counter);
void device_get_id(uint64_t* id);
char nibble_to_hexchar(uint8_t nibble);
bool string_only_numbers(char* the_string);
void string_char_sub(char* the_string, char orig, char sub);


// Local Globals
cJSON*   snon_root = NULL;
cJSON*   snon_list = NULL;

uint64_t rtc_set_count = 0;
uint64_t rtc_set_epoch = 0;


// =================================================================================

void snon_initialize(char* entity_name)
{
    char    uuid[37];
    char    current_time[28];
    cJSON*  entity = NULL;
    cJSON*  name = NULL;
    cJSON*  array = NULL;

    rtc_init();

    snon_root = cJSON_CreateObject();
    snon_list = cJSON_CreateArray();

    // Create the device entity
    entity_get_uuid("Device", uuid);
    cJSON_AddItemToObject(snon_root, uuid, entity = cJSON_CreateObject());
    cJSON_AddStringToObject(entity, "eC", "device");
    cJSON_AddStringToObject(entity, "eID", uuid);
    cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
    cJSON_AddStringToObject(name, "*", entity_name);

    // Add the entity to the master entity list
    cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));

    // Create the entity list entity
    entity_get_uuid("Entities", uuid);
    cJSON_AddItemToObject(snon_root, uuid, entity = cJSON_CreateObject());
    cJSON_AddStringToObject(entity, "eC", "value");
    cJSON_AddStringToObject(entity, "eID", uuid);
    cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
    cJSON_AddStringToObject(name, "*", "Entities");
    cJSON_AddItemToObject(entity, "v", snon_list);
    cJSON_AddItemToObject(entity, "vT", array = cJSON_CreateArray());
    rtc_now_to_counter(current_time);
    cJSON_AddItemToArray(array, cJSON_CreateString(current_time));

    // Add the entity to the master entity list
    cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));
}

bool entity_has_eID(const char* entity_json, char* uuid_buffer)
{
    cJSON*  entity = NULL;
    cJSON*  eID = NULL;
    char*   eID_string = NULL;
    bool    has_eID = false;

    entity = cJSON_Parse(entity_json);

    if(entity != NULL)
    {
        eID = cJSON_GetObjectItemCaseSensitive(entity, "eID");

        if(eID != NULL)
        {
            eID_string = cJSON_GetStringValue(eID);
            strcpy(uuid_buffer, eID_string);

            has_eID = true;
        }

        cJSON_free(entity);
    }

    return(has_eID);
}

bool entity_has_value(const char* entity_json, char* value_buffer)
{
    cJSON*  entity = NULL;
    cJSON*  value_array = NULL;
    char*   value_array_string = NULL;
    bool    has_value = false;

    entity = cJSON_Parse(entity_json);

    if(entity != NULL)
    {
        value_array = cJSON_GetObjectItemCaseSensitive(entity, "v");

        if(value_array != NULL)
        {
            value_array_string = cJSON_PrintUnformatted(value_array);
            strcpy(value_buffer, value_array_string);
            cJSON_free(value_array_string);

            has_value = true;
        }

        cJSON_free(entity);
    }

    return(has_value);
}

bool entity_register(char* entity_name, char* entity_class, char* initial_values)
{
    char    uuid[37];
    char    current_time[28];
    cJSON*  entity = NULL;
    cJSON*  name = NULL;
    cJSON*  array = NULL;
    bool    register_result = false;

    entity_get_uuid(entity_name, uuid);

    if(initial_values != NULL)
    {
        // If additional data is passed, parse it
        entity = cJSON_Parse(initial_values);
    }
    else
    {
        // Otherwise, create an empty object for the entity
        entity = cJSON_CreateObject();
    }

    if(entity != NULL)
    {
        cJSON_AddItemToObject(snon_root, uuid, entity);
        cJSON_AddStringToObject(entity, "eC", entity_class);
        cJSON_AddStringToObject(entity, "eID", uuid);
        cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
        cJSON_AddStringToObject(name, "*", entity_name);

        if(entity_class == SNON_CLASS_VALUE)
        {
            rtc_now_to_counter(current_time);

            cJSON_AddItemToObject(entity, "vT", array = cJSON_CreateArray());
            cJSON_AddItemToArray(array, cJSON_CreateString(current_time));
        }

        if(strcmp(entity_name, "Device Time") == 0 ||
           strcmp(entity_name, "Device Uptime") == 0)
        {
            cJSON_AddItemToObject(entity, "v", array = cJSON_CreateArray());
            cJSON_AddItemToArray(array, cJSON_CreateString(current_time));
        }

        cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));
        register_result = true;
    }

    return(register_result);
}

bool entity_add_relationship(char* entity_name, char* rel_type, char* rel_entity_name)
{
    char        uuid[37];
    cJSON*      entity = NULL;
    cJSON*      rel = NULL;
    cJSON*      array = NULL;
    bool        add_result = false;

    // Find the UUID for the entity
    entity_get_uuid(entity_name, uuid);

    entity = cJSON_GetObjectItemCaseSensitive(snon_root, uuid);
    if(entity != NULL)
    {
        rel = cJSON_GetObjectItemCaseSensitive(entity, "eR");

        if(rel == NULL)
        {
            cJSON_AddItemToObject(entity, "eR", rel = cJSON_CreateObject());
        }

        array = cJSON_GetObjectItemCaseSensitive(rel, rel_type);

        if(array == NULL)
        {
            cJSON_AddItemToObject(rel, rel_type, array = cJSON_CreateArray());
        }

        entity_get_uuid(rel_entity_name, uuid);
        cJSON_AddItemToArray(array, cJSON_CreateString(uuid));

        add_result = true;
    }

    return(add_result);
}


void entity_name_update(char* entity_name, char* updated_values)
{
    char        uuid[37];

    // Find the UUID for the entity
    entity_get_uuid(entity_name, uuid);

    return(entity_uuid_update(uuid, updated_values));
}

void entity_uuid_update(char* entity_uuid, char* updated_values)
{
    char        uuid[37];
    char        current_time[28];
    cJSON*      entity = NULL;
    cJSON*      new_value = NULL;
    cJSON*      new_time_value = NULL;
    char*       new_time_value_string = NULL;
    cJSON*      value_array = NULL;
    cJSON*      value_time_array = NULL;
    cJSON*      array = NULL;
    bool        is_time_entity = false;

    // Check if it is the time entity
    entity_get_uuid("Device Time", uuid);
    if(strcmp(entity_uuid, uuid) == 0)
    {
        is_time_entity = true;
    }

    // Parse the value array
    new_value = cJSON_Parse(updated_values);

    if(new_value != NULL)
    {
        // Find the entity by UUID
        entity = cJSON_GetObjectItemCaseSensitive(snon_root, entity_uuid);
        if(entity != NULL)
        {
            rtc_now_to_counter(current_time);
            
            value_array = cJSON_GetObjectItemCaseSensitive(entity, "v");
            if(value_array != NULL)
            {
                cJSON_Delete(cJSON_DetachItemFromObject(entity, "v"));
            }

            // Special case for updating the time
            if(is_time_entity)
            {
                new_time_value = cJSON_GetArrayItem(new_value, 0);
                if(new_time_value != NULL)
                {
                    new_time_value_string = cJSON_GetStringValue(new_time_value);
                    rtc_set_time(new_time_value_string);
                }

                cJSON_AddItemToObject(entity, "v", array = cJSON_CreateArray());
                cJSON_AddItemToArray(array, cJSON_CreateString(current_time));
            }
            else
            {
                cJSON_AddItemToObject(entity, "v", new_value);
            }

            value_time_array = cJSON_GetObjectItemCaseSensitive(entity, "vT");
            if(value_time_array != NULL)
            {
                cJSON_Delete(cJSON_DetachItemFromObject(entity, "vT"));
            }

            cJSON_AddItemToObject(entity, "vT", array = cJSON_CreateArray());
            cJSON_AddItemToArray(array, cJSON_CreateString(current_time));
        }
        else
        {
            cJSON_free(new_value);
        }
    }
    else
    {
        printf("\nError: Invalid value %s", updated_values);
    }
}

char* entity_name_to_json(char* entity_name)
{
    char        uuid[37];

    // Find the UUID for the entity
    entity_get_uuid(entity_name, uuid);

    return(entity_uuid_to_json(uuid));
}

char* entity_uuid_to_json(char* entity_uuid)
{
    char        uuid[37];
    char        new_time[28];
    char*       entity_json = NULL;
    cJSON*      entity = NULL;
    cJSON*      time_array = NULL;
    cJSON*      entity_time = NULL;
    cJSON*      value_array = NULL;
    char*       entity_time_string = NULL;
    uint64_t    entity_time_value = 0;
    bool        is_time_entity = false;
    bool        is_uptime_entity = false;

    // Check if it is the time entity
    entity_get_uuid("Device Time", uuid);
    if(strcmp(entity_uuid, uuid) == 0)
    {
        is_time_entity = true;
    }

    entity_get_uuid("Device Uptime", uuid);
    if(strcmp(entity_uuid, uuid) == 0)
    {
        is_uptime_entity = true;
    }

    // Find the entity by UUID
    entity = cJSON_GetObjectItemCaseSensitive(snon_root, entity_uuid);
    if(entity != NULL)
    {
        time_array = cJSON_GetObjectItemCaseSensitive(entity, "vT");

        if(time_array != NULL)
        {
            if(is_time_entity || is_uptime_entity)
            {
                // Special handling for the device time entity
                entity_time_value = time_us_64();
                if(rtc_counter_to_iso8601(new_time, entity_time_value) == false)
                {
                    rtc_now_to_counter(new_time);
                }

                cJSON_ReplaceItemInArray(time_array, 0, cJSON_CreateString(new_time));

                value_array = cJSON_GetObjectItemCaseSensitive(entity, "v");
                if(value_array != NULL)
                {
                    if(is_uptime_entity)
                    {
                        rtc_now_to_counter(new_time);
                    }

                    cJSON_ReplaceItemInArray(value_array, 0, cJSON_CreateString(new_time));
                }
            }
            else
            {
                entity_time = cJSON_GetArrayItem(time_array, 0);
                entity_time_string = cJSON_GetStringValue(entity_time);
                
                // Only replace if it is a raw timestamp
                if(string_only_numbers(entity_time_string))
                {
                    sscanf(entity_time_string, "%llu", &entity_time_value);
                    if(rtc_counter_to_iso8601(new_time, entity_time_value) == true)
                    {
                        cJSON_ReplaceItemInArray(time_array, 0, cJSON_CreateString(new_time));
                    }
                }
            }
        }
    }

    if(entity != NULL)
    {
        entity_json = cJSON_PrintUnformatted(entity);
    }

    return(entity_json);
}

char* entity_name_to_values(char* entity_name)
{
    char        uuid[37];

    // Find the UUID for the entity
    entity_get_uuid(entity_name, uuid);

    return(entity_uuid_to_values(uuid));
}

char* entity_uuid_to_values(char* entity_uuid)
{
    char*       entity_json = NULL;
    cJSON*      entity = NULL;
    cJSON*      value_array = NULL;

    // Find the entity by UUID
    entity = cJSON_GetObjectItemCaseSensitive(snon_root, entity_uuid);
    if(entity != NULL)
    {
        value_array = cJSON_GetObjectItemCaseSensitive(entity, "v");

        if(value_array != NULL)
        {
            entity_json = cJSON_PrintUnformatted(value_array);

            string_char_sub(entity_json, ' ', '\n');
        }
    }

    return(entity_json);
}

void entity_get_uuid(char* entity_name, char* uuid_buffer)
{
    uint64_t    device_id = 0;
    SHA1_CTX    sha1_context;
    uint8_t     sha1_result[20];
    uint8_t     counter = 0;
    uint8_t     offset = 0;

    device_get_id(&device_id);

    SHA1Init(&sha1_context);
    SHA1Update(&sha1_context, SNON_UUID, strlen(SNON_UUID));
    SHA1Update(&sha1_context, &device_id, 8);
    SHA1Update(&sha1_context, entity_name, strlen(entity_name));
    SHA1Final(&sha1_result, &sha1_context);

    uuid_buffer[36] = 0;

    while(counter < 16)
    {
        if(counter == 4 || counter == 6 || counter == 8 || counter == 10)
        {
            uuid_buffer[(counter * 2) + offset] = '-';
            offset = offset + 1;
        }

        uuid_buffer[(counter * 2) + offset] = nibble_to_hexchar(sha1_result[counter] >> 4);
        uuid_buffer[(counter * 2) + 1 + offset] = nibble_to_hexchar(sha1_result[counter] & 0x0F);

        if(counter == 6)
        {
            uuid_buffer[(counter * 2) + offset] = '5';
        }

        if(counter == 8)
        {
            uuid_buffer[(counter * 2) + offset] = nibble_to_hexchar(((sha1_result[counter] >> 4) & 0b1011) | 0b1000);
        }

        counter = counter + 1;
    }
}

// =================================================================================

bool rtc_now_to_counter(char* buffer)
{
    snprintf(buffer, 28, "%llu", time_us_64());

    return(true);
}

bool rtc_now_to_iso8601(char* buffer)
{
    bool        get_time_valid = false;
    datetime_t  t;

    // Length of ISO8601 string used here is 28 characters:
    // "YYYY-MM-DDTHH:MM:SS.123456Z"
    //  123456789012345678901234567

    get_time_valid = rtc_get_datetime(&t);

    if(get_time_valid == true)
    {
        snprintf(buffer, 28, "%04d-%02d-%02dT%02d:%02d:%02d.%06lluZ", t.year, t.month, t.day, t.hour, t.min, t.sec, (time_us_64() % 1000000));
    }
    
    return(get_time_valid);
}

bool rtc_set_time(char* time_iso8601)
{
    unsigned int    year = 0;
    unsigned int    month = 0;
    unsigned int    day = 0;
    unsigned int    hours = 0;
    unsigned int    minutes = 0;
    unsigned int    seconds = 0;
    bool            set_valid = true;
    uint64_t        before_count = 0;
    uint64_t        after_count = 0;
    datetime_t      time_pico;
    struct tm       time_c;

    sscanf(time_iso8601, "%4u-%2u-%2uT%2u:%2u:%2uZ", &year, &month, &day, &hours, &minutes, &seconds);
    if(year < 2022 || year > 2055)
    {
        set_valid = false;
    }
    else if(month < 1 || month > 12)
    {
        set_valid = false;
    }
    else if(day < 1 || day > 31)
    {
        set_valid = false;
    }
    else if(hours < 0 || hours > 23)
    {
        set_valid = false;
    }
    else if(minutes < 0 || minutes > 59)
    {
        set_valid = false;
    }
    else if(seconds < 0 || seconds > 59)
    {
        set_valid = false;
    }
    
    if(set_valid == true)
    {
        time_pico.year = time_c.tm_year = year;
        time_pico.month = time_c.tm_mon = month;
        time_pico.day = time_c.tm_mday = day;
        time_pico.hour = time_c.tm_hour = hours;
        time_pico.min = time_c.tm_min = minutes;
        time_pico.sec = time_c.tm_sec = seconds;
        time_pico.dotw = 0;
        time_c.tm_isdst = -1;

        before_count = time_us_64();
        set_valid = rtc_set_datetime(&time_pico);
        after_count = time_us_64();

        rtc_set_count = after_count - ((after_count - before_count) / 2);
        rtc_set_epoch = (uint64_t) mktime(&time_c);

        printf("\nsc = %llu, offset = %llu", rtc_set_count, rtc_set_epoch);


    }

    return(set_valid);
}

bool rtc_counter_to_iso8601(char* buffer, uint64_t counter)
{
    bool        get_time_valid = false;
    struct tm   time;
    int64_t     offset_counter = 0;
    uint64_t    epoch_counter = 0;
    uint64_t    epoch_counter_secs = 0;

    // Length of ISO8601 string used here is 28 characters:
    // "YYYY-MM-DDTHH:MM:SS.123456Z"
    //  123456789012345678901234567

    if(rtc_running())
    {
        offset_counter = counter - rtc_set_count;
        epoch_counter = (rtc_set_epoch * 1000000) + offset_counter;
        epoch_counter_secs = epoch_counter / 1000000;

        gmtime_r(&epoch_counter_secs, &time);

        snprintf(buffer, 28, "%04d-%02d-%02dT%02d:%02d:%02d.%06lluZ",
                 time.tm_year, time.tm_mon, time.tm_mday,
                 time.tm_hour, time.tm_min, time.tm_sec,
                 (epoch_counter % 1000000));

        get_time_valid = true;
    }
    
    return(get_time_valid);
}

void device_get_id(uint64_t* id)
{
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);
    *id = *((uint64_t*)(board_id.id));
}

char nibble_to_hexchar(uint8_t nibble)
{
    char hex_value = '_';

    if(nibble <= 9)
    {
        hex_value = '0' + nibble;
    }
    else if(nibble <= 15)
    {
        hex_value = 'A' + nibble - 10;
    }

    return(hex_value);
}

bool string_only_numbers(char* the_string)
{
    bool        only_numbers = true;
    uint16_t    string_counter = 0;

    while(the_string[string_counter] != 0 && only_numbers == true)
    {
        if(the_string[string_counter] < '0' || the_string[string_counter] > '9')
        {
            only_numbers = false;
        }

        string_counter = string_counter + 1;
    }

    return(only_numbers);
}

void string_char_sub(char* the_string, char orig, char sub)
{
    uint16_t    string_counter = 0;

    while(the_string[string_counter] != 0)
    {
        if(the_string[string_counter] == orig)
        {
            the_string[string_counter] = sub;
        }

        string_counter = string_counter + 1;
    }
}



