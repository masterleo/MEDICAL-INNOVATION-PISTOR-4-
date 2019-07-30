/****************************************************************************
		 	 PROGRAMME: Affiche.H

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME : Header file du Module routine affichage sur bus I2C   
 
  ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/

#define AFF_MES_DAT		0x4C//46
#define AFF_MES_CMD		0x4E
#define WR_REGISTRE_LCD	0x00
#define WR_DATA_LCD		0x04

extern unsigned char Tampon[5];

void InitLCD(void);
void EcritureCmdAfficheur(char CommandeLCD);
void ClearAfficheur();
void SetDdramAdresse(char Position);
void AfficheCaractere(char Caractere);
//void AfficheMessage(auto const rom char*Message,char Longueur,char Position);
void AfficheMessage(auto const rom char*Message,char Longueur,char Ligne);
