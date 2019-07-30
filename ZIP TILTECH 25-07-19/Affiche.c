/****************************************************************************
		 	 PROGRAMME: Affiche.C

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME :Module routine affichage sur bus I2C (DEBUG uniquement)  
 
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/

#include "bibliop.h"
#include "i2crxtx.h"
#include "delay.h"

#define AFF_MES_DAT		0x4C//7C
#define AFF_MES_CMD		0x4E//7E
#define WR_REGISTRE_LCD	0x00
#define WR_DATA_LCD		0x04

void InitLCD(void);
void EcritureCmdAfficheur(char CommandeLCD);
void ClearAfficheur();
void SetDdramAdresse(char Position);
void AfficheCaractere(char Caractere);
//void AfficheMessage(auto const rom char*Message,char Longueur,char Position);
void AfficheMessage(auto const rom char*Message,char Longueur,char Ligne);
/***********************************************************************
				INITIALISATION DE L'AFFICHEUR
***********************************************************************/
void InitLCD(void)
{

//init des sorties du PCF8574
	
	EcritureCharI2C(AFF_MES_DAT,0xFF);
	EcritureCharI2C(AFF_MES_CMD,0xFF);

	EcritureCharI2C(AFF_MES_DAT,0x30);
	EcritureCmdAfficheur(WR_REGISTRE_LCD);
	EcritureCmdAfficheur(WR_REGISTRE_LCD);
	EcritureCmdAfficheur(WR_REGISTRE_LCD);
	
	
	EcritureCharI2C(AFF_MES_DAT,0x38);//fonction set 00110000 8bits,1ligne,5x7 dots	
	EcritureCmdAfficheur(WR_REGISTRE_LCD);

	EcritureCharI2C(AFF_MES_DAT,0x0C);//display ON,B=0 ,Blink OFF
	EcritureCmdAfficheur(WR_REGISTRE_LCD);

	EcritureCharI2C(AFF_MES_DAT,0x06);//increment not shift
	EcritureCmdAfficheur(WR_REGISTRE_LCD);
	
	EcritureCharI2C(AFF_MES_DAT,0x01);//clear display
	EcritureCmdAfficheur(WR_REGISTRE_LCD);

		
	
}

/**********************************************************************
	Genere le pulse sur E de 500us
**********************************************************************/
void EcritureCmdAfficheur(char CommandeLCD)
{
unsigned char i;

	
	//for(i=0;i<50;i++);
	Delay10Us();
	EcritureCharI2C(AFF_MES_CMD,(0xF1|CommandeLCD));//E=1
	
	//for(i=0;i<50;i++);
	Delay10Us();
	EcritureCharI2C(AFF_MES_CMD,(0xF0|CommandeLCD));//E=0
	

}
/**********************************************************************
		Affichage d'un message
**********************************************************************/
/*void AfficheMessage(auto const rom char*Message,char Longueur,char Position)
{
char i=0,j=0;

/*	if (Position <= 15)	i = Position;
	else i = (Position - 16) + 0xC0;

	SetDdramAdresse(i);

	do
		{
		if (i == 16) SetDdramAdresse(0xC0);

		AfficheCaractere(*(Message++)); 
		i++;
      		j++;
		}
	while (j < Longueur);*/

/*	if (Position <= 7) i = Position;
	else i = (Position - 8) + 0xC0;

	SetDdramAdresse(i);
	do
		{
		if (i== 8) SetDdramAdresse(0xC0);
		AfficheCaractere(*(Message++));
		i++;
		j++;

		}
	while (j<Longueur);		
}*/
void AfficheMessage(auto const rom char*Message,char Longueur,char Ligne)
{
char i=0,j=0;

	SetDdramAdresse(Ligne);
	while (j<Longueur)
		{
		AfficheCaractere(*(Message++));
		j++;
		}
			
}
/**********************************************************************
		Affichage d'un caractere
**********************************************************************/
void AfficheCaractere(char Caractere)
{
	
	EcritureCharI2C(AFF_MES_DAT,Caractere);
	EcritureCmdAfficheur(WR_DATA_LCD);
	

}	
/**********************************************************************
	Positionnnement de l'adresse de la Ddram de l'afficheur
**********************************************************************/
void SetDdramAdresse(char Position)
{

	EcritureCharI2C(AFF_MES_DAT,(0x80|Position));
	EcritureCmdAfficheur(WR_REGISTRE_LCD);
	
}	

	
/**********************************************************************
	Clear de l'afficheur (1.65ms)
**********************************************************************/
void ClearAfficheur()
{

	EcritureCharI2C(AFF_MES_DAT,0x01);//Clear display
	EcritureCmdAfficheur(WR_REGISTRE_LCD);		

}	
