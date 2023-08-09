// ---------------------------------------------------------------------------------
// WS2812 Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist in programming with WS2812-style smart RGB LEDs.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

#include "hardware/pio.h"
#include "ws2812.pio.h"

extern void put_pixel(uint32_t pixel_grb);
extern uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

