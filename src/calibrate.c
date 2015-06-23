/*******************************************************************************
 * Calibrate - Source file
 *
 * Calibration application for the touch screen and API for coordinate
 * conversion.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "calibrate.h"
#include "utils.h"
#include "tsc2101.h"
#include "lcddisplay.h"
#include "lcdc.h"
#include "kernel.h"
#include "view_calibrate.h"
#include "touchmanager.h"
#include "model.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

typedef struct {
	int x[5], xfb[5];
	int y[5], yfb[5];
} calibration;

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/

static bool run = false;

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

static void print_config(int config[7]) {
	printf("int config[7] = {");
	printf(" %d", config[0]);
	for (int i = 1; i < 7; i++) {
		printf(", %d", config[i]);
	}
	printf(" };\n\r");
}

static int perform_calibration(calibration *cal) {
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	int config[7];

	// Get sums for matrix
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

	// Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		printf("ts_calibrate: determinant is too small -- %f\n",det);
		return 0;
	}

	// Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

	// Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j]*cal->x[j]);
		zy += (float)(cal->xfb[j]*cal->y[j]);
	}

	// Now multiply out to get the calibration for framebuffer x coord
	config[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	config[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	config[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

	// Get sums for y calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j]*cal->x[j]);
		zy += (float)(cal->yfb[j]*cal->y[j]);
	}

	// Now multiply out to get the calibration for framebuffer y coord
	config[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	config[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	config[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

	// If we got here, we're OK, so assign scaling to a[6] and return
	config[6] = (int)scaling;

	touchmanager_set_config(config);
	print_config(config);
	return 1;
}

static void put_cross(int x, int y) {
	struct utils_position position = { .x=x, .y=y };
	view_calibrate_set_cross_position(position);
}

static void getxy(int *x, int *y) {
	struct utils_position p = touchmanager_wait_release(false);
	(*x) = p.x;
	(*y) = p.y;
}

static void get_sample (calibration *cal,
			int index, int x, int y, char *name)
{
	static int last_x = -1, last_y;

	if (last_x != -1) {
	#define NR_STEPS 10
		int dx = ((x - last_x) << 16) / NR_STEPS;
		int dy = ((y - last_y) << 16) / NR_STEPS;
		int i;
		last_x <<= 16;
		last_y <<= 16;
		for (i = 0; i < NR_STEPS; i++) {
			put_cross (last_x >> 16, last_y >> 16);
			kernel_thread_sleep(100);
			//put_cross (last_x >> 16, last_y >> 16);
			last_x += dx;
			last_y += dy;
		}
	}

	put_cross(x, y);
	getxy (&cal->x [index], &cal->y [index]);
	put_cross(x, y);

	last_x = cal->xfb [index] = x;
	last_y = cal->yfb [index] = y;

	printf("%s : X = %4d Y = %4d\n", name, cal->x [index], cal->y [index]);
}

static void thread(__attribute__ ((unused)) void* param) 
{
	run = true;
	
	bool touchmanager_run = touchmanager_is_running();

	if(touchmanager_run) {
		touchmanager_thread_stop();
	}

	calibration cal;
	bool success = false;
	
	view_calibrate_init();
	while(!success && run) {
	
		get_sample (&cal, 0, 50,        50,        "Top left");
		get_sample (&cal, 1, LCD_WIDTH - 50, 50,        "Top right");
		get_sample (&cal, 2, LCD_WIDTH - 50, LCD_HEIGHT - 50, "Bot right");
		get_sample (&cal, 3, 50,        LCD_HEIGHT - 50, "Bot left");
		get_sample (&cal, 4, LCD_WIDTH / 2,  LCD_HEIGHT / 2,  "Center");
		lcddisplay_init();

		if (perform_calibration(&cal)) {
			success = true;
		} else {
			printf("Calibration failed.\n");
		}
	}
	calibrate_thread_stop();

	if(touchmanager_run) {
		touchmanager_thread_start();
	}

	kernel_thread_exit();
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void calibrate_thread_start() {
	if (run) return;
	kernel_thread_create(thread, 0);
	model_start_app(app_calibrate);
}

void calibrate_thread_stop() {
	run = false;
	model_stop_app(app_calibrate);
}
