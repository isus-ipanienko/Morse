/*--------------------------------------------------------------------------------------------------------
Authors: Cezary Szczepañski, Pawe³ Religa
Subcject: Techinka Mikroprocesorowa 2
University: AGH Univerity of Science and Technology
Project: Morse code reciever with decoding
Section: Library for port initialization
---------------------------------------------------------------------------------------------------------*/



#include "MKL05Z4.h"
#include "port.h"

void port_Init(void){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(1);
	PORTB->PCR[1] |= PORT_PCR_IRQC(10);				// 9 - rosnace, 10 - opadajace 12 - logic, 11 - oba
	NVIC_SetPriority(PORTB_IRQn, 2);
	NVIC_ClearPendingIRQ(PORTB_IRQn);				
	NVIC_EnableIRQ(PORTB_IRQn);
	
}
