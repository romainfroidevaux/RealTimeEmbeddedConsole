/**
 * Serial Interface Drivers - Source file
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 *
 *
 * @todo
 * - Disable the interrupt for incoming data when in place of a condition
 *   in interrupt service routine.
 * - Use Semaphore to wait a character
 *
 */

#include "serial.h"
#include "imx27_clock.h"
#include "kernel.h"
#include "interrupt.h"
#include "aitc.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

/* UART Receive Register (URXD) bit fields */
#define  URXD_ERR        (1<<14) /* Error detect */
#define  URXD_OVRRUN     (1<<13) /* Receiver overrun */
#define  URXD_FRMERR     (1<<12) /* Frame error */
#define  URXD_BRK        (1<<11) /* BREAK detect */
#define  URXD_PRERR      (1<<10) /* Parity error */

/* UART Control Register 1 (UCR1) bit fields */
#define  UCR1_ADEN       (1<<15) /* Auto dectect interrupt */
#define  UCR1_ADBR       (1<<14) /* Auto detect baud rate */
#define  UCR1_TRDYEN     (1<<13) /* Transmitter ready interrupt enable */
#define  UCR1_IDEN       (1<<12) /* Idle condition interrupt */
#define  UCR1_ICD_SHF	 10      /* Idle condition detect shift */
#define  UCR1_ICD_MSK    (3<<10) /* Idle condition detect mask */
#define  UCR1_RRDYEN     (1<<9)	 /* Recv ready interrupt enable */
#define  UCR1_RDMAEN     (1<<8)	 /* Recv ready DMA enable */
#define  UCR1_IREN       (1<<7)	 /* Infrared interface enable */
#define  UCR1_TXMPTYEN   (1<<6)	 /* Transimitter empty interrupt enable */
#define  UCR1_RTSDEN     (1<<5)	 /* RTS delta interrupt enable */
#define  UCR1_SNDBRK     (1<<4)	 /* Send break */
#define  UCR1_TDMAEN     (1<<3)	 /* Transmitter ready DMA enable */
#define  UCR1_DOZE       (1<<1)	 /* Doze */
#define  UCR1_UARTEN     (1<<0)	 /* UART enabled */

/* UART Control Register 2 (UCR2) bit fields */
#define  UCR2_ESCI     	 (1<<15) /* Escape seq interrupt enable */
#define  UCR2_IRTS  	 (1<<14) /* Ignore RTS pin */
#define  UCR2_CTSC  	 (1<<13) /* CTS pin control */
#define  UCR2_CTS        (1<<12) /* Clear to send */
#define  UCR2_ESCEN      (1<<11) /* Escape enable */
#define  UCR2_PREN       (1<<8)  /* Parity enable */
#define  UCR2_PROE       (1<<7)  /* Parity odd/even */
#define  UCR2_STPB       (1<<6)	 /* Stop */
#define  UCR2_WS         (1<<5)	 /* Word size */
#define  UCR2_RTSEN      (1<<4)	 /* Request to send interrupt enable */
#define  UCR2_ATEN	 (1<<3)  /* Aging Timer Enable */
#define  UCR2_TXEN       (1<<2)	 /* Transmitter enabled */
#define  UCR2_RXEN       (1<<1)	 /* Receiver enabled */
#define  UCR2_SRST 	 (1<<0)	 /* SW reset */

/* UART Control Register 3 (UCR3) bit fields */
#define  UCR3_PARERREN   (1<<12) /* Parity enable */
#define  UCR3_FRAERREN   (1<<11) /* Frame error interrupt enable */
#define  UCR3_ADNIMP  	 (1<<7)  /* Autobaud Detection Not Improved */
#define  UCR3_RXDSEN	 (1<<6)  /* Receive status interrupt enable */
#define  UCR3_AIRINTEN   (1<<5)  /* Async IR wake interrupt enable */
#define  UCR3_AWAKEN	 (1<<4)  /* Async wake interrupt enable */
#define  UCR3_RXDMUXSEL  (1<<2)  /* RXD Muxed input select (i.MX27) */
#define  UCR3_INVT  	 (1<<1)  /* Inverted Infrared transmission */
#define  UCR3_ACIEN  	 (1<<0)  /* Autobaud Counter Interrupt Enable */

/* UART Control Register 4 (UCR4) bit fields */
#define  UCR4_CTSTL_SHF  10      /* CTS trigger level shift */
#define  UCR4_CTSTL_MSK  (0x3f<<10) /* CTS trigger level mask */
#define  UCR4_INVR  	 (1<<9)  /* Inverted infrared reception */
#define  UCR4_ENIRI 	 (1<<8)  /* Serial infrared interrupt enable */
#define  UCR4_WKEN  	 (1<<7)  /* Wake interrupt enable */
#define  UCR4_IRSC  	 (1<<5)  /* IR special case */
#define  UCR4_LPBYP 	 (1<<4)  /* Low Power Bypass */
#define  UCR4_TCEN  	 (1<<3)  /* Transmit complete interrupt enable */
#define  UCR4_BKEN  	 (1<<2)  /* Break condition interrupt enable */
#define  UCR4_OREN  	 (1<<1)  /* Receiver overrun interrupt enable */
#define  UCR4_DREN  	 (1<<0)  /* Recv data ready interrupt enable */

