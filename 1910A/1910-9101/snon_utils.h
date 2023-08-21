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
void snon_initialize(char* device_name);

bool entity_register(char* entity_name, char* entity_class, char* initial_values);
bool entity_has_eID(const char* entity_json, char* uuid_buffer);
bool entity_has_value(const char* entity_json, char* value_buffer);
bool entity_add_relationship(char* entity_name, char* rel_type, char* rel_entity_name);
void entity_name_update(char* entity_name, char* updated_values);
void entity_uuid_update(char* entity_uuid, char* updated_values);
char* entity_name_to_json(char* entity_name);
char* entity_uuid_to_json(char* entity_uuid);
char* entity_name_to_values(char* entity_name);
char* entity_uuid_to_values(char* entity_uuid);
void entity_get_uuid(char* entity_name, char* uuid_buffer);

bool rtc_set_time(char* time_iso8601);
bool rtc_counter_to_iso8601(char* buffer, uint64_t counter);

// Defines

// SNON Classes
#define SNON_CLASS_DEVICE		"device"
#define SNON_CLASS_LOCATION		"location"
#define SNON_CLASS_SENSOR		"sensor"
#define SNON_CLASS_MEASURAND	"measurand"
#define SNON_CLASS_SERIES		"series"
#define SNON_CLASS_VALUE		"value"

// SNON Relationships
#define	SNON_REL_CHILD_OF		"child_of"
#define	SNON_REL_MEASURAND		"measurand"
#define	SNON_REL_POWERED_BY		"powered_by"
#define	SNON_REL_TIMESYNC_BY	"timesync_by"
#define	SNON_REL_CONNETED_TO	"connected_to"
#define	SNON_REL_LOCATED_AT		"located_at"
#define	SNON_REL_MEASURED_FROM	"measured_from"
#define	SNON_REL_HEALTH			"health"
#define	SNON_REL_SETPOINT		"setpoint"
#define	SNON_REL_ALARMS			"alarms"
#define	SNON_REL_ALARM_INHIBIT	"alarm_inhibit"
#define	SNON_REL_INDETERMINATE	"indeterminate"
#define	SNON_REL_FLAG			"flag"
#define	SNON_REL_INTERRUPTS		"interrupts"
#define	SNON_REL_VALUES			"values"



