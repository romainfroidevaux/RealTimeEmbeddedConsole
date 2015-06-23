/*******************************************************************************
 * FPGA-GPIO Handling - Header File
 *
 * This module provides services to display a value on the FPGA 7-segment.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#include "display.h"
#include "gpt.h"

///Refressh rate in ms
#define refresh_rate 10

/**
 * 7-segment display and LED interface
 */
struct fpga_regs {
	uint16_t reserved1[4];
	uint16_t seg7_rw;
	uint16_t seg7_ctrl;
	uint16_t seg7_id;
	uint16_t reserved2[1];
	uint16_t leds_rw;
	uint16_t leds_ctrl;
	uint16_t leds_id;
};

/**
 * The pointer to the FPGA interface
 */
static volatile struct fpga_regs* fpga = (struct fpga_regs*) 0xd6000000;
/* 7-segment: segment definition

 +-- seg A --+
 |           |
 seg F       seg B
 |           |
 +-- seg G --+
 |           |
 seg E       seg C
 |           |
 +-- seg D --+
 */

#define SEG_A 		0x008
#define SEG_B 		0x010
#define SEG_C 		0x020
#define SEG_D 		0x040
#define SEG_E 		0x080
#define SEG_F 		0x100
#define SEG_G 		0x200
#define SEG_DOT 	0x004
#define SEG_MINUS	SEG_G

static const uint16_t seg_7[] = {
	SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F, /* 0 */
	SEG_B + SEG_C, /* 1 */
	SEG_A + SEG_B + SEG_E + SEG_D + SEG_G, /* 2 */
	SEG_A + SEG_B + SEG_C + SEG_D + SEG_G, /* 3 */
	SEG_B + SEG_C + SEG_F + SEG_G, /* 4 */
	SEG_A + SEG_C + SEG_D + SEG_F + SEG_G, /* 5 */
	SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* 6 */
	SEG_A + SEG_B + SEG_C, /* 7 */
	SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* 8 */
	SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G, /* 9 */
	SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G, /* A */
	SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* b */
	SEG_A + SEG_D + SEG_E + SEG_F, /* C */
	SEG_B + SEG_C + SEG_D + SEG_E + SEG_G, /* d */
	SEG_A + SEG_D + SEG_E + SEG_F + SEG_G, /* E */
	SEG_A + SEG_E + SEG_F + SEG_G, /* F */
};

int timer = -1;
uint16_t units = 0;
uint16_t tens = 0;
uint16_t dot = 0;
bool display_running = false;

/**
 * Method to initialize the 7-segment display
 */
void display_init() {
	fpga->leds_rw = 0;
	fpga->leds_ctrl = 0xff;
	fpga->seg7_rw = 0;
	fpga->seg7_ctrl = 0x3ff;
}

static void refresh() {

	static bool alternate = true;

	if (alternate) {
		//fpga->seg7_rw = 0x2;
		fpga->seg7_rw = units + 0x1;
	} else {
		//fpga->seg7_rw = 0x1;
		fpga->seg7_rw = tens + 0x2 + dot;
	}

	alternate = !alternate;
}

void display_enable() {
	if (timer != -1)
		return;

	timer = gpt_timer_create(GPT_FREQ_MS, refresh, 0);

	gpt_timer_start_continuous(timer, refresh_rate);
}

void display_disable() {
	gpt_timer_destroy(timer);
	timer = -1;
}

/**
 * Method to display a decimal number on the 7-segment
 */
void display_set(int8_t value) {
	if (value < 0) {
		value = -value;
		dot = SEG_DOT;
	} else {
		dot = 0;
	}

	units = seg_7[value % 10];
	tens = seg_7[value / 10];
	if (value > 99) {
		units = SEG_MINUS;
		tens = SEG_MINUS;
	}
}