/* UART Control Register Summary (UFCR) bit fields */
#define  UFCR_RXTL_SHF   0       /* Receiver trigger level shift */
#define  UFCR_RXTL_MSK   (0x3f<<0) /* Receiver trigger level shift */
#define  UFCR_RFDIV_SHF  7  	 /* Reference freq divider shift */
#define  UFCR_RFDIV_MSK  (7<<7)  /* Reference freq divider mask */
#define  UFCR_TXTL_SHF   10      /* Transmitter trigger level shift */
#define  UFCR_TXTL_MSK   (0x3f<<10) /* Transmitter trigger level shift */

/* UART Status Register 1 (USR1) bit fields */
// PCONFIG_IMX_SERIAL1arity error interrupt flag */
#define  USR1_PARITYERR  (1<<15)
#define  USR1_RTSS  	 (1<<14) /* RTS pin status */
#define  USR1_TRDY  	 (1<<13) /* Transmitter ready interrupt/dma flag */
#define  USR1_RTSD  	 (1<<12) /* RTS delta */
#define  USR1_ESCF  	 (1<<11) /* Escape seq interrupt flag */
#define  USR1_FRAMERR    (1<<10) /* Frame error interrupt flag */
#define  USR1_RRDY       (1<<9)	 /* Receiver ready interrupt/dma flag */
#define  USR1_AGTIM	 (1<<8)  /* Aging Timer Interrupt Flag */
#define  USR1_RXDS  	 (1<<6)	 /* Receiver idle interrupt flag */
#define  USR1_AIRINT	 (1<<5)	 /* Async IR wake interrupt flag */
#define  USR1_AWAKE 	 (1<<4)	 /* Aysnc wake interrupt flag */

/* UART Status Register 2 (USR2) bit fields */
#define  USR2_ADET  	 (1<<15) /* Auto baud rate detect complete */
#define  USR2_TXFE  	 (1<<14) /* Transmit buffer FIFO empty */
#define  USR2_IDLE  	 (1<<12) /* Idle condition */
#define  USR2_ACST       (1<<11) /* Autobaud Counter Stopped */
#define  USR2_IRINT 	 (1<<8)	 /* Serial infrared interrupt flag */
#define  USR2_WAKE  	 (1<<7)	 /* Wake */
#define  USR2_RTSF  	 (1<<4)	 /* RTS edge interrupt flag */
#define  USR2_TXDC  	 (1<<3)	 /* Transmitter complete */
#define  USR2_BRCD  	 (1<<2)	 /* Break condition */
#define  USR2_ORE        (1<<1)	 /* Overrun error */
#define  USR2_RDR        (1<<0)	 /* Recv data ready */

/* UART Test Register (UTS) bit fields */
#define  UTS_FRCPERR	 (1<<13) /* Force parity error */
#define  UTS_LOOP        (1<<12) /* Loop tx and rx */
#define  UTS_DBGEN  	 (1<<11) /* Debug Enable b */
#define  UTS_LOOPIR  	 (1<<10) /* Loop TX and RX for IR Test */
#define  UTS_RXDGB       (1<<9)  /* RX fifo debug mode */
#define  UTS_TXEMPTY	 (1<<6)	 /* TxFIFO empty */
#define  UTS_RXEMPTY	 (1<<5)	 /* RxFIFO empty */
#define  UTS_TXFULL 	 (1<<4)	 /* TxFIFO full */
#define  UTS_RXFULL 	 (1<<3)	 /* RxFIFO full */
#define  UTS_SOFTRST	 (1<<0)	 /* Software reset */

#define BUFFER_SIZE	128

struct uart_ctrl {
	uint32_t urxd;
	uint32_t reseverd1[(0x40-0x04)/4];
	uint32_t utxd;
	uint32_t reseverd2[(0x80-0x44)/4];
	uint32_t ucr1;
	uint32_t ucr2;
	uint32_t ucr3;
	uint32_t ucr4;
	uint32_t ufcr;
	uint32_t usr1;
	uint32_t usr2;
	uint32_t uesc;
	uint32_t utim;
	uint32_t ubir;
	uint32_t ubmr;
	uint32_t ubrc;
	uint32_t onems;
	uint32_t uts;
};

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/

/* UART3 */
static volatile struct uart_ctrl* uart = (struct uart_ctrl*)0x1000c000;


/**
 * Incoming data active.
 */
static bool rx_enabled = true;

