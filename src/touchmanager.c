/*******************************************************************************
 * Touch Manager - Source file
 *
 * This module provide services for the application view.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "touchmanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"
#include "tsc2101.h"
#include "kernel.h"
#include "calibrate.h"
#include "lcdc.h"
#include "model.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

#define PRESSURE_DIF 30     /**< Minimum pressure difference for detection */
#define PRESSURE_MAX 800    /**< Maximum pressure value */
#define POLL_DELAY 40       /**< Delay before to retrieve the next position */
#define RELEASE_DELAY 1000  /**< Delay before the next release */
#define BUTTON_PADDING 10   /**< Margin arround the button for the touch surface */
#define NB_SAMPLES 5        /**< The number of sampler for the noise correction */
#define BUTTONS_LEN 50      /**< The number of buttons for the memory preallocation */
#define GRIDS_LEN 10        /**< The number of grid for the memory preallocation */

/**
 * The grid.
 */
struct grid {
	struct utils_position tl;
	struct utils_position br;
	struct utils_size size;
	touchmanager_grid_routine_t routine;
	void* param;
};

/**
 * The button.
 */
struct button {
	struct utils_position tl;
	struct utils_position br;
	touchmanager_button_routine_t routine;
	void* param;
};

/**
 * A 3D position structure.
 */
struct position {
	int x;
	int y;
	int z;
};

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/

/**
 * The array with all buttons.
 */
static struct button buttons[BUTTONS_LEN];

/**
 * The array with all grids
 */
static struct grid grids[GRIDS_LEN];

/**
 * The state of the application thread.
 */
static bool run = false;

/**
 * The calibration configuration.
 */
static int config[7] = { -47992, 54144, -470, 33587728, -245, -33826, 65536 };

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

/**
 * The compare function for two integer
 * @param a the first int to compare
 * @param b the second int to compare
 * @return the difference, 0 when identical
 */
static int cmpint(const void * a, const void * b) {
	return (*(int*) a - *(int*) b);
}

/**
 * Get a clean position values
 *
 * @param calibrated if true get the calibrated position, else the raw position
 */
static struct position get_position(bool calibrated) {
	struct position r;
	struct tsc2101_position p;

	//Take N samples and get the median value
	int x[NB_SAMPLES];
	int y[NB_SAMPLES];
	int z[NB_SAMPLES];

	for (int i = 0; i < NB_SAMPLES; i++) {
		do {
			p = tsc2101_read_position();
		} while (p.z > PRESSURE_MAX); // Exclude too big pressure
		x[i] = p.x;
		y[i] = p.y;
		z[i] = p.z;
	}
	qsort(x, NB_SAMPLES, sizeof(x[0]), cmpint);
	qsort(y, NB_SAMPLES, sizeof(y[0]), cmpint);
	qsort(z, NB_SAMPLES, sizeof(z[0]), cmpint);

	r.x = x[NB_SAMPLES / 2];
	r.y = y[NB_SAMPLES / 2];
	r.z = z[NB_SAMPLES / 2];

	//printf("Position {%i, %i, %i}\n\r", r.x, r.y, r.z);

	if (calibrated) {
		r.x = (config[0] + config[1] * r.x + config[2] * r.y) / config[6];
		r.y = (config[3] + config[4] * r.x + config[5] * r.y) / config[6];
	}

	return r;
}

/**
 * Test if a position is in a rectangle.
 * @param tl the top left position of the rectangle
 * @param br the bottom right position of the rectangle
 * @param p the position to test
 * @return true if the position is in the rectangle, else false
 */
static bool position_is_in(struct utils_position tl, struct utils_position br, struct utils_position p) {
	return (p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y);
}

/**
 * Processing a position.
 *
 * Find the good routine to callback if a element exist at the specified position.
 *
 * @param position the possition to process
 */
static void process_position(struct utils_position position) {
	int i;

	printf("Position {%i, %i}\n\r", position.x, position.y);

	for (i = 0; i < GRIDS_LEN; i++) {

		if (grids[i].routine != 0 && position_is_in(grids[i].tl, grids[i].br, position)) {

			position.x -= grids[i].tl.x;
			position.y -= grids[i].tl.y;

			position.x /= grids[i].size.w;
			position.y /= grids[i].size.h;

			grids[i].routine(position, grids[i].param);
			return;
		}
	}

	for (i = 0; i < BUTTONS_LEN; i++) {
		if (buttons[i].routine != 0 && position_is_in(buttons[i].tl, buttons[i].br, position)) {
			buttons[i].routine(buttons[i].param);
			return;
		}
	}
}

