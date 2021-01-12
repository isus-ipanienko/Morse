#include "MKL05Z4.h"
#include "port.h"

void port_Init(void){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(1);
	PORTB->PCR[1] |= PORT_PCR_IRQC(11); // 9 - rosnace, 10 - opadajace 12 - logic, 11 - oba
	NVIC_ClearPendingIRQ(PORTB_IRQn);				/* Clear NVIC any pending interrupts on PORTC_B */
	NVIC_EnableIRQ(PORTB_IRQn);
	
}
