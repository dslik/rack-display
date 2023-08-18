// ---------------------------------------------------------------------------------
// SNON Utilities - Header
// ---------------------------------------------------------------------------------
// Provides standard routines to register and manipulate SNON entities
// See https://www.snon.org/ for more details
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/snon-utils/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// Utility routines
void get_device_id(uint64_t* id);
void get_device_entity_uuid(char* entity_name, char* uuid_buffer);
