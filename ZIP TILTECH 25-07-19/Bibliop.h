/****************************************************************************
		 	 PROGRAMME: Affiche.H

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME : header file du Module bibliotheque 
 
  ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/

void ConvIntDecimal(unsigned int Nombre);
void ConversionIntBCD(unsigned int Nombre1);
extern unsigned char Digit[4];
void EcritureCharI2C(char Adresse, char Valeur);
int LectureCharI2C(char Adresse);
int LectureADCI2C(char Adresse);
void EcritureIntI2C(char Adresse, int Valeur);
int LectureIntI2C(char Adresse);
void EcritureCharEeprom (unsigned char Adresse, unsigned char Valeur);
unsigned char LectureCharEeprom (unsigned char Adresse);
unsigned int MesureAnalogique(unsigned char Voie);

void WriteMCP23017(char Opcode,char RegAddress,char Value);
unsigned char ReadMCP23017(char Opcode,char RegAddress);
