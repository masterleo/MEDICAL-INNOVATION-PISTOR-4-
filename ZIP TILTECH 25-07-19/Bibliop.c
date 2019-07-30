/****************************************************************************
		 	 PROGRAMME: Bibliop.C

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 20.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME :Bibliotheque de fonctions  
 
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46
***************************************************************************/
#include "p18f46K22.h"
#include "Bibliop.h"
#include "Main.h"
#include "i2crxtx.h"
#include "delay.h"

#define FLAGDEBUG		LATBbits.LATB6

unsigned char Tampon[5];

/********************************************************************
		MESURE ANALOGIQUE 10 bits
*********************************************************************/
unsigned int MesureAnalogique(unsigned char Voie)
{
static unsigned char i;
	//FLAGDEBUG = 1;

	ADCON0 = (Voie << 2)|0x01 ;//
	_asm nop _endasm
	_asm nop _endasm
	_asm nop _endasm
	_asm nop _endasm
	//Delay10Us();
	//for (i=0;i<5;i++);// Tempo pour changement de voie
	ADCON0bits.GO_DONE = 1;// Début de la conversion
	while(ADCON0bits.GO_DONE)continue;// wait for conversion complete
	//FLAGDEBUG = 0;	
return(((unsigned int)ADRESH << 8) + ADRESL);
	
}
/****************************************************************
	 Ecriture d'un Char dans l'Eeprom interne pour micro flash
*****************************************************************/
void EcritureCharEeprom(unsigned char Adresse,unsigned char Valeur)
{
	EEADR = Adresse;
	EEDATA = Valeur;
//	STATUSbits.C = 0;
//	if(INTCONbits.GIE)STATUSbits.C = 1;

	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;
	EECON1bits.WREN = 1;
	INTCONbits.GIE = 0;
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;
	INTCONbits.GIE = 1;
	EECON1bits.WREN = 0;
//	if(STATUSbits.C)INTCONbits.GIE = 1;
	while(EECON1bits.WR);
}
/****************************************************************
	 Lecture d'un Char dans l'Eeprom interne pour micro flash
*****************************************************************/
unsigned char LectureCharEeprom(unsigned char Adresse)
{
unsigned char Donnes,i;

	EEADR = Adresse;
	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;
	EECON1bits.RD = 1;
	Donnes = EEDATA;
	return (Donnes);
}
/*****************************************************************************
	 Conversion d'un entier hexa en digits decimaux
 		le resultat est dans tampon[]
****************************************************************************/
void ConvIntDecimal(unsigned int Nombre)
{
	Tampon[0] = (unsigned char)(Nombre / 10000);
	Nombre    = Nombre - (10000 * (unsigned int)Tampon[0]);
	Tampon[1] = (unsigned char)(Nombre / 1000);
	Nombre    = Nombre - (1000 * (unsigned int)Tampon[1]);
	Tampon[2] = (unsigned char)(Nombre / 100) ;
	Nombre    = Nombre - (100 * (unsigned int)Tampon[2]);
	Tampon[3] = (unsigned char)(Nombre / 10);
	
	Tampon[4] = (unsigned char)(Nombre - (10 * (unsigned int)Tampon[3]));

//ASCII Code
	Tampon[0] = Tampon[0] + '0';
	Tampon[1] = Tampon[1] + '0';
	Tampon[2] = Tampon[2] + '0';
	Tampon[3] = Tampon[3] + '0';
	Tampon[4] = Tampon[4] + '0';
}
/***********************************************************************
		Permet d'ecrire un char sur le bus I2c
***********************************************************************/
void EcritureCharI2C(char Adresse, char Valeur)
{
unsigned char TempACK = 1;

	I2c_start();
	I2c_write(Adresse&0xFE);
	I2c_write(Valeur);
	I2c_stop();
}
/***********************************************************************
		Permet de lire un Char sur le bus I2c
***********************************************************************/
int LectureCharI2C(char Adresse)
{
int Valeur;

	I2c_start();
	I2c_write(Adresse|0x01);
	Valeur = I2c_read (0); //no acknoledge
	I2c_stop();
	return (Valeur);
}
/***********************************************************************
		Permet d'ecrire un Int sur le bus I2c
***********************************************************************/
void WriteMCP23017(char Opcode,char RegAddress,char Value)
{

	I2c_start();
	I2c_write(Opcode&0xFE);
	I2c_write(RegAddress);
	I2c_write(Value);
	I2c_stop();
}
/***********************************************************************
		Permet de lire un Int sur le bus I2c
***********************************************************************/
unsigned char ReadMCP23017(char Opcode,char RegAddress)
{
int Valeur1;

	I2c_start();
	I2c_write(Opcode&0xFE);
	I2c_write(RegAddress);
	I2c_repStart();
	I2c_write(Opcode|0x01);
	Valeur1 = I2c_read (0); //MSB no acknoledge
	I2c_stop();
	return (Valeur1);
}

