// ---------------------------------------------------------------------------------
// WS2812 Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist in programming with WS2812-style smart RGB LEDs.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

#include "ws2812.h"

// =================================================================================
// SPDX-SnippetBegin
// SPDX-SnippetCopyrightText:  Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// SDPX-SnippetAttributionText: https://github.com/raspberrypi/pico-examples/blob/master/pio/ws2812/ws2812.c
// SPDX-License-Identifier: BSD-3-Clause
// ---------------------------------------------------------------------------------

inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio1, 0, pixel_grb << 8u);
}

inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    if(r > 26) r = 26;
    if(g > 26) g = 26;
    if(b > 26) b = 26;

    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

// SPDX-SnippetEnd
// ---------------------------------------------------------------------------------
