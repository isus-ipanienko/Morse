/*--------------------------------------------------------------------------------------------------------
Authors: Cezary Szczepa�ski, Pawe� Religa
Subject: Techinka Mikroprocesorowa 2
University: AGH Univerity of Science and Technology
Project: Morse code reciever with decoding
Section: Library for port initialization
---------------------------------------------------------------------------------------------------------*/



#include "MKL05Z4.h"
#include "port.h"

void port_Init(void){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTB_MASK;			// Pod��czenie zegara do PORTB
	PORTB->PCR[1] |= PORT_PCR_MUX(1);				// Ustawienie na portB na GPIO
	PORTB->PCR[1] |= PORT_PCR_IRQC(10);				// Ustawienie przerwa� na zbocze: 9 - rosnace, 10 - opadajace 12 - logic, 11 - oba
	NVIC_SetPriority(PORTB_IRQn, 2);				// Priorytet przerwa� 
	NVIC_ClearPendingIRQ(PORTB_IRQn);				
	NVIC_EnableIRQ(PORTB_IRQn);						// W��czenie przera� na portB
	
}
