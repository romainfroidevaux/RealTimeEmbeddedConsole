/*******************************************************************************
 * LCD Display - Source code file
 *
 * This module provide an API to use the LCD display driver
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "lcdc.h"
#include "xpm.h"
#include "utils.h"
#include "view.h"
#include "font_8x8.h"

void lcddisplay_init() {
	lcdc_init();
	lcdc_enable();
	xpm_init();
	//printf ("lcddisplay_init done\n");
}

void lcddisplay_print_image(struct utils_position start_pos, struct xpm_image icon) {

	//Check to not print an image out of screen
	if (((start_pos.x + icon.width) > LCD_WIDTH) | ((start_pos.y + icon.height) > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_image : image position is out of screen\n");
		return;
	}

	uint16_t* lcdc_bitmap = lcdc_get_bitmap();

	//Print the image pixel by pixel
	for (uint32_t h = 0; h < icon.height; h++)
		for (uint32_t w=0; w<icon.width; w++)
			lcdc_bitmap[((h+start_pos.y)*LCD_WIDTH)+start_pos.x+w] = icon.image[h*icon.width+w];

	//printf ("lcddisplay_print_image done\n");
}

void lcddisplay_print_zone(struct utils_position start_pos, struct utils_position stop_pos, enum view_color color) {

	//Check to not print a zone out of screen
	if ((start_pos.x > LCD_WIDTH) | (start_pos.y > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_zone : start position is out of screen\n");
		return;
	} else if ((stop_pos.x > LCD_WIDTH) | (stop_pos.y > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_zone : stop position is out of screen\n");
		return;
	} else if ((start_pos.x > stop_pos.x) | (start_pos.y > stop_pos.y)) {
		printf ("ERROR : lcddisplay_print_zone : start_pos is bigger than stop_pos\n");
		return;
	}

	uint16_t* lcdc_bitmap = lcdc_get_bitmap();

	//Print the zone pixel by pixel
	for (uint16_t h = 0; h < (stop_pos.y - start_pos.y); h++)
		for (uint16_t w=0; w < (stop_pos.x - start_pos.x); w++)
			lcdc_bitmap[((h+start_pos.y)*LCD_WIDTH)+start_pos.x+w] = color;

	//printf ("lcddisplay_print_zone done\n");
}

void lcddisplay_print_grid(enum view_color color, enum view_grid_size size) {

	struct utils_position start_position;
	struct utils_position stop_position;

	uint16_t increment;

	//Check the grid size
	if (size == x66) {
		increment = 1;
	} else if (size == x33) {
		//For a x33 grid, print 1 line over 2 (based on x66 grid)
		increment = 2;
	} else {
		printf ("ERROR : lcddisplay_print_grid : invalid size\n");
		return;
	}

	for (uint16_t i = 0 ; i<7 ; i+=increment) {

		//Print vertical line
		start_position.x = 321 + (i * (3 + 76));
		start_position.y = 1;
		stop_position.x = start_position.x + 3 + 1;
		stop_position.y = 478;
		lcddisplay_print_zone(start_position, stop_position, color);

		//Print horizontal line
		start_position.x = 321;
		start_position.y = i * (3 + 76) + 1;
		stop_position.x = 320 + 478;
		stop_position.y = start_position.y + 3 + 1;
		lcddisplay_print_zone(start_position, stop_position, color);
	}
	//printf ("lcddisplay_print_grid done\n");
}

//To do : Use the lcddisplay_print_char function for each char of the string ... (22.05 | Romain)
void lcddisplay_print_text(char* string, struct utils_position start_pos, enum view_color color, bool whiteBackground) {

	if ((start_pos.x > LCD_WIDTH) | (start_pos.y > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_text : start position is out of screen\n");
		return;
	} else if (((start_pos.x + (8*strlen(string))) > LCD_WIDTH) | ((start_pos.y+4) > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_text : part of string is out of screen\n");
		return;
	}

	uint16_t* lcdc_bitmap = lcdc_get_bitmap();
	int string_len = strlen(string);

	//For each char of the string
	for (int sel_char = 0; sel_char < string_len; sel_char++) {
		uint32_t index = string[sel_char];
		const unsigned char* font = fontdata_8x8[index];

		//For each line (8) of pixels (8 per line) of the current char of the string
		for (uint16_t sel_line = 0; sel_line < 8; sel_line++) {
			//For each pixel (8) of the current line (sel_line)
			for (uint16_t sel_pixel = 0; sel_pixel < 8 ; sel_pixel++) {
				//If the current pixel is colored, set the color
				if (font[sel_line] >> (7-sel_pixel) & 1) {
					lcdc_bitmap[((start_pos.y + sel_line) * LCD_WIDTH + start_pos.x) + sel_pixel + 8 * sel_char] = color;
				//Else, displays a white pixel
				} else {
					if (whiteBackground)
						lcdc_bitmap[((start_pos.y + sel_line) * LCD_WIDTH + start_pos.x) + sel_pixel + 8 * sel_char] = white;
				}
			}
		}
	}
	//printf ("lcddisplay_print_text done\n");
}

void lcddisplay_print_char(char c, struct utils_position start_pos, enum view_color color, bool whiteBackground) {
	if ((start_pos.x > LCD_WIDTH) | (start_pos.y > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_char : start position is out of screen\n");
		return;
	} else if (((start_pos.x + 8) > LCD_WIDTH) | ((start_pos.y+4) > LCD_HEIGHT)) {
		printf ("ERROR : lcddisplay_print_char : part of char is out of screen\n");
		return;
	}

	uint16_t* lcdc_bitmap = lcdc_get_bitmap();

	#pragma GCC diagnostic ignored "-Wchar-subscripts"
	const unsigned char* font =  fontdata_8x8[c];
	#pragma GCC diagnostic pop

	//For each line (8) of pixels (8 per line) of the current char of the string
	for (uint16_t sel_line = 0; sel_line < 8; sel_line++) {
		//For each pixel (8) of the current line (sel_line)
		for (uint16_t sel_pixel = 0; sel_pixel < 8 ; sel_pixel++) {
			//If the current pixel is colored, set the color
			if (font[sel_line] >> (7-sel_pixel) & 1) {
				lcdc_bitmap[((start_pos.y + sel_line) * LCD_WIDTH + start_pos.x) + sel_pixel] = color;
			//Else, displays a white pixel
			} else {
				if (whiteBackground)
					lcdc_bitmap[((start_pos.y + sel_line) * LCD_WIDTH + start_pos.x) + sel_pixel] = white;
			}
		}
	}
	//printf ("lcddisplay_print_char done\n");
}

void lcddisplay_swap() {
	lcdc_memory_space_swap();
}
