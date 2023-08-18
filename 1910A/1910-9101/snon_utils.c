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
#include "sha1.h"

// Local Constants
#define SNON_UUID   "C3A4DD12-EFD4-537A-885C-50EC74A2CB12"

// =================================================================================

void get_device_id(uint64_t* id)
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

void get_device_entity_uuid(char* entity_name, char* uuid_buffer)
{
	uint64_t	device_id = 0;
	SHA1_CTX    sha1_context;
    uint8_t     sha1_result[20];
    uint8_t		counter = 0;
    uint8_t     offset = 0;

	get_device_id(&device_id);

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
