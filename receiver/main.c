/*--------------------------------------------------------------------------------------------------------
Authors: Cezary Szczepański, Paweł Religa
Subcject: Techinka Mikroprocesorowa 2
University: AGH Univerity of Science and Technology
Project: Morse code reciever with decoding
---------------------------------------------------------------------------------------------------------*/


/*
					REALIZACJA PROGRAMU
	Kod Morse'a dla każdego znaku przypisuję serię '.' i '-'. Dodatkowo wykorzystuje stałą czasową, dla rozróżnienia
	tych dwóch symboli, a także dla wyznaczenia kolejnego symbolu oraz nadawanego wyrazu.	
	Czas trwania symboli:
		- '.': jedna stała czasowa
		- '-': trzy stałe czasowe
		- przerwa między nadawanymi symbolami: jedna stała czasowa
		- przerwa między nadawanymi literami: trzy stałe czasowe
		- przerwa między nadawanymi wyrazami: siedem stałych czasowych
	
	Dodatkowo każdy nadawany symbol, ma nie więcej niż 5 znaków.
*/
					
#include "MKL05Z4.h"
#include "pit.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "port.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TIME_UNIT BUS_CLOCK/10

uint8_t wynik_ok = 0;
uint16_t temp;
uint16_t wynik;
int sampling_on = 0;
uint16_t mCnt = 0;																				
uint16_t tempCnt = 0;
char recSym[] = {'0','0','0','0','0'};
static int wordCnt = 0;
//													DEKLARACJA TABLICY DO DEKODOWANIA kodu Morse'a
char morseTab[] = {'5','H','4','S','-','V','3','I','-','F','-','U','-','-','2','E','-','L','-','R','-','-','-','A','-','P','-','W','-','J','1','6','B','-','D','-','X','-','N','-','C','-','K','-','Y','-','T','7','Z','-','G','-','Q','-','M','8','-','-','O','9','-','0'};
char recWord[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
char recChar = 'x';

int symCnt = 0;
uint64_t bool_map = 0;
uint64_t bool_map_temp = 0;
int position = 0;
int zero_cnt=0;
int translate_flag = 0;
int break_flag = 0;
int ones_cnt = 0;
int space_flag = 0;
static int index = 0;	

void morseDecoder(char word[5]);

void PORTB_IRQHandler() {
													//	Obsługa przerwań GPIO na portB1. Mikrofon rejestrując sygnał, zmienia stan z wysokiego (1.5V) na niski. Każdorazowe takie zbocze 
													//	opadające jest zliczane w zmiennej mCnt. 

	if (sampling_on == 0) {							//	Funkcja do obsługi próbkowania sygnału z mikrofonu. Włącza licznik PIT gdy pojawi się pierwsze zbocze na portB.
		sampling_on = 1;
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	}

	mCnt++;

	PORTB->PCR[1] |= PORT_PCR_IRQC_MASK;			//	Wykasowanie flagi żądania przerwania.

}


void PIT_IRQHandler(){
													//	Licznik PIT, wywoływany po 1 jednostce czasowej. Sprawdza ile razy wystąpiły zbocza opadające w sygnale od mikrofonu od ostatniego 
													//	wywołania PIT. Jeśli mikrofon zanotował sygnał, ustawia bit w rejestrze bool_map na wysoki, jeśli nie zostawia w stanie niskim.
													//	Kolejne wywołania, ustawiają kolejne bity.
	int tempCnt = mCnt;
	mCnt = 0;
	
	if(tempCnt > 60){
		bool_map |= 1<<position;
		zero_cnt = 0;
	} else {
		zero_cnt++;
	}
	if(zero_cnt == 3){								//	Jeśli wystąpiły 3 zera, oznacza to że zakończono nadawanie jednej litery alfabetu.
		translate_flag = 1;							//	Ustawienie flagi wyzwalającej tłumaczenie.
		break_flag = position;
		bool_map_temp = bool_map;
		bool_map = 0;
		position = -1;
	}
	if(zero_cnt == 7){								// Wystąpienie 7 zer, oznacza przerwę między wyrazową w nadawnym zdaniu
		space_flag = 1;
	}
	position++;
	
	
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;		// Skasuj flagę żądania przerwania
	NVIC_ClearPendingIRQ(PIT_IRQn);
}



void morseDecoder(char word[5]){
													//	Algorytm dekodowania został stworzony na podstawie schematu: https://en.wikipedia.org/wiki/Morse_code#/media/File:Morse_code_tree3.svg
													//	Zbudowano do tego specjalną tbalicę morseTab. Funkcja polega na poruszaniu się po owej tablicy. Indexy są odpwiednio zwiększane 
													//	lub zmniejszane, zależnie od tego jaki znak jest w przekazanym słowie. 

	int i = 0;
	if (word[0] == '-') i += 47; else if (word[0] == '.')i += 16;
	if (word[1] == '-') i += 8; else if (word[1] == '.') i -= 8; else recChar =  morseTab[i-1];
	if (word[2] == '-') i += 4; else if (word[2] == '.') i -= 4; else recChar =  morseTab[i-1];
	if (word[3] == '-') i += 2; else if (word[3] == '.') i -= 2; else recChar =  morseTab[i-1];
	if (word[4] == '-') i += 1; else if (word[4] == '.') i -= 1; else recChar =  morseTab[i-1];
	recChar = morseTab[i-1];
}





int main (void)
{
	LCD1602_Init();		 							// Inicjalizacja wyświetlacza LCD
	PIT_Init();										// Inicjalizacja licznika PIT0
	port_Init();									// Inicjalizacja portów
	LCD1602_Backlight(TRUE);
	LCD1602_Print("xx");							// Ekran kontrolny
	LCD1602_SetCursor(0,1);
	LCD1602_Print("--");
	

	while(1)
	{
		if(translate_flag){							//	Wywołanie warunku oznacza, że wystąpiła przerwa między nadawanymi znakami. 
			translate_flag = 0;						//	Na podstawie zawartości rejestru bool_map pętla while rozpoznaje znak jako '.' lub '-', i zapisuje znak do recSym.
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
			
			morseDecoder(recSym);					//	Po rozpoznaniu zawartości rejestru bool_map i rozdzieleniu na odpowiednie znaki,
			recWord[wordCnt++] = recChar;			//	zarejstrowane znaki są przekazywane do tłumaczenia na litery alfabetu.
			
			LCD1602_SetCursor(0,0);
			LCD1602_Print(recSym);
			
			for(int i = 0; i < 5; i++){recSym[i] = '0';}
			symCnt = 0;
			
			LCD1602_SetCursor(0,1);
			LCD1602_Print(recWord);					//	Wysyłanie odczytanej litery na ekran.
		}
		
		if(space_flag){								//	Rozpoznanie i wyświetlenie spacji w odebranym zdaniu.
			space_flag = 0;
			recWord[wordCnt++] = ' ';
		}
		
	}
}
