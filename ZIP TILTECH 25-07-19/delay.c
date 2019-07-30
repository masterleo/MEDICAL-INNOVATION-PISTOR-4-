/****************************************************************************
		 	 PROGRAMME: delay.C

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME : Fonction delay   
 
  ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/
#include "delay.h"

#define	XTAL_FREQ 8 /* frequence en MHz */

/*
void Delay100Us(void);
void Delay250Us(void);
void DelayMs(unsigned char cnt);*/
/****************************************************************/
void Delay10Us(void)
{
static unsigned char  _dcnt;
	//_dcnt=5/(12/XTAL_FREQ);
	//_dcnt=10;
	_dcnt=40;
	while(--_dcnt) continue;
}
/**************************************************************************/
void Delay100Us(void)
{
static unsigned char  _dcnt;
	//_dcnt=50/(12/XTAL_FREQ);//100/(12/XTAL_FREQ)
	_dcnt=100;
	while(--_dcnt) continue;
}		
/**************************************************************************/	 
void Delay250Us(void)
{
static unsigned char  _dcnt;
//	_dcnt=125/(12/XTAL_FREQ);//250/(12/XTAL_FREQ)
	_dcnt=250;
	while(--_dcnt) continue;
}		
/**************************************************************************/
void DelayMs(unsigned char cnt)
{
static unsigned char	i;
	do {
		//i = 4;
		i = 16;
		do {
			Delay250Us();
		} while(--i);
	} while(--cnt);
}
/*************************************************************************/