/**
 * Thread for the Touch Manager
 */
static void thread() {

	run = true;
	struct utils_position pos;

	while (run) {
		pos = touchmanager_wait_release(true);
		process_position(pos);
		//kernel_thread_sleep(RELEASE_DELAY);
		kernel_reschedule();
	}
	kernel_thread_exit();
}

/**
 * Add a grid to the array.
 *
 * @param grid the grid to add
 * @return the index position, -1 on error
 */
static inline int add_grid(struct grid grid) {
	for (int i = 0; i < GRIDS_LEN; i++) {
		if (grids[i].routine != 0)
			continue;

		grids[i] = grid;
		return i;
	}
	return -1;
}

/**
 * Add a button to the array.
 *
 * @param grid the grid to add
 * @return the index position, -1 on error
 */
static inline int add_button(struct button button) {
	for (int i = 0; i < GRIDS_LEN; i++) {
		if (buttons[i].routine != 0)
			continue;

		buttons[i] = button;
		return i;
	}
	return -1;
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

struct utils_position touchmanager_wait_release(bool calibrated) {
	struct position p;
	struct utils_position r = { .x = 0, .y = 0 };
	struct position f_p = { .x = -1, .y = -1, .z = -1, };
	int iter = 0;

	while (true) {
		p = get_position(calibrated);
		if ((p.x == f_p.x) && (p.y == f_p.y) && (p.z == f_p.z)) {
			if (iter >= 2) {
				r.x = p.x;
				r.y = p.y;
				break;
			}
		} else {
			if (abs(p.z - f_p.z) > PRESSURE_DIF)
				iter++;
			f_p = p;
		}
		//kernel_thread_sleep(POLL_DELAY);
		kernel_reschedule();
	}
	return r;
}

void touchmanager_set_config(int conf[7]) {
	for (int i = 0; i < 7; i++)
		config[i] = conf[i];
}

int touchmanager_init() {
	touchmanager_detach_all();

	return 0;
}

void touchmanager_thread_start() {
	//Precondition
	if (run) return;
	kernel_thread_create(thread, 0);
	model_start_app(app_touchmanager);
}

void touchmanager_thread_stop() {
	run = false;
	model_stop_app(app_touchmanager);
}

int touchmanager_grid_attach(
		struct utils_position position,
		struct utils_size size,
		struct utils_size dimension,
		touchmanager_grid_routine_t routine,
		void* param)
{
	//Precondition
	if (routine == 0 || dimension.w == 0 || dimension.h == 0)
		return -1;

	struct grid g;

	g.size.w = size.w / dimension.w;
	g.size.h = size.h / dimension.h;
	g.tl = position;
	g.br.x = position.x + size.w - 1;
	g.br.y = position.y + size.h - 1;
	g.routine = routine;
	g.param = param;

	return add_grid(g);
}

int touchmanager_grid_detach(int index)
{
	//Precondition
	if ((unsigned) index >= GRIDS_LEN) return -1;

	grids[index].routine = 0;

	return 0;
}

void touchmanager_grid_detach_all() {
	for (int i = 0; i < GRIDS_LEN; i++) {
		grids[i].routine = 0;
	}
}

int touchmanager_button_attach(
		struct utils_position position,
		struct utils_size size,
		touchmanager_button_routine_t routine,
		void* param)
{
	//Precondition
	if (routine == 0) return -1;

	struct button b;

	b.tl = position;
	b.br.x = position.x + size.w - 1;
	b.br.y = position.y + size.h - 1;
	b.routine = routine;
	b.param = param;

	return add_button(b);
}

int touchmanager_button_detach(int index) {
	if ((unsigned) index >= BUTTONS_LEN) return -1;

	buttons[index].routine = 0;

	return 0;
}

void touchmanager_button_detach_all() {
	for (int i = 0; i < BUTTONS_LEN; i++) {
		buttons[i].routine = 0;
	}
}

void touchmanager_detach_all() {
	touchmanager_button_detach_all();
	touchmanager_grid_detach_all();
}

bool touchmanager_is_running() {
	return run;
}
