/**
 * EIA-FR - Embedded Systems 1 laboratory
 *
 * Abstract: 	TP5: Introduction to XPM File Format & LDC Display
 *
 * Purpose:	This module is simple program to convert xpm file into a 
 * 		16 bits bitmap to be printed out a LCD display
 *
 * Author:	Daniel Gachet / EIA-FR / Telecoms
 * Date:	26.01.2014
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "xpm.h"


/**
 * color code structure used to build color map
 */
struct color_code
{
	uint32_t code;		// color code
	uint32_t color;		// color value in RGB565
};


/* implementation ---------------------------------------------------------- */

/**
 * Convert an ascii hex value into a interger value
 */
static uint32_t a2i (char c)
{
	static const uint8_t lut[256] = {
		['0']=0, ['1']=1, ['2']=2, ['3']=3, ['4']=4,
		['5']=5, ['6']=6, ['7']=7, ['8']=8, ['9']=9,
		['a']=10,['b']=11,['c']=12,['d']=13,['e']=14,['f']=15,
		['A']=10,['B']=11,['C']=12,['D']=13,['E']=14,['F']=15,
	};
	return lut[(unsigned)c];
}

/* ------------------------------------------------------------------------- */

/**
 * Convert RGB888 value into RGB565
 */
static uint32_t rgb565 (uint32_t r, uint32_t g, uint32_t b)
{
	return (   (r & 0xf8ul) << (11-3)) 
		| ((g & 0xfcul) << (5-2)) 
		| ((b & 0xf8ul) >> 3);
}

/* ------------------------------------------------------------------------- */

#ifdef __QSORT__
/**
 * qsort comparison routine
 */
static int qsort_cmp (const void* a, const void* b)
{
	return struct color_code*)a)->code - ((struct color_code*)b)->code; 
}

/* ------------------------------------------------------------------------- */

/**
 * Get associated color value to a color code contained within the color map
 *
 * @param map color map
 * @param colors numbers of colors contained into the color map
 * @param line one line of the image to convert
 * @param chars number of characters per color-code
 * @result associated color value
 */
static uint16_t get_color(
	struct color_code* map,
	uint32_t colors, 
	const char* line, 
	uint32_t chars)
{
	uint32_t code = 0;
	for (int i = chars-1; i >= 0; i--) code = (code << 8) + line[i];

	static struct color_code cache = {.code = 0, .color = 0,};
	if (cache.code == code) return cache.color;

	struct color_code *ele = bsearch (&code, map, colors, sizeof(struct color_code), qsort_cmp);
	if (ele != 0) {
		cache.code = code;	
		cache.color = ele->color;
		return cache.color; 
	}
	return 0; 
}

#else

/**
 * Search one element (color-code) within the color map 
 *
 * @param code color code to search within the color map
 * @param map color map
 * @param colors number of colors contained within the color map
 * @result index(position) of the color-code paar within the color map. If
 * 	 	the returned value is positive (>=0), the code has been found, 
 *		otherwise the returned value indicates the position to insert 
 *		the new element -1
 */
static int search_ele (uint32_t code, struct color_code* map, int colors)
{
	/* binary search with ic_divider table */
	int l = -1;
	int r = colors;
	while (r - l > 1) {
		int m = (l + r) / 2;
		if ((map[m].code < code))
			l = m;
		else
			r = m;
	}
	if (map[r].code != code) r = -r-1;

	return r;
}

/* ------------------------------------------------------------------------- */

/**
 * Ìnsert one color-code paar into the color map
 *
 * @param ele element (color-code) to be inserted into the map
 * @param map color map
 * @param colors number of colors contained within the color map
 */
static void insert_ele (
	const struct color_code* ele, 
	struct color_code* map, 
	uint32_t colors)
{
    int index = search_ele (ele->code, map, colors);
    if (index < 0) {
	index = -index-1;
	uint32_t x = colors - index;
	if (x > 0) memmove (&map[index+1], &map[index], x*sizeof(*ele));
        map[index] = *ele;
    }
}

/* ------------------------------------------------------------------------- */

/**
 * Get associated color value to a color code contained within the color map
 *
 * @param map color map
 * @param colors numbers of colors contained into the color map
 * @param line one line of the image to convert
 * @param chars number of characters per color-code
 * @result associated color value
 */
static uint16_t get_color(
	struct color_code* map,
	uint32_t colors, 
	const char* line, 
	uint32_t chars)
{
	uint32_t code = 0;
	for (int i = chars-1; i >= 0; i--) code = (code << 8) + line[i];

	static struct color_code cache = {.code = 0, .color = 0,};
	if (cache.code == code) return cache.color;

	int i = search_ele (code, map, colors);
	if (i >= 0) {
		cache.code = code;
		cache.color = map[i].color;
		return cache.color;
	}
	return 0; 
}
#endif

/* ------------------------------------------------------------------------- */

/**
 * Parse a color line to extract color code and color value
 *
 * @param color_string color line to be parsed
 * @param chars number of characters per color code
 * @result converted color code & value
 */
static struct color_code parse_color(
	const char* c_str, 
	int chars)
{
	struct color_code map = {.code=0, .color=0,};
	for (int i = chars-1; i >= 0; i--) 
		map.code = (map.code << 8) + c_str[i];
	c_str += chars;

	while (*c_str != 0)
		if (*c_str++ == '#') {
			uint32_t r = (a2i(c_str[0]) << 4) + a2i(c_str[1]);
			uint32_t g = (a2i(c_str[2]) << 4) + a2i(c_str[3]);
			uint32_t b = (a2i(c_str[4]) << 4) + a2i(c_str[5]);
			map.color = rgb565(r,g,b);
			break;
		}

	return map;
}

/* ------------------------------------------------------------------------- */

void xpm_init()
{ }


/* ------------------------------------------------------------------------- */

struct xpm_image xpm_convert_image (char* xpm_data[])
{
	struct xpm_image xpm = {.width=0, .height=0, .image=0,};
	uint32_t colors = 0;
	uint32_t chars = 0;

   	sscanf(xpm_data[0], "%lu %lu %lu %lu", 
		&xpm.width, &xpm.height, &colors, &chars);

	xpm.image = calloc(xpm.height * xpm.width, sizeof(*xpm.image));
	struct color_code* map = calloc (colors, sizeof(*map));
	if ((xpm.image != 0) && (map != 0)) {
#ifdef __QSORT__
		for (uint32_t i = 1; i <= colors; i++) {
			map[i-1] = parse_color (xpm_data[i], chars);
		}
		qsort (map, colors, sizeof(struct color_code), qsort_cmp);
#else
		for (uint32_t i = 1; i <= colors; i++) {
			struct color_code ele = parse_color (xpm_data[i], chars);
			insert_ele (&ele, map, i-1);
		}
#endif
		uint16_t* p = xpm.image;
		for (uint32_t y = 0; y < xpm.height; y++) {
			const char* l = xpm_data[1 + colors + y];
			for (uint32_t x = 0; x < xpm.width; x++) {
				*p++ = get_color (map, colors, l, chars);
				l+=chars;
			}
		}

		free (map);
	}

	return xpm;
}
