/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: 	ARM Exception Handling 
 * Author: 	Daniel Gachet / EIA-FR / Telecoms
 * Date: 	29.01.2014
 */

#include <stdio.h>

#include "interrupt.h"
#include "stdbool.h"

static void exception_handler(void* addr, enum interrupt_vectors vector, void* param)
{
	printf ("ARM Exception(%d): %s at address: %p\n", vector, (char*)param, addr);
	while (vector == INT_PREFETCH);
}

void exception_init()
{
	printf("Init: exception\n\r");

	interrupt_attach (INT_UNDEF,    exception_handler, "undefined instruction");
	interrupt_attach (INT_SWI,      exception_handler, "software interrupt");
	interrupt_attach (INT_PREFETCH, exception_handler, "prefetch abort");
	interrupt_attach (INT_DATA,     exception_handler, "data abort");
}



