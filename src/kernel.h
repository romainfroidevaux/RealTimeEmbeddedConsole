/**
 * Micro Kernel - Header File
 *
 * Introduction to Operating System implementation.
 * Definition of services for a micro kernel.
 *
 * @author Daniel Gacher <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 */

#pragma once
#ifndef KERNEL_H
#define KERNEL_H

typedef void (*kernel_thread_t) (void* param);

/**
 * Initialize the ressources of the kernel module.
 */
extern void kernel_init();

/**
 * Launch the kernel main thread.
 */
extern void kernel_launch();

/**
 * Passes control to another thread.
 */
extern void kernel_reschedule();

/**
 * Create a new thread.
 *
 * @param thread the routine of the thread
 * @param param the param to pass to the thread routine
 * @return -1 on error, 0 on success
 */
extern int  kernel_thread_create (kernel_thread_t thread, void* param);

/**
 * Exit the current thread.
 */
extern void kernel_thread_exit();

/**
 * Passes controle to another thread and sleep during for specified time.
 *
 * @param ms the time in ms to sleep
 * @return 0 on success, -1 if the thread can not sleep
 */
extern int  kernel_thread_sleep(int ms);

/**
 * Create a new semaphore. 
 *
 * @param initial_value the initial value for the semaphore
 * @return the id of the semaphore, -1 on error
 */
extern int  kernel_sema_create (int initial_value);

/**
 * Destroy an existant semaphore.
 *
 * @param sema_id the id of the semaphore to destroy
 * @return 0 on success, -1 on error
 */
extern int  kernel_sema_destroy (int sema_id);
/** 
 * Block the thread until one is available.
 *
 * @param sema_id the id of the semaphore to take
 * @return 0 on success, -1 on error
 */
extern int  kernel_sema_wait (int sema_id);

/**
 * Signal that one is available.
 *
 * @param sema_id the id of the semaphore to signal
 * @return 0 on success, -1 on error
 */
extern int  kernel_sema_signal (int sema_id);

extern int kernel_sema_signal_all(int sema_id, int initial_value);

#endif
