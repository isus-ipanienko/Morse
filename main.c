/*-------------------------------------------------------------------------------------
					Technika Mikroprocesorowa 2 - projekt
					Kod Morse'a - nadajnik
					autor: Pawel Religa, Cezary Szczepanski
					wersja: 13.12.2020r.
----------------------------------------------------------------------------*/
 
#include "frdm_bsp.h" 
#include "tpm_pcm.h"
#include "uart0.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CYCLE 100
#define CHARACTER_LIMIT 64
#define LF	0xa //newline
// Private prototypes
void UART0_IRQHandler(void);
void Morse_Encoder(char* str);
// Private memory declarations
static const char morse_map[36][5] = {
																		{'-', '-', '-', '-', '-'}, //0
																		{'.', '-', '-', '-', '-'}, //1
																		{'.', '.', '-', '-', '-'}, //2
																		{'.', '.', '.', '-', '-'}, //3
																		{'.', '.', '.', '.', '-'}, //4
																		{'.', '.', '.', '.', '.'}, //5
																		{'-', '.', '.', '.', '.'}, //6
																		{'-', '-', '.', '.', '.'}, //7
																		{'-', '-', '-', '.', '.'}, //8
																		{'-', '-', '-', '-', '.'}, //9 offset 48
																		{'.', '-', '0', '0', '0'}, //A offset 55
																		{'-', '.', '.', '.', '0'}, //B
																		{'-', '.', '-', '.', '0'}, //C
																		{'-', '.', '.', '0', '0'}, //D
																		{'.', '0', '0', '0', '0'}, //E
																		{'.', '.', '-', '.', '0'}, //F
																		{'-', '-', '.', '0', '0'}, //G
																		{'.', '.', '.', '.', '0'}, //H
																		{'.', '.', '0', '0', '0'}, //I
																		{'.', '-', '-', '-', '0'}, //J
																		{'-', '.', '-', '0', '0'}, //K
																		{'.', '-', '.', '.', '0'}, //L
																		{'-', '-', '0', '0', '0'}, //M
																		{'-', '.', '0', '0', '0'}, //N
																		{'-', '-', '-', '0', '0'}, //O
																		{'.', '-', '-', '.', '0'}, //P
																		{'-', '-', '.', '-', '0'}, //Q
																		{'.', '-', '.', '0', '0'}, //R
																		{'.', '.', '.', '0', '0'}, //S
																		{'-', '0', '0', '0', '0'}, //T
																		{'.', '.', '-', '0', '0'}, //U
																		{'.', '.', '.', '-', '0'}, //V
																		{'.', '-', '-', '0', '0'}, //W
																		{'-', '.', '.', '-', '0'}, //X
																		{'-', '.', '-', '-', '0'}, //Y
																		{'-', '-', '.', '.', '0'}, //Z 
																		};
static int offset;
static char rx_buf[CHARACTER_LIMIT+1];
static char temp;
static uint8_t rx_buf_pos = 0;
static uint8_t rx_FULL=0;
static uint8_t too_long=0;

																		
int main (void) { 
  TPM0_Init_PCM ();
	UART0_Init();
	
  while(1)
	{
		if(rx_FULL)		             // if message ready
		{
			if(!too_long){
				Morse_Encoder(rx_buf);   
				rx_FULL=0;	           // message played
				rx_buf_pos = 0;
				}
		}
	}
}


void Morse_Encoder(char* str){
	
	for (int j = 0; j < rx_buf_pos; j++){
		
		if ((uint8_t)*str == 32){ 
			DELAY(7*CYCLE) // wait 7 cycles (between words)
			str++;
			continue;
		}
		
		if ((uint8_t)*str > 60) offset = 55;
		else offset = 48;
		
		for (int i = 0; i < 5; i++){
			
			if (morse_map[*str-offset][i] == '0') {
				DELAY(CYCLE)   // wait 1 cycle (between letters)
				break;
			} 
			else if (morse_map[*str-offset][i] == '.'){ 
				TPM0_PCM_Play(1);
				DELAY(CYCLE)   // wait for speaker to stop playing
			}
			else {
				TPM0_PCM_Play(3);
				DELAY(3*CYCLE) // wait for speaker to stop playing
			}
			DELAY(CYCLE)   // wait 1 cycle (inter-element)
		}
		
		DELAY(2*CYCLE)     // wait 2+1 cycles (between letters)
		
		str++;
	}
	
}

void UART0_IRQHandler()
{
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		temp=UART0->D;	                     // Odczyt wartosci z bufora odbiornika i skasowanie flagi RDRF
		if(!rx_FULL)
		{
			if(temp!=LF)
			{
				if(!too_long)	                   // Jesli za dlugi ciag, ignoruj reszte znaków
				{
					rx_buf[rx_buf_pos] = temp;	   // Kompletuj komende
					rx_buf_pos++;
					if(rx_buf_pos == CHARACTER_LIMIT + 1) too_long=1;  // Za dlugi ciag
				}
			}
			else
			{
				if(!too_long) rx_buf[rx_buf_pos] = 0;       	// Jesli za dlugi ciag, porzuc tablice
				rx_FULL=1;
			}
		}
	NVIC_EnableIRQ(UART0_IRQn);
	}
}
