/*--------------------------------------------------------------------------------------------------------
					Technika Mikroprocesorowa 2 - laboratorium
					Lab 5 - Ćwiczenie 4: wyzwalanie sprzętowe przetwornika A/C - tryb wyzwalania na żądanie (PIT0)
					autor: Mariusz Sokołowski
					wersja: 07.11.2020r.
---------------------------------------------------------------------------------------------------------*/
					
#include "MKL05Z4.h"
#include "ADC.h"
#include "pit.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "port.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "uart0.h"
#define TIME_UNIT BUS_CLOCK/10

//float adc_volt_coeff = ((float)(((float)2.91) / 4095) );			// Współczynnik korekcji wyniku, w stosunku do napięcia referencyjnego przetwornika
uint8_t wynik_ok = 0;
uint16_t temp;
uint16_t	wynik;
int recieveFlag = 0;
static int minDotCnt = 20;																			// zmienne do rozróżniania . od - na podstawie zliczonych impulsów
static int maxDotCnt = 40;
int sampling_on = 0;
uint16_t mCnt = 0;																			// wewnętrzny licznik do zliczania wywołań ADC
uint16_t tempCnt = 0;
//static char recChar;
char recSym[] = {'0','0','0','0','0'};
static int wordCnt = 0;
char morseTab[] = {'5','H','4','S','-','V','3','I','-','F','-','U','-','-','2','E','-','L','-','R','-','-','-','A','-','P','-','W','-','J','1','6','B','-','D','-','X','-','N','-','C','-','K','-','Y','-','T','7','Z','-','G','-','Q','-','M','8','-','-','O','9','-','0'};
char recWord[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
char recChar = 'k';
void morseDecoder(char word[5]);	
int symCnt = 0;
uint64_t bool_map = 0;
uint64_t bool_map_temp = 0;
int position = 0;
	int zero_cnt=0;
	int translate_flag = 0;
	int break_flag = 0;
	int ones_cnt = 0;

static int index = 0;	
void PIT_IRQHandler(){
	
	int tempCnt = mCnt;
	mCnt = 0;
	
	if(tempCnt > 60){
		bool_map |= 1<<position;
		zero_cnt = 0;
	} else {
		zero_cnt++;
	}
	if(zero_cnt == 3){
		translate_flag = 1;
		break_flag = position;
		bool_map_temp = bool_map;
		bool_map = 0;
		position = -1;
	}
	position++;
	
	
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;		// Skasuj flagę żądania przerwania
	NVIC_ClearPendingIRQ(PIT_IRQn);
}



void morseDecoder(char word[5]){
	int i = 0;
	if (word[0] == '-') i += 47; else if (word[0] == '.')i += 16;
	if (word[1] == '-') i += 8; else if (word[1] == '.') i -= 8; else recChar =  morseTab[i-1];
	if (word[2] == '-') i += 4; else if (word[2] == '.') i -= 4; else recChar =  morseTab[i-1];
	if (word[3] == '-') i += 2; else if (word[3] == '.') i -= 2; else recChar =  morseTab[i-1];
	if (word[4] == '-') i += 1; else if (word[4] == '.') i -= 1; else recChar =  morseTab[i-1];
	recChar = morseTab[i-1];
}




uint16_t status = 0;

void PORTB_IRQHandler(){
	
	
		if(sampling_on == 0){
			sampling_on = 1;
			PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
		}
	
		mCnt++;
	
		PORTB->PCR[1] |= PORT_PCR_IRQC_MASK;

}

int main (void)
{
//	uint32_t i=0;
	LCD1602_Init();		 																		// Inicjalizacja wyświetlacza LCD
	LCD1602_Backlight(TRUE);
	LCD1602_Print("xx");																	// Ekran kontrolny
	PIT_Init();																					  // Inicjalizacja licznika PIT0
	port_Init();																					// Inicjalizacja portów
	LCD1602_SetCursor(0,1);
	LCD1602_Print("--");
	

	while(1)
	{
		if(translate_flag){
			translate_flag = 0;
			ones_cnt = 0;
			for(int i = 0; i < break_flag; i++){
				if(bool_map_temp & 1<<i){
					ones_cnt++;
				}else{
					if(ones_cnt == 1){
						recSym[symCnt++] = '.';
					}else if(ones_cnt == 3){
						recSym[symCnt++] = '-';
					}
					ones_cnt = 0;
				}
			}
			
			morseDecoder(recSym);
			recWord[wordCnt++] = recChar;
			
			LCD1602_SetCursor(0,0);
			LCD1602_Print(recSym);
			sprintf(display,"%d",bool_map_temp);
			
			for(int i = 0; i < 5; i++){recSym[i] = '0';}
			symCnt = 0;
			
			LCD1602_SetCursor(0,1);
			LCD1602_Print(recWord);
		}

		
	}
}
