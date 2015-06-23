#pragma once
#ifndef DMAC_H
#define DMAC_H
/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: ARM Direct Memory Access Controller
 *
 * Purpose:
 *   Module to deal with the DMA controller (DMAC) of the i.MX27.
 *
 * Author: 	Daniel Gachet
 * Date: 	14.02.2014
 */


/**
 * List of available DMA channels
 */
enum dmac_channels {
	DMAC_CHANNEL_0,  DMAC_CHANNEL_1,  DMAC_CHANNEL_2,  DMAC_CHANNEL_3,
	DMAC_CHANNEL_4,  DMAC_CHANNEL_5,  DMAC_CHANNEL_6,  DMAC_CHANNEL_7,
	DMAC_CHANNEL_8,  DMAC_CHANNEL_9,  DMAC_CHANNEL_10, DMAC_CHANNEL_11,
	DMAC_CHANNEL_12, DMAC_CHANNEL_13, DMAC_CHANNEL_14, DMAC_CHANNEL_15,
	DMAC_NB_OF_CHANNELS};

/**
 * DMA transfer direction for FIFO to memory transfers
 */
enum dmac_directions {DMAC_TRANSMIT,	/** transfer data from memory to FIFO */
		      DMAC_RECEIVE};	/** transfer data from FIFO to memory */

/**
 * Memory data bus width
 */
enum dmac_buswidth {DMAC_8BIT, DMAC_16BIT, DMAC_32BIT};

/**
 * DMA transfer status
 */
enum dmac_transfer_status {
	DMAC_SUCCESS, 		/** transfer performed with success */
	DMAC_BURST_TIMEOUT, 	/** burst transfer could not occurred within specified number of burst cycle */
	DMAC_REQUEST_TIMEOUT,	/** request time-out occurred during transfer */
	DMAC_TRANSFER_TIMEOUT, 	/** error occurred during transfer */
	DMAC_BUFFER_OVERFLOW};	/** DMA internal FIFO buffer overflow occurred during transfer */


/**
 * Prototype of the application specific interrupt service routine
 * @param handle application specific parameter (used for OO-design)
 * @param status DMA transfer status
 * @param transferred number of bytes transferred
 */
typedef void (*dmac_isr_t) (void* handle, enum dmac_transfer_status status, uint32_t transferred);


/**
 * initialization method
 * should be called prior to use any other method of this module
 */
extern void dmac_init();


/**
 * method to attach an application specific interrupt service routine to the specified DMA channel
 *
 * @param channel DMA channel number to which the application ISR should be attached
 * @param routine application specific interrupt service routine
 * @param handle application specific parameter passed to the ISR during call
 *
 * @return execution status: 0 <=> success, -1 <=> error
 */
extern int dmac_attach(enum dmac_channels channel, dmac_isr_t routine, void* handle);


/**
 * method to detach application specific interrupt service routine from the specified DMA channel
 *
 * @param channel DMA channel number from which the application ISR should be detached
 *
 */
extern void dmac_detach(enum dmac_channels channel);


/**
 * method to copy a chunk of memory from one place to another.
 * The memory areas should not overlap.
 *
 * @param channel DMA channel to use for the transfer
 * @param dst_addr destination address
 * @param src_addr source address
 * @param size number of bytes to be copied
 * @return execution status (0 <=> success, -1 <=> error
 */
extern int dmac_memmov (enum dmac_channels channel,
			void* dst_addr,
                        const void* src_addr,
			uint32_t size);


#endif
