// ---------------------------------------------------------------------------------
// Serial Command-line Utilities - Header
// ---------------------------------------------------------------------------------
// Provides a simple command line interface over a UART
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

#pragma once
#ifndef UART_H
#define UART_H

// Globals
extern volatile uint8_t collect_active;

// UART Setup
void uart_setup(void);
const char* uart_command_get(void);
void uart_command_clear(void);

#endif // UART_H