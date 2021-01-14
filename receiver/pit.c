/*--------------------------------------------------------------------------------------------------------
Authors: Cezary Szczepański, Paweł Religa
Subcject: Techinka Mikroprocesorowa 2
University: AGH Univerity of Science and Technology
Project: Morse code reciever with decoding
Section: Library for PIT initialization
---------------------------------------------------------------------------------------------------------*/

#include "MKL05Z4.h"
#include	"pit.h"

#define TIME_UNIT 2477500									//	Stała czasowa wykorzystywana do transmisji i odbioru kodu Morse'a


void PIT_Init(void)
{
	uint32_t tsv;	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;						//	Włączenie sygnału zegara do modułu PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;							//	Włączenie modułu PIT
	
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(TIME_UNIT);		//	Czas między znakami
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	NVIC_SetPriority(PIT_IRQn, 3);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}
