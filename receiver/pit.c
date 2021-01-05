#include "MKL05Z4.h"
#include	"pit.h"



void PIT_Init(void)
{
	uint32_t tsv;
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;			// Włączenie sygnału zegara do modułu PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;				// Włączenie modułu PIT
	// wartość tsv do ustalenia mniej wiecej tyle, ile będzie przerwa w kolejnych znakach
	tsv=BUS_CLOCK/2;											// Przerwanie co 0.5s
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(tsv);		// Załadowanie wartości startowej
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;		// Odblokowanie przerwania i wystartowanie licznika
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}
