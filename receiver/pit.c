#include "MKL05Z4.h"
#include	"pit.h"

#define TIME_UNIT 2477500


void PIT_Init(void)
{
	uint32_t tsv;
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;			// Włączenie sygnału zegara do modułu PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;				// Włączenie modułu PIT
	
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(TIME_UNIT);	  //czas między znakami
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	//PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(TIME_UNIT*3);	//czas między znakami
	//PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	//PIT->CHANNEL[2].LDVAL = PIT_LDVAL_TSV(TIME_UNIT*7);	//czas między znakami
	//PIT->CHANNEL[2].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	NVIC_SetPriority(PIT_IRQn, 3);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}