static struct ring_buffer {
	volatile unsigned int in;
	volatile unsigned int out;
	volatile char buffer[BUFFER_SIZE];
} rx, tx;

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

static uint32_t serial_reffreq()
{
	uint32_t rfdiv;

	rfdiv = (uart->ufcr >> 7) & 7;
	rfdiv = rfdiv < 6 ? 6 - rfdiv : 7;

	return imx27_clock_get_perclk1() / rfdiv;
}

/**
 * Interrupt service routine.
 *
 * This function is called when an interrupt occur.
 *
 * @param param an optional param no used for this routine
 */

static void serial_interrupt_service_routine (void* param __attribute__((unused)))
{
	if ((uart->usr1 & USR1_RRDY) != 0) {
		char data = uart->urxd;
		if (rx_enabled && ((rx.in - rx.out) % BUFFER_SIZE) < (BUFFER_SIZE-1))	{
			rx.buffer[rx.in] = data;
			rx.in++;
			if (rx.in >= BUFFER_SIZE) rx.in = 0;
		}
	}

	if ((uart->usr1 & USR1_TRDY) != 0) {
		if (tx.in != tx.out) {
			uart->utxd = tx.buffer[tx.out];
			tx.out++;
			if (tx.out >= BUFFER_SIZE) tx.out = 0;
		}
		if (tx.in == tx.out) uart->ucr1 &= ~UCR1_TRDYEN;
	}
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void serial_init()
{
	/* initialize the I/O buffers */
	rx.in = 0;
	rx.out = 0;
	tx.in = 0;
	tx.out = 0;

	/* initialize the controller */
	uart->ucr2 = 0; /* reset the uart */
	uart->ucr1 = 0;
	uart->ucr2 = UCR2_SRST | UCR2_WS | UCR2_IRTS;
	uart->ucr3 = UCR3_RXDMUXSEL;
	uart->ucr4 = (0 << UCR4_CTSTL_SHF);
	uart->uesc = 0x0;
	uart->utim = 0;
	uart->ubir = 0;
	uart->ubmr = 0;
	uart->uts  = 0;

	/* Configure FIFOs and reference clock divider */
	uart->ufcr = (2 << UFCR_TXTL_SHF) + (0x5 << UFCR_RFDIV_SHF)
			+ (1 << UFCR_RXTL_SHF);

	/* Set the one millisecond register */
	uint32_t reffreq = serial_reffreq();
	uart->onems = reffreq / 1000;

	/* Set default baudrate generator */
	serial_set_baudrate(9600);

	/* Enable TX & RX FIFOs */
	uart->ucr2 |= UCR2_SRST | UCR2_RXEN | UCR2_TXEN;

  	/* Clear all status flags */
	uart->usr2 = -1;
	uart->usr1 = -1;

	/* Attach interrupt service routine to aitc */
	aitc_attach (AITC_UART3, AITC_IRQ, serial_interrupt_service_routine, 0);

	/* Enable the UART & RX interrupt handling */
	uart->ucr1 |= UCR1_UARTEN | UCR1_RRDYEN;
}


void serial_putc(char c)
{
// former code
//	/* Wait until TX FIFO ready */
//	while ((uart->usr1 & USR1_TRDY) == 0);
//	uart->utxd = c;

	while (((tx.in - tx.out) % BUFFER_SIZE) >= (BUFFER_SIZE-1));
	tx.buffer[tx.in] = c;
	tx.in++;
	if (tx.in >= BUFFER_SIZE) tx.in = 0;
	interrupt_disable();
	uart->ucr1 |= UCR1_TRDYEN;
	interrupt_enable();
}

bool serial_tstc()
{
// former code
//	/* If receive fifo is empty, return false */
//	return ((uart->usr2 & USR2_RDR) != 0);
	return rx.in != rx.out;
}


int serial_getc()
{
// former code
//	// Wait unitl RX FIFO ready (characters have been received) */
//	while ((uart->usr2 & USR2_RDR) == 0);
//	unsigned char data = uart->urxd;
	
	while (rx.in == rx.out);
	unsigned char data = rx.buffer[rx.out];
	rx.out++;
	if (rx.out >= BUFFER_SIZE) rx.out = 0;

	return data;
}


void serial_puts (const char *s)
{
	while (*s) serial_putc (*s++);
}

void serial_set_baudrate (uint32_t baudrate)
{
	/* disable UART */
	uint32_t backup = uart->ucr1;
	uart->ucr1 &= ~UCR1_UARTEN;

	/* Set the numerator value minus one of the BRM ratio */
	uart->ubir = (baudrate / 100) - 1;

	/* Set the denominator value minus one of the BRM ratio    */
	uart->ubmr = (serial_reffreq() / (16*100)) - 1;

	/* enable UART */
	uart->ucr1 = backup;
}

void serial_stop_rx() {
	rx_enabled = false;
}

void serial_start_rx() {
	rx_enabled = true;
}

