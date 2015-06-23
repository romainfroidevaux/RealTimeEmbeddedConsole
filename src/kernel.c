/**
 * Micro Kernel - Header File
 *
 * @author Daniel Gacher <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "kernel.h"
#include "interrupt.h"
#include "gpt.h"

/* task implementation... */
#define NB_OF_THREADS 50
#define STACK_SIZE 0x8000

enum states {TERMINATED, RUNNING, WAITING, BLOCKED};
struct tcb {
	uint32_t regs[15];
	uint32_t psr;
	enum states state;

	struct tcb* t_next;	// used to chain all tcb
	struct tcb* s_next;	// used to chain blocked threads within a sema

	uint32_t stack[STACK_SIZE];
};
struct tcb tcb[NB_OF_THREADS];
struct tcb* running_thread;

static void kernel_startup (void* param, kernel_thread_t thread)
{
	thread (param);
	kernel_thread_exit();
}

static void kernel_thread_init()
{
	for (int i=0; i<NB_OF_THREADS; i++) {
		tcb[i].state = TERMINATED;
		tcb[i].t_next = &tcb[i+1];
	} 
	tcb[NB_OF_THREADS-1].t_next = &tcb[0];
	tcb[0].state = RUNNING;
	running_thread = &tcb[0];
}												   		
	  
void kernel_thread_exit()
{
	running_thread->state = TERMINATED;
	kernel_reschedule();
}


/* forward declaration of low level transfer routine...
 * this method will be implemented in assembler...
 */
extern void kernel_transfer(struct tcb* former, uint32_t psr, struct tcb* new);

void kernel_reschedule()
{
	struct tcb* former_thread = running_thread;
	struct tcb* new_thread = former_thread->t_next;

	interrupt_enable();
	if (former_thread->state == RUNNING) former_thread->state = WAITING;
	while (new_thread->state != WAITING) new_thread = new_thread->t_next;

	running_thread = new_thread;
	new_thread->state = RUNNING;
	kernel_transfer(former_thread, 0x13, new_thread);
}

void kernel_launch()
{
	while(1) {
		kernel_reschedule();
	}
}

int kernel_thread_create (kernel_thread_t thread, void* handle)
{
	int status = -1;
	int i=0;
	while ((i < NB_OF_THREADS) && (tcb[i].state != TERMINATED)) i++;

	if (i < NB_OF_THREADS) {
		memset (&tcb[i].regs[0], 0, sizeof(tcb[i].regs));
		tcb[i].regs[0] = (uint32_t)handle;
		tcb[i].regs[1] = (uint32_t)thread;
		tcb[i].regs[13] = (uint32_t)&tcb[i].stack[STACK_SIZE];
		tcb[i].regs[14] = (uint32_t)kernel_startup;
		tcb[i].psr = 0x13;
		tcb[i].state = WAITING;

		status = 0;
	}

	return status;
}

static void kernel_thread_wakeup_isr(void* param) {
	struct tcb* thread = (struct tcb*) param;
	thread->state = WAITING;
}

int kernel_thread_sleep(int ms) {
	int timer = gpt_timer_create(GPT_FREQ_MS, kernel_thread_wakeup_isr, running_thread);

	//Postcondition
	if(timer == -1) return -1;

	//Postcondition
	if(gpt_timer_start_oneshot(timer, ms) == -1)
		return -1;

	running_thread->state = BLOCKED;
	kernel_reschedule();
	gpt_timer_destroy(timer);
	return 0;
}

/* semaphore implementation... */
#define NB_OF_SEMAS 200

struct sema {
	int32_t count;
	bool is_used;
	struct tcb* s_next;
};
struct sema sema[NB_OF_SEMAS];


static void kernel_sema_init()
{
	for (int i=0; i<NB_OF_SEMAS; i++) {
		sema[i].count = 0;
		sema[i].is_used = false;
		sema[i].s_next = 0;
	}
}

int kernel_sema_create (int initial_value)
{
	int sema_id = -1;
	int i = 0;

	while ((i < NB_OF_SEMAS) && (sema[i].is_used)) i++;

	if (i < NB_OF_SEMAS) {
		sema_id = i;
		sema[i].count = initial_value;
		sema[i].is_used = true;
		sema[i].s_next = 0;
	}

	return sema_id;
}

int kernel_sema_destroy (int sema_id)
{
	if (((unsigned)sema_id >= NB_OF_SEMAS) || (sema[sema_id].s_next != 0)) 
		return -1;
	
	sema[sema_id].is_used = false;
	return 0;
}


int kernel_sema_wait (int sema_id)
{
	if (((unsigned)sema_id >= NB_OF_SEMAS) || (!sema[sema_id].is_used)) 
		return -1;

	interrupt_disable();
	sema[sema_id].count--;
	if (sema[sema_id].count < 0) {
		if (sema[sema_id].s_next == 0) {
			sema[sema_id].s_next = running_thread;
		} else {
			struct tcb* next = sema[sema_id].s_next;
			while (next->s_next != 0) next = next->s_next;
			next->s_next = running_thread;
		}
		running_thread->s_next = 0;
		running_thread->state = BLOCKED;
		kernel_reschedule();
	}
	interrupt_enable();
	return 0;
}

int kernel_sema_signal (int sema_id)
{
	if (((unsigned)sema_id >= NB_OF_SEMAS) || (!sema[sema_id].is_used)) 
		return -1;
	
	interrupt_disable();
	sema[sema_id].count++;
	if (sema[sema_id].s_next != 0) {
		sema[sema_id].s_next->state = WAITING;
		sema[sema_id].s_next = sema[sema_id].s_next->s_next;
	}
	interrupt_enable();
	return 0;
}

int kernel_sema_signal_all(int sema_id, int initial_value)
{
	if (((unsigned)sema_id >= NB_OF_SEMAS) || (!sema[sema_id].is_used)) 
		return -1;
	
	interrupt_disable();
	sema[sema_id].count = initial_value;
	
	while (sema[sema_id].s_next != 0) {
		sema[sema_id].s_next->state = WAITING;
		sema[sema_id].s_next = sema[sema_id].s_next->s_next;
	}
	
	interrupt_enable();
	return 0;
}

/* micro kernel initialization...*/
void kernel_init()
{
	printf("Init: kernel\n\r");

	kernel_thread_init();
	kernel_sema_init();
}
