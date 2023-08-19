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

// Pico Headers
#include "pico/unique_id.h"

// Local Headers
#include "snon_utils.h"
#include "sha1.h"
#include "cJSON.h"

// Local Constants
#define SNON_UUID   "C3A4DD12-EFD4-537A-885C-50EC74A2CB12"

// Local Globals
cJSON   *snon_root = NULL;
cJSON   *snon_list = NULL;


// =================================================================================

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

void device_initialize(char* entity_name)
{
    char    uuid[37];
    char    current_time[20];
    cJSON   *entity = NULL;
    cJSON   *name = NULL;
    cJSON   *array = NULL;

    snon_root = cJSON_CreateObject();
    snon_list = cJSON_CreateArray();

    // Create the device entity
    entity_get_uuid("device", uuid);
    cJSON_AddItemToObject(snon_root, uuid, entity = cJSON_CreateObject());
    cJSON_AddStringToObject(entity, "eC", "device");
    cJSON_AddStringToObject(entity, "eID", uuid);
    cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
    cJSON_AddStringToObject(name, "*", entity_name);

    // Add the entity to the master entity list
    cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));

    // Create the entity list entity
    entity_get_uuid("entities", uuid);
    cJSON_AddItemToObject(snon_root, uuid, entity = cJSON_CreateObject());
    cJSON_AddStringToObject(entity, "eC", "value");
    cJSON_AddStringToObject(entity, "eID", uuid);
    cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
    cJSON_AddStringToObject(name, "*", "Defined Entities");
    cJSON_AddItemToObject(entity, "eV", snon_list);

    cJSON_AddItemToObject(entity, "eT", array = cJSON_CreateArray());
    snprintf(current_time, 19, "%llu", time_us_64());
    cJSON_AddItemToArray(array, cJSON_CreateString(current_time));

    // Add the entity to the master entity list
    cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));
}

void entity_register(char* entity_name, char* initial_values)
{
    char    uuid[37];
    char    current_time[20];
    cJSON   *entity = NULL;
    cJSON   *name = NULL;
    cJSON   *array = NULL;

    entity_get_uuid(entity_name, uuid);

    cJSON_AddItemToObject(snon_root, uuid, entity = cJSON_Parse(initial_values));
    cJSON_AddStringToObject(entity, "eID", uuid);
    cJSON_AddItemToObject(entity, "eN", name = cJSON_CreateObject());
    cJSON_AddStringToObject(name, "*", entity_name);

    cJSON_AddItemToObject(entity, "eV", array = cJSON_CreateArray());
    snprintf(current_time, 19, "%llu", time_us_64());
    cJSON_AddItemToArray(array, cJSON_CreateString(current_time));

    cJSON_AddItemToObject(entity, "eT", array = cJSON_CreateArray());
    cJSON_AddItemToArray(array, cJSON_CreateString(current_time));

    cJSON_AddItemToArray(snon_list, cJSON_CreateString(uuid));

    printf("\n%s\n", cJSON_Print(snon_root));
}


void entity_update(char* entity_name, char* updated_values)
{



}


void entity_get_uuid(char* entity_name, char* uuid_buffer)
{
	uint64_t	device_id = 0;
	SHA1_CTX    sha1_context;
    uint8_t     sha1_result[20];
    uint8_t		counter = 0;
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
