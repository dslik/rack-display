// ---------------------------------------------------------------------------------
// Font Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist displaying text on the 1910A display.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "fonts.h"
#include "image.h"
#include "ws2812.h"

// Local Prototypes
const char* character_to_array(char character, uint8_t typeface);
const uint8_t character_to_width(char character, uint8_t typeface);


// =================================================================================

int16_t insert_character(char character, uint8_t typeface, int16_t x_offset)
{
	const char*	character_data = character_to_array(character, typeface);
	const uint8_t character_length = character_to_width(character, typeface);
	uint16_t counter = 0;
	uint8_t row_counter = 0;
	uint8_t col_counter = 0;

	if(character_data != NULL)
	{
		while(counter != character_length * 10)
		{
            if(col_counter + x_offset >= 0 && col_counter + x_offset <= 99)
            {
	            fb_set_grb(col_counter + x_offset, row_counter, urgb_u32(character_data[counter], 0, 0));
            }

			row_counter = row_counter + 1;

			if(row_counter == 10)
			{
				col_counter = col_counter + 1;
				row_counter = 0;
			}

			counter = counter + 1;
		}

	}

	return(x_offset + character_length);
}


int16_t insert_string(char* string, uint8_t typeface, int16_t x_offset)
{
	uint16_t counter = 0;
	int8_t offset = x_offset;

	while(string[counter] != 0)
	{
		offset = insert_character(string[counter], typeface, offset);

		counter = counter + 1;
	}

	return(offset);
}

const char* character_to_array(char character, uint8_t typeface)
{
	if(typeface == geneva_bold)
	{
		if(character == ' ') return(glyph_sp_bold);
		if(character == 'a' || character == 'A') return(glyph_a_bold);
		if(character == 'b' || character == 'B') return(glyph_b_bold);
		if(character == 'c' || character == 'C') return(glyph_c_bold);
		if(character == 'd' || character == 'D') return(glyph_d_bold);
		if(character == 'e' || character == 'E') return(glyph_e_bold);
		if(character == 'f' || character == 'F') return(glyph_f_bold);
		if(character == 'g' || character == 'G') return(glyph_g_bold);
		if(character == 'h' || character == 'H') return(glyph_h_bold);
		if(character == 'i' || character == 'I') return(glyph_i_bold);
		if(character == 'j' || character == 'J') return(glyph_j_bold);
		if(character == 'k' || character == 'K') return(glyph_k_bold);
		if(character == 'l' || character == 'L') return(glyph_l_bold);
		if(character == 'm' || character == 'M') return(glyph_m_bold);
		if(character == 'n' || character == 'N') return(glyph_n_bold);
		if(character == 'o' || character == 'O') return(glyph_o_bold);
		if(character == 'p' || character == 'P') return(glyph_p_bold);
		if(character == 'q' || character == 'Q') return(glyph_q_bold);
		if(character == 'r' || character == 'R') return(glyph_r_bold);
		if(character == 's' || character == 'S') return(glyph_s_bold);
		if(character == 't' || character == 'T') return(glyph_t_bold);
		if(character == 'u' || character == 'U') return(glyph_u_bold);
		if(character == 'v' || character == 'V') return(glyph_v_bold);
		if(character == 'w' || character == 'W') return(glyph_w_bold);
		if(character == 'x' || character == 'X') return(glyph_x_bold);
		if(character == 'y' || character == 'Y') return(glyph_y_bold);
		if(character == 'z' || character == 'Z') return(glyph_z_bold);
		if(character == '1') return(glyph_1_bold);
		if(character == '2') return(glyph_2_bold);
		if(character == '3') return(glyph_3_bold);
		if(character == '4') return(glyph_4_bold);
		if(character == '5') return(glyph_5_bold);
		if(character == '6') return(glyph_6_bold);
		if(character == '7') return(glyph_7_bold);
		if(character == '8') return(glyph_8_bold);
		if(character == '9') return(glyph_9_bold);
		if(character == '0') return(glyph_0_bold);
		if(character == '!') return(glyph_ex_bold);
		if(character == '(') return(glyph_ob_bold);
		if(character == ')') return(glyph_cb_bold);
		if(character == ',') return(glyph_com_bold);
		if(character == '.') return(glyph_per_bold);
	}

	return(NULL);
}

const uint8_t character_to_width(char character, uint8_t typeface)
{
	if(typeface == geneva_bold)
	{
		if(character == ' ') return(glyph_sp_bold_width);
		if(character == 'a' || character == 'A') return(glyph_a_bold_width);
		if(character == 'b' || character == 'B') return(glyph_b_bold_width);
		if(character == 'c' || character == 'C') return(glyph_c_bold_width);
		if(character == 'd' || character == 'D') return(glyph_d_bold_width);
		if(character == 'e' || character == 'E') return(glyph_e_bold_width);
		if(character == 'f' || character == 'F') return(glyph_f_bold_width);
		if(character == 'g' || character == 'G') return(glyph_g_bold_width);
		if(character == 'h' || character == 'H') return(glyph_h_bold_width);
		if(character == 'i' || character == 'I') return(glyph_i_bold_width);
		if(character == 'j' || character == 'J') return(glyph_j_bold_width);
		if(character == 'k' || character == 'K') return(glyph_k_bold_width);
		if(character == 'l' || character == 'L') return(glyph_l_bold_width);
		if(character == 'm' || character == 'M') return(glyph_m_bold_width);
		if(character == 'n' || character == 'N') return(glyph_n_bold_width);
		if(character == 'o' || character == 'O') return(glyph_o_bold_width);
		if(character == 'p' || character == 'P') return(glyph_p_bold_width);
		if(character == 'q' || character == 'Q') return(glyph_q_bold_width);
		if(character == 'r' || character == 'R') return(glyph_r_bold_width);
		if(character == 's' || character == 'S') return(glyph_s_bold_width);
		if(character == 't' || character == 'T') return(glyph_t_bold_width);
		if(character == 'u' || character == 'U') return(glyph_u_bold_width);
		if(character == 'v' || character == 'V') return(glyph_v_bold_width);
		if(character == 'w' || character == 'W') return(glyph_w_bold_width);
		if(character == 'x' || character == 'X') return(glyph_x_bold_width);
		if(character == 'y' || character == 'Y') return(glyph_y_bold_width);
		if(character == 'z' || character == 'Z') return(glyph_z_bold_width);
		if(character == '1') return(glyph_1_bold_width);
		if(character == '2') return(glyph_2_bold_width);
		if(character == '3') return(glyph_3_bold_width);
		if(character == '4') return(glyph_4_bold_width);
		if(character == '5') return(glyph_5_bold_width);
		if(character == '6') return(glyph_6_bold_width);
		if(character == '7') return(glyph_7_bold_width);
		if(character == '8') return(glyph_8_bold_width);
		if(character == '9') return(glyph_9_bold_width);
		if(character == '0') return(glyph_0_bold_width);
		if(character == '!') return(glyph_ex_bold_width);
		if(character == '(') return(glyph_ob_bold_width);
		if(character == ')') return(glyph_cb_bold_width);
		if(character == ',') return(glyph_com_bold_width);
		if(character == '.') return(glyph_per_bold_width);
	}

	return(0);		
}