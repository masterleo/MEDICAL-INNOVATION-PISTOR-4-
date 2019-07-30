/****************************************************************************
		 	 PROGRAMME: Main.C

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :	15.03.2017			

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME : Programme principal   
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46  
*****************************************************************************
Historique des modifications:
03.12.2015	V1:Version prototype debugage hard
15.02.2015	V2:Version Valisée production
17.01.2017	V6: correction bugs rapport 170315
20.10.2017	V7: correction bug
15.03.2018	V8: correction bugs rapport 171130
*****************************************************************************/
#include <p18f46K22.h>
#include "Main.h"
#include "bibliop.h"
#include "i2crxtx.h"
#include "MCP23X17.h"
#include "delay.h"
#include "affiche.h"
//#include <delays.h>

//#define DEBUG

//Temps avec base de temps 50ms (boucle)
#define _50Ms		1
#define _100Ms		2
#define _200Ms		4
#define _250Ms		5
#define _400Ms		8
#define _600Ms		12
#define _800Ms		16
#define _1s			20
#define _2s			40
#define _8s			160
#define _1mn		1200
#define _15mn		18000	
//etat des variables
#define ON			0
#define OFF			1
#define FOWARD		1
#define REVERSE		2
#define ARRET		0
//#define ALLER		1
//#define RETOUR		0

#define NB_BIP	1//1=50ms

//parametres moteur CHARIOT (656 impulsions par Tour)
#define NB_PULSE_PAR_MM			46//44	// 656 impulsions par tour MAX 708pts pour 15mm soit 44pts/mm
#define VALEUR_GARDE			87	//2mm
#define ZERO_AIGUILLE_4MM		396 //390 //9mm
#define ZERO_AIGUILLE_6MM		303 //296 //7mm
#define ZERO_AIGUILLE_13MM		0	//0mm
#define NB_FREIN_CH				10  //NB impulsion pour le frein chariot

//Parametre moteur SERINGUE (448 impulsion par tour)
#define NB_FREIN_SE						10//NB impulsion pour le frein seringue
#define NB_PULSE_ANTIGOUTTE_CONTINU		224-NB_FREIN_SE//	 1/2Tour - frein
#define NB_PULSE_ANTIGOUTTE_STANDARD	30-NB_FREIN_SE// 	1/15Tour - frein
#define NB_PULSE_ANTIGOUTTE_RAFALE		60-NB_FREIN_SE// 	2/15Tour - frein
#define NB_PULSE_MESO_PERFUSION			120-NB_FREIN_SE// 130=	4/15Tour - frein
//#define NB_PULSE_MESO_PERFUSION_1S		32-NB_FREIN_SE// 25 =	1/15Tour - frein
#define NB_PULSE_MESO_PERFUSION_1S		27-NB_FREIN_SE// 25 =	1/15Tour - frein --> 32=16ml; 24=6ml

#define NB_COUP_INIT	50

#define SEUIL_SURCOURANT_SERINGUE		600//540mA
#define SEUIL_SURCOURANT_CHARIOT		700//620mA
//
#define ETAT_PROF_DEF	4
//batterie
#define BATTERIE_VIDE			630//1 barre clignotante 6.00V		
#define BATTERIE25PC			660//1 barre
#define BATTERIE50PC			700//2 barres
#define BATTERIE75PC			740//3 barres
#define BATTERIE100PC			780//4 barres  = 8.00V
//define des differents menus
#define MENU_DEBIT_MAX				8
#define	MENU_AIGUILLE_MAX			2 //0,1,2
#define MENU_SERINGUE_MAX			5
#define MENU_PROFONDEUR_MAX			9
#define MENU_DUREE_MAX				3
#define MENU_TEMPSENTREDOSE_MAX		6

#define SERINGUE_10ML				5
#define SERINGUE_5ML				4
#define SERINGUE_3ML				3
#define SERINGUE_2ML				2
#define SERINGUE_1ML				1

#define FREQUENCE_MAX_150			2//150
#define FREQUENCE_MAX_250			4//250
#define FREQUENCE_MAX_300			5//300

#define FREQUENCE_100			1
#define FREQUENCE_150			2
#define FREQUENCE_200			3
#define FREQUENCE_250			4
#define FREQUENCE_300			5
//profondeur
#define PROFONDEUR_MAX_4MM		4
#define PROFONDEUR_MAX_6MM		6
#define PROFONDEUR_MAX_9MM		9

#define PROFONDEUR_0MM			0
#define PROFONDEUR_1MM			1
#define PROFONDEUR_2MM			2
#define PROFONDEUR_3MM			3
#define PROFONDEUR_4MM			4
#define PROFONDEUR_5MM			5
#define PROFONDEUR_6MM			6
#define PROFONDEUR_7MM			7
#define PROFONDEUR_8MM			8
#define PROFONDEUR_9MM			9 

#define DEBITINJECTION_3ML		1
#define DEBITINJECTION_5ML		3
#define DEBITINJECTION_8ML		6
#define DEBITINJECTION_10ML		8

#define AIGUILLE_4MM			0
#define AIGUILLE_6MM			1
#define AIGUILLE_13MM			2
//MODE
#define MENU_INIT				0
#define MENU_CONTINU			1
#define MENU_STANDARD			2
#define MENU_MESOPERFUSION		3
#define MENU_RAFALE				4
#define MENU_DEFAUT_BATT		5
//#define MENU_DEFAUT_COURANT		6


//adresse des composants I2C
#define ADD_U1	0x040//0x40 & 0x41
#define ADD_U2	0x042//0x42 & 0x43
#define ADD_U3	0x044//0x44 & 0x45
#define ADD_U4	0x046//0x46 & 0x47

//afficheur
#define LIGNE1	0x00
#define LIGNE2	0x40
#define LIGNE3	0x10
#define LIGNE4	0x50
//********************************************************************
// Global Variable declarations
//********************************************************************

unsigned int i;
unsigned char Menu = MENU_INIT,OldMenu = MENU_INIT,MenuSauvegarde=MENU_INIT;
unsigned int TempoCycle = 0,TempoArretAuto=0;
unsigned char FctGachetteMeso=0,FctGachette=0,OldGachette=0,FctGachetteRafale=0,DeclencheMeso=0;
unsigned char BaseTemps=0,Bip=0,TempoInjection=0;
unsigned char TempoSecondes = 0;
unsigned char Blink_1s = 0;
unsigned int TensionBatterie=0,ValeurBatterie=800;
unsigned char ValeurClavier=0,OldValeurClavier=0;

unsigned char TempoRafale = 0,TempoFrequence=7;
unsigned char Compteur=0,TempoSecondeInjection=0;
unsigned int TempoMeso = 0,TempoEntreDoses=0;
unsigned int CourantSeringue = 0,CourantChariot = 0;
unsigned char SensSeringue = 0,FlagFinRafale = 1,FlagSurcourant = 0;
unsigned char TempoDefautBatterie=0,TempoDefautCourant=0;
int ZeroAiguille = 0;
char FlagInit = OFF;
unsigned char OldPlus1=0,OldPlus2=0,OldMoins1=0,OldMoins2=0;
unsigned char FrequenceMax = 0,DebitInjectionMax=0;
unsigned char FlagStopGoutte = 0;
unsigned char SensChariot = ARRET;//,PhaseChariot = ALLER;
unsigned char FlagPlus=0,FlagCorrection=0,CompteurCoups=0;
unsigned char FlagRetourChariot=0;

int NbPulseSeringue=0,NbPulseQuantiteAInjecter = 100,NbPulseAntigoutte=50,NbPulseSeringueABS=0;
int NbPulseChariot=0,NbPulseProfondeur = 100;//,NbPulseChariotRef=0;
//int DProfondeurFW=0,DProfondeurRW=0,DProfondeurFWRW = 100;
int CompteurPulseCh = 0,MaxAiguille =0,PositionZAiguille=0;
//debug
int NbChFW=0,NbChRW=0,NbSeFW=0,NbSeRW=0,NbSeFW1=0,NbRafale=0;

unsigned char FlagA = 0,FlagHalt=0;

/*unsigned char NbPulseFrein[10]={10,11,11,11,12,12,12,13,13,13};//en fonction de la profondeur
unsigned int NbPulProContinu[3][10]={{98,138,181,230,271,271,271,271,271,271},{90,134,177,223,265,312,357,357,357,357},{99,141,187,230,271,314,353,395,436,480}};//[Aiguille][profondeur] 
unsigned int NbPulProStandard[3][10]={{93,138,181,230,271,271,271,271,271,271},{90,134,177,223,265,312,357,357,357,357},{99,141,187,230,271,314,353,395,436,480}};//[Aiguille][profondeur]
unsigned int NbPulProRafale[3][10]={{93,138,181,230,271,271,271,271,271,271},{90,134,177,223,265,312,357,357,357,357},{99,141,187,230,271,314,353,395,436,480}};//[Aiguille][profondeur]*/

const rom unsigned char NbPulseFrein[10]={10,11,11,11,12,12,12,13,13,13};//en fonction de la profondeur
//const rom unsigned int NbPulProContinu[3][10]={{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501}};//[Aiguille][profondeur] 
//const rom unsigned int NbPulProStandard[3][10]={{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501}};//[Aiguille][profondeur]
//const rom unsigned int NbPulProRafale[3][10]={{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501},{87,133,179,225,271,317,363,409,455,501}};//[Aiguille][profondeur]

//const rom unsigned int NbPulProContinu[3][10]={{83,130,177,227,278,0,0,0,0,0},{77,120,165,212,261,311,361,0,0,0},{97,140,183,226,270,312,352,396,440,484}};
//const rom unsigned int NbPulProStandard[3][10]={{83,129,178,227,279,0,0,0,0,0},{78,133,165,211,261,312,362,0,0,0},{95,140,183,225,270,311,352,395,438,482}};
//const rom unsigned int NbPulProRafale[3][5]={{98,140,190,237,291},{97,127,182,214,274},{94,137,181,225,269}};

const rom unsigned int NbPulProContinu[3][10]={{85,131,178,226,280,0,0,0,0,0},{81,121,168,212,260,312,361,0,0,0},{96,141,180,225,266,306,348,392,433,481}};
const rom unsigned int NbPulProStandard[3][10]={{87,131,178,229,278,0,0,0,0,0},{81,128,166,211,260,308,360,0,0,0},{90,136,178,221,266,309,355,392,434,478}};
const rom unsigned int NbPulProRafale[3][5]={{102,142,190,239,290},{100,122,183,214,273},{91,133,176,221,265}};

#define MARCHE			LATCbits.LATC7
#define BUZZER			LATAbits.LATA3
#define LED				LATCbits.LATC0
#define FLAGDEBUG		LATBbits.LATB6 

#define IN1				LATDbits.LATD0
#define IN2				LATDbits.LATD1
#define IN3				LATDbits.LATD2
#define IN4				LATDbits.LATD3
#define ENA				LATCbits.LATC2
#define ENB				LATCbits.LATC1

#define GACHETTE		PORTEbits.RE2
#define FDC_CHARIOT		PORTBbits.RB3
#define PLUS1			PORTAbits.RA4
#define PLUS2			PORTBbits.RB4
#define MOINS1			PORTAbits.RA5
#define MOINS2			PORTBbits.RB5
#define SENSROT			PORTBbits.RB1
#define SENSROT2		PORTBbits.RB2

union byte_def LedU1A=0xFF;
union byte_def LedU1B=0xFF;
union byte_def LedU2A=0xFF;
union byte_def LedU2B=0xFF;
union byte_def LedU3A=0xFF;
union byte_def LedU3B=0xFF;
union byte_def LedU4B=0xFF;
unsigned char Led_Touche_Profondeur = OFF;


typedef struct
{
unsigned char Aiguille;
unsigned char Seringue;
unsigned char Profondeur;
unsigned char DureeInjection;
unsigned char TempsEntreDose;
unsigned char DebitInjection;
unsigned char Frequence;
unsigned int ProfondeurMax;
} MENU_TYPE;
//Variable d'état
MENU_TYPE Etat;

/****************************/
// Declaration de fonctions
void InterruptHandlerHigh ();
void IntRafale(void);
void InitPIC(void);
void InitLedsClavier(void);
unsigned char LectureClavier(void);
void GestionClavier(unsigned char Valeur);
void EcritureLeds(void);
void LectureTensionBatterie(void);

void MoteurSeringueFoward(char ValeurPwm);
void MoteurChariotFoward(char ValeurPwm);
void MoteurSeringueReverse(char ValeurPwm);
void MoteurChariotReverse(char ValeurPwm);
void MoteurSeringueBrake(char ValeurPwm);
void MoteurChariotBrake(char ValeurPwm);
void MoteurSeringueRL(void);
void MoteurChariotRL(void);

void GestionMenuModeContinu(void);
void GestionMenuModeStandard(void);
void GestionMenuModeMesoPerfusion(void);
void GestionMenuModeRafale(void);

void GestionLeds(void);
void InitDefaultValue(unsigned char ModeT);
void InitDemarrage(void);
void RAZLeds(void);
void GestionMoteurModeContinu(void);
void GestionMoteurModeStandard(void);
void GestionMoteurModeMesoPerfusion(void);
void GestionMoteurModeRafale(void);
void StopGoutteRafale(void);
void InitPositionChariot(void);
void CalculNbPulseQuantiteContinu(void);
void CalculNbPulseQuantiteStandard(void);
void CalculNbPulseQuantiteRafale(void);
void CalculProfondeurMax(unsigned char ModeApp);
void CalculZeroAiguille(void);
void CalculTempoFrequence(void);
void CalculFrequenceMax(void);
void CalculDebitInjectionMax(void);
void LectureCourantsMoteurs(void);
void GestionDefautCourant(void);
void GestionDefautBatterie(void);
void GestionInit(void);
void AfficheI2C(void);
void AfficheTexte(void);
void LectureBoutonPlusMoins(unsigned char ModeApp);
void BipBipArret(void);
void Test(void);
void SauvegardeMemoire(void);
void LectureMemoire(void);
void RetourSeringue(void);
void AjustePositionChariot(void);
/**************************************************************************************
						Programme principal
***************************************************************************************/
void main(void)
{
	
	InitPIC();
	
	INTCONbits.GIE = 0;

	_asm
        clrwdt //Commande en assembleur du chien de garde 
	_endasm

	I2c_init();

	INTCONbits.GIE = 1;
	DelayMs(200);

#if defined(DEBUG)//uniquement pour debug
	InitLCD();
	DelayMs(200);
	AfficheTexte();
#endif
	ClrWdt();
	InitLedsClavier();
	
	//InitDefaultValue(MENU_CONTINU);
	InitDemarrage();

	Led_TempsEntreDose_0s = ON;
	Led_Frequence_100cps_mn = ON;
	Led_Debit_3ml_mn = ON;
	Led_Duree_Injection_0s = ON;
	Led_Profondeur_0mm = ON;
	Led_Aiguille_4mm = ON;
	Led_Seringue_1ml = ON;
	MoteurSeringueBrake(0);

	T1CONbits.TMR1ON = 1;
	while(1)
		{
		//FLAGDEBUG = !FLAGDEBUG;
		ClrWdt();
		RAZLeds();

		if (TempoCycle >= _50Ms) // Led ALIVE
			{
			TempoCycle= 0;

			ValeurClavier = LectureClavier();
			switch(Menu)
				{
				case MENU_INIT			:InitPositionChariot();
										 Bip = NB_BIP;
										 DelayMs(100);
										 Bip = NB_BIP;
										 Menu = MenuSauvegarde;
										 //Menu = MENU_CONTINU;
										 GestionLeds();
										 break;
				case MENU_CONTINU		:GestionMenuModeContinu();
										 if(FlagInit == ON) GestionInit();
										 if(FctGachette == 1)
											{
											GestionMoteurModeContinu();
											//Test();
											Bip = NB_BIP;
											FctGachette = 0;
											if(CompteurCoups > NB_COUP_INIT)FlagInit = ON;
											}
										 if((GACHETTE == 1)&&(FlagRetourChariot==1))RetourSeringue();
										 GestionLeds();
										 break;
				case MENU_STANDARD		:GestionMenuModeStandard();
										 if(FlagInit == ON) GestionInit();
										 if(FctGachette == 1)
											{
											GestionMoteurModeStandard();
											Bip = NB_BIP;
											FctGachette = 0;
											if(CompteurCoups > NB_COUP_INIT)FlagInit = ON;
											}
										 MoteurChariotBrake(0);
										 if((GACHETTE == 1)&&(FlagRetourChariot==1))RetourSeringue();
										 GestionLeds();
										 break;
				case MENU_MESOPERFUSION	:GestionMenuModeMesoPerfusion();
										 if(DeclencheMeso == 1)
											{
											GestionMoteurModeMesoPerfusion();
											DeclencheMeso = 0;
											}
										 if((GACHETTE == 1)&&(FlagRetourChariot==1))RetourSeringue();
										 GestionLeds();
										 break;
				case MENU_RAFALE		:GestionMenuModeRafale();
										 if(FlagInit == ON) GestionInit();
										 if(FctGachetteRafale == 1)
											{
											GestionMoteurModeRafale();
											FctGachetteRafale = 0;
											if(CompteurCoups > NB_COUP_INIT)FlagInit = ON;
											}
										 
										 if(FlagStopGoutte == 1)StopGoutteRafale();
										 if((GACHETTE == 1)&&(FlagRetourChariot==1))RetourSeringue();
										 GestionLeds();
										 break;
				case MENU_DEFAUT_BATT	:GestionDefautBatterie();break;
				

				default:Menu = MENU_INIT;break;
				}
			
			
			EcritureLeds();

			#if defined(DEBUG)//uniquement en debug
			AfficheI2C();
			#endif
			}

		}
}
/***********************************************************************
				INTERRUPTION
***********************************************************************/
// High priority interrupt vector

#pragma code InterruptVectorHigh = 0x08
void
InterruptVectorHigh (void)
{
  _asm
    goto InterruptHandlerHigh //jump to interrupt routine
  _endasm
}

// High priority interrupt routine

#pragma code
#pragma interrupt InterruptHandlerHigh

void InterruptHandlerHigh ()
{    

if (PIR1bits.TMR1IF)	// Interruption timer1 25ms
	{
	TMR1H = 0x3C;//0xFFFF-0x3CAF =0xC350 = 50000d = 50ms/32Mhz
	TMR1L = 0xAF;//ajustement tempo
		
	FlagA = !FlagA;//Flag 50ms

	CourantChariot = (CourantChariot + CourantChariot + CourantChariot + MesureAnalogique(2))>>2;//mesure courant

	if(FlagA==0)//toutes les 50ms uniquement
		{
		ClrWdt();
		TempoSecondes++;
		if(TempoSecondes > _1s)//clignotement 1s
			{
			TempoSecondes = 0;
			Blink_1s = ~Blink_1s;
			}

		TempoArretAuto++;
		if(TempoArretAuto >= _15mn)MARCHE=0;//arret automatique 15mn
		if(TempoSecondeInjection > 0) TempoSecondeInjection--;

	//	if(FlagHalt == 0)
	//		{
			switch(Menu)//suivant le menu
				{
				case MENU_CONTINU		:FctGachetteMeso = 0;
										FctGachetteRafale = 0;
										if((GACHETTE==1)&&(OldGachette == 0)&&(FlagRetourChariot==0))
											{
											FctGachette = 1;
											TempoArretAuto = 0;
											}
										OldGachette = GACHETTE;
										if(GACHETTE == 0)FlagRetourChariot=0;
										break;
				case MENU_STANDARD		:FctGachetteMeso = 0;
										FctGachetteRafale = 0;
										if((GACHETTE==1)&&(OldGachette == 0)&&(FlagRetourChariot==0))
											{
											FctGachette = 1;
											TempoArretAuto = 0;
											}
										OldGachette = GACHETTE;
										if(GACHETTE == 0)FlagRetourChariot=0;
										if(TempoInjection > 0)TempoInjection--;
											
										break;
				case MENU_RAFALE		:FctGachetteMeso = 0;
										FctGachette = 0;
										if(FlagRetourChariot==0)
											{
											if(GACHETTE == 1)
												{
												TempoRafale++;
												TempoArretAuto = 0;
												}
											else 
												{
												TempoRafale = (TempoFrequence-1);
												if(OldGachette == 1)
													{
													Bip=2;
													FlagStopGoutte = 1;
													}
												}
											}
										if(GACHETTE == 0)FlagRetourChariot=0;
										OldGachette = GACHETTE;
		
										if((TempoRafale >= TempoFrequence)&&(FlagFinRafale == 1))
											{
											FLAGDEBUG = !FLAGDEBUG;
											FlagFinRafale = 0;
											TempoRafale = 0;
											FctGachetteRafale = 1;
											}
										break;
				case MENU_MESOPERFUSION	:
										 if((GACHETTE==1)&&(OldGachette == 0)&&(FlagRetourChariot==0))
											{
											FctGachetteMeso = !FctGachetteMeso;
											TempoMeso = TempoEntreDoses-1;
											}
										OldGachette = GACHETTE;
										if(GACHETTE == 0)FlagRetourChariot=0;

										if(FctGachetteMeso == 1) 
											{
											TempoMeso++;
											TempoArretAuto = 0;
											}
										if(TempoMeso >= TempoEntreDoses)
											{
											TempoMeso = 0;
											DeclencheMeso = 1;
											}
										break;	
				case MENU_DEFAUT_BATT		:TempoDefautBatterie++;break;
					
				default:break;
				}

		if(FlagHalt == 0)
			{
		//affichage batterie
			LectureTensionBatterie();
		
			WriteMCP23017(ADD_U2,OLATA,LedU2A.c);//affichage batterie
			}//fin FlagHalt
		if(Bip > 0) 
			{
			BUZZER = ON;
			Bip--;
			}
		else BUZZER = OFF;
	//seuil surcourant seringue
		if(CourantSeringue > SEUIL_SURCOURANT_SERINGUE)
			//if(TempoArretAuto >= _1mn)
			{
			OldMenu = Menu;
			MoteurSeringueRL();
			BipBipArret();
			}
		if((CourantChariot > SEUIL_SURCOURANT_CHARIOT)&&(SensChariot == FOWARD))
			{
			OldMenu = Menu;
			MoteurChariotRL();
			BipBipArret();
			}
	
		TempoCycle++;
		}
	else
		{
		if(Menu == MENU_RAFALE) IntRafale();
		}
	//FLAGDEBUG = 0;
	PIR1bits.TMR1IF = 0;
	}

if(INTCONbits.INT0IF)//INT Pulses Seringue
	{
//FLAGDEBUG = 0;
		//INTCONbits.INT0IF = 0;
	NbPulseSeringue++;
	NbPulseSeringueABS++;
	if((NbPulseSeringue > 30)&&(NbPulseSeringue <= 40))
		{
		LED = 1;
		if(SensSeringue == 1)CourantSeringue = (CourantSeringue + MesureAnalogique(1))>>1;
		}
	LED = 0;
	INTCONbits.INT0IF = 0;
	}
/*	if (INTCON3bits.INT1IF)
		{
		if(SENSROT2 == 0)CompteurPulseCh++;
		else CompteurPulseCh--;
			
		INTCON3bits.INT1IF = 0;
		}*/

if(INTCON3bits.INT2IF)//INT Pulses Chariot
	{ 
	if(SENSROT == 0)
		{
		CompteurPulseCh++;
		NbPulseChariot++;
		}
	else
		{
		CompteurPulseCh--;
		NbPulseChariot--;
		}

	INTCON3bits.INT2IF = 0;
	}

}
/*************************************************************************

**************************************************************************/
void IntRafale(void)
{
	FctGachetteMeso = 0;
	FctGachette = 0;
	
	if(FlagRetourChariot==0)
		{
		if(GACHETTE == 1)
			{
			TempoRafale++;
			TempoArretAuto = 0;
			}
		else 
			{
			TempoRafale = (TempoFrequence-1);
			if(OldGachette == 1)
				{
				Bip=2;
				FlagStopGoutte = 1;
				}
			}
		}
	OldGachette = GACHETTE;
	
	if((TempoRafale >= TempoFrequence)&&(FlagFinRafale == 1))
		{
		FLAGDEBUG = !FLAGDEBUG;
		FlagFinRafale = 0;
		TempoRafale = 0;
		FctGachetteRafale = 1;
		}

}
/***************************************************************************
					Initialisation du PIC
****************************************************************************/
void InitPIC(void)
{
	RCON = 0;
	OSCCON = 0x70;//16MHz
	OSCCON2 = 0x80;// PLLx4 --> 64MHz
	OSCTUNE = 0x40;//40;

	INTCON = 0;
	INTCON2 = 0;
	INTCON3 = 0;
//convertisseur ADC
	ADCON0 = 0x01;// A/D module power up 
	ADCON1 = 0x00;	// AN4,AN3,AN2,AN1,AN0 
	ADCON2 = 0xAE;	//8TAD, Fosc/32

	ANSELA = 0x07; //,AN2,AN1,AN0 Analog and digital input buffer enable
	ANSELB = 0; //digital input buffer enable
	ANSELC = 0; //digital input buffer enable att pour I2C
	ANSELE = 0;

	TRISA = 0xF7;	// PortA
	PORTA = 0x08;
	
	TRISB = 0xBF;	//Port B entree
	PORTB = 0x00;

	TRISC = 0x00;	// Port C
	PORTC = 0x80;//on active MARCHE

	TRISD = 0x00;	// Port D
	PORTD = 0x00;

	TRISE = 0xFF;	// Port E
	PORTE = 0x00;

//	RCONbits.IPEN = 1;            //enable priority levels

// Init des interruptions Timer1  	
	TMR1H = 0x3C;
	TMR1L = 0xAF;
	T1CON = 0x30;	// osc/8 
// Init des interruptions Timer1  	
	//TMR4 = 0x7F;
	//T4CON = 0xF7;	// osc/8

	INTCONbits.INT0IE = 1;  // int0 enable seringue
	INTCON2bits.INTEDG0 = 0; // falling edge

//	INTCON2bits.INTEDG1 = 0; // rising edge
//	INTCON3bits.INT1IE = 1;  // int1 enable

	INTCON2bits.INTEDG2 = 0; // falling edge chariot
	INTCON3bits.INT2IE = 1;  // int2 enable
	INTCON3bits.INT2IP = 1;//high priority

	INTCONbits.INT0IF = 0;
	INTCON3bits.INT2IF = 0;
	INTCON3bits.INT1IF = 0;

	PIE1            = 0x01;//RCIE ,I2C, TMR1IE interupt reg 1 0x29
//	PIE2 			= 2;
	//PIE5			 = 2;//timer4
//	PIE3			= 0;
	IPR1            = 0x00;//Priority Register 1
	IPR2 			= 0x00;
//	IPR3			= 0;
	PIR1			= 0;
	PIR2            = 0;//interupt request flag register 2
//	PIR3 			= 0;

	INTCONbits.PEIE = 1;

	
}
/***************************************************************************
				Initialisation des leds du clavier
****************************************************************************/
void InitLedsClavier(void)
{

	WriteMCP23017(ADD_U1,IOCONA,0x30);//0010 0000
	WriteMCP23017(ADD_U1,IOCONB,0x30);//0011 0000
	WriteMCP23017(ADD_U2,IOCONA,0x30);//0010 0000
	WriteMCP23017(ADD_U2,IOCONB,0x30);//0011 0000
	WriteMCP23017(ADD_U3,IOCONA,0x30);//0010 0000
	WriteMCP23017(ADD_U3,IOCONB,0x30);//0011 0000
	WriteMCP23017(ADD_U4,IOCONA,0x30);//0010 0000
	WriteMCP23017(ADD_U4,IOCONB,0x30);//0011 0000

	WriteMCP23017(ADD_U1,IODIRA,0);//en sortie
	WriteMCP23017(ADD_U1,IODIRB,0);//en sortie
	WriteMCP23017(ADD_U2,IODIRA,0);//en sortie
	WriteMCP23017(ADD_U2,IODIRB,0);//en sortie
	WriteMCP23017(ADD_U3,IODIRA,0);//en sortie
	WriteMCP23017(ADD_U3,IODIRB,0);//en sortie
	WriteMCP23017(ADD_U4,IODIRA,0);//en sortie
	WriteMCP23017(ADD_U4,IODIRB,0);//en sortie
	BUZZER = ON;
	DelayMs(250);
	DelayMs(250);
	DelayMs(250);
	DelayMs(250);
	BUZZER = OFF;
	WriteMCP23017(ADD_U1,OLATA,0xFF);
	WriteMCP23017(ADD_U1,OLATB,0xFF);
	WriteMCP23017(ADD_U2,OLATA,0xFF);
	WriteMCP23017(ADD_U2,OLATB,0xFF);
	WriteMCP23017(ADD_U3,OLATA,0xFF);
	WriteMCP23017(ADD_U3,OLATB,0xFF);
	WriteMCP23017(ADD_U4,OLATB,0xFF);

}
/***************************************************************************
					Lecture du clavier
****************************************************************************/
unsigned char LectureClavier(void)
{
unsigned char ValeurClavier1 = 0,ValeurClavier2 = 0;

	WriteMCP23017(ADD_U4,IODIRA,0xE0);//0x1110 0000 en entree et sortie
	if(Led_Touche_Profondeur == ON) WriteMCP23017(ADD_U4,OLATA,0x00);
	else WriteMCP23017(ADD_U4,OLATA,0x01);

	ValeurClavier1 = ReadMCP23017(ADD_U4,GPIOA);//on lit les 3 entrées
	ValeurClavier1 = ValeurClavier1&0xE0;


	WriteMCP23017(ADD_U4,IODIRA,0x1E);//0x0001 1110 en sortie et entree
	if(Led_Touche_Profondeur == ON) WriteMCP23017(ADD_U4,OLATA,0x00);
	else WriteMCP23017(ADD_U4,OLATA,0x01);
	
	ValeurClavier2 =  ReadMCP23017(ADD_U4,GPIOA);
	ValeurClavier2 = ValeurClavier2 & 0x1F;

	return (ValeurClavier1 | ValeurClavier2 | 1);
}
/***************************************************************************
					Ecriture des leds
****************************************************************************/
void EcritureLeds(void)
{

	WriteMCP23017(ADD_U1,OLATA,LedU1A.c);
	WriteMCP23017(ADD_U1,OLATB,LedU1B.c);
	WriteMCP23017(ADD_U2,OLATA,LedU2A.c);
	WriteMCP23017(ADD_U2,OLATB,LedU2B.c);
	WriteMCP23017(ADD_U3,OLATA,LedU3A.c);
	WriteMCP23017(ADD_U3,OLATB,LedU3B.c);
	WriteMCP23017(ADD_U4,OLATB,LedU4B.c);

}
/***************************************************************************
				Lecture de la tension batterie et gestion led
****************************************************************************/
void LectureTensionBatterie(void)
{
unsigned char Buffer=0;

	ValeurBatterie = ((ValeurBatterie*15) + MesureAnalogique(0))>>4;
	TensionBatterie = (ValeurBatterie *50)/51;

	Buffer = (LedU2A.c & 0xF0);

	if(TensionBatterie < BATTERIE_VIDE)//clignote
		{
		//if(Blink_1s != 0) LedU2A.c = 0x0E;//allume dernier segment
		//else LedU2A.c = 0x0F;//eteint
		Menu = MENU_DEFAUT_BATT;
		}
	else
		{
		if(TensionBatterie < BATTERIE25PC)
			{
			if(Blink_1s != 0) LedU2A.c = 0x0E;//allume dernier segment
			else LedU2A.c = 0x0F;//eteint
			}
		else if (TensionBatterie < BATTERIE50PC)LedU2A.c = 0x0E;//1 barre
			 else if(TensionBatterie < BATTERIE75PC)LedU2A.c = 0x0C;//2 barres
				  else if(TensionBatterie < BATTERIE100PC)LedU2A.c = 0x08;//3 barres
					   else LedU2A.c = 0x00;//4 barres		
		}

	LedU2A.c = (LedU2A.c | Buffer);

	if(Etat.DebitInjection == 5)Led_Debit_7ml_mn = ON;
	else if(Etat.DebitInjection == 6)Led_Debit_8ml_mn = ON;
		 else if(Etat.DebitInjection == 7)Led_Debit_9ml_mn = ON;
			  else if(Etat.DebitInjection == 8)Led_Debit_10ml_mn = ON;

}
/********************************************************
Moteur seringue:
-- Codeur 16 impulsions par tour
-- Reducteur 28:1
--> 448 impulsions par tour

*********************************************************/
void MoteurSeringueFoward(char ValeurPwm)
{
	SensSeringue = 1;
	IN1 = 1;
	IN2 = 0;	
//	CCPR1L = ValeurPwm;
	ENA = 1;
	TempoArretAuto = 0;
}
void MoteurSeringueReverse(char ValeurPwm)
{
	SensSeringue = 0;
	IN1 = 0;
	IN2 = 1;
//	CCPR1L = ValeurPwm;
	ENA = 1;
	
}
void MoteurSeringueBrake(char ValeurPwm)
{
	SensSeringue = 0;
	IN1 = 0;
	IN2 = 0;
//	CCPR1L = ValeurPwm;
	ENA = 1;

}
void MoteurSeringueRL(void)
{
	SensSeringue = 0;
	IN1 = 0;
	IN2 = 0;
//	CCPR1L = 0xFE;
	ENA = 0;

}
/*********************************************************
Moteur Chariot:
-- Codeur 16 impulsions par tour
-- Reducteur 41:1
--> 656 impulsions par tour MAX 708pts pour 15mm soit 47pts/mm
**********************************************************/
void MoteurChariotFoward(char ValeurPwm)
{
	SensChariot = FOWARD;
	IN3 = 1;
	IN4 = 0;	
	//CCPR2L = ValeurPwm;
	ENB = 1;
}
void MoteurChariotReverse(char ValeurPwm)
{
	SensChariot = REVERSE;
	IN3 = 0;
	IN4 = 1;
//	CCPR2L = ValeurPwm;
	ENB = 1;
}
void MoteurChariotBrake(char ValeurPwm)
{
	SensChariot = ARRET;
	IN3 = 0;
	IN4 = 0;
//	CCPR2L = ValeurPwm;
	ENB = 1;
}
void MoteurChariotRL(void)
{
	SensChariot = ARRET;
	IN3 = 0;
	IN4 = 0;
//	CCPR2L = 0xFF;
	ENB = 0;
}
/********************************************************
					MODE CONTINU
*********************************************************/
void GestionMenuModeContinu(void)
{

	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_CONTINU; 

	Led_Touche_Fonction_Continu = ON;
	Led_Touche_Aiguille=ON;
	Led_Touche_Seringue=ON;
	Led_Touche_Profondeur=ON;
	Led_Touche_DebitInjection=ON;

	if(ValeurClavier != OldValeurClavier)
		{
		BUZZER = ON;
		switch(ValeurClavier)
			{
			case TOUCHE_AIGUILLE		:
										if(Etat.Aiguille < MENU_AIGUILLE_MAX)Etat.Aiguille++;
										else Etat.Aiguille = AIGUILLE_4MM;;
										EcritureCharEeprom(ADD_AIGUILLE,Etat.Aiguille);
										CalculProfondeurMax(MENU_CONTINU);
										CalculZeroAiguille();
										FlagInit = ON;
										break;
			case TOUCHE_SERINGUE		:
										if(Etat.Seringue < MENU_SERINGUE_MAX) Etat.Seringue++;
										else Etat.Seringue = SERINGUE_1ML;
										EcritureCharEeprom(ADD_SERINGUE,Etat.Seringue);
										CalculDebitInjectionMax();

										if((Etat.Seringue == SERINGUE_10ML)||(Etat.Seringue == SERINGUE_5ML)) Etat.DebitInjection = DEBITINJECTION_5ML;
										else Etat.DebitInjection= DEBITINJECTION_3ML;
										EcritureCharEeprom(ADD_DEBIT_INJECTION,Etat.DebitInjection);
										CalculNbPulseQuantiteContinu();////
										break;
			case TOUCHE_PROFONDEUR		:
										if(Etat.Profondeur < Etat.ProfondeurMax) Etat.Profondeur++;
										else Etat.Profondeur = PROFONDEUR_0MM;
										EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
										//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NB_FREIN_CH;
										//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
										
										
										break;
			case TOUCHE_DEBITINJECTION	:
										if(Etat.DebitInjection < DebitInjectionMax) Etat.DebitInjection++;
										else Etat.DebitInjection = DEBITINJECTION_3ML;
 										EcritureCharEeprom(ADD_DEBIT_INJECTION,Etat.DebitInjection);
										CalculNbPulseQuantiteContinu();
										break;
			case TOUCHE_MESOPERFUSION	:Menu = MENU_MESOPERFUSION;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_STANDARD		:Menu = MENU_STANDARD;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_RAFALE			:Menu = MENU_RAFALE;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
//			case TOUCHE_CONTINU			:Menu = MENU_CONTINU;break;
			default:BUZZER = OFF;
					break;
			}
		}
	else 
		{
		if((ValeurClavier == TOUCHE_PROFONDEUR))FlagRetourChariot = 1;
		else FlagRetourChariot = 0;
		}

	OldValeurClavier = ValeurClavier;

	LectureBoutonPlusMoins(MENU_CONTINU);
		
}
/********************************************************
					MODE STANDARD
*********************************************************/
void GestionMenuModeStandard(void)
{
//unsigned char ValeurProfondeurMax = 0;

	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_STANDARD;
	//Led_Touche_Fonction_Rafale = ON;
	Led_Touche_Dose_Standard = ON;
	//Led_Touche_Mesoperfusion = ON;
	//Led_Touche_Fonction_Continu = ON;

	Led_Touche_Aiguille = ON;
	Led_Touche_Seringue = ON;
	Led_Touche_Profondeur = ON;
	//Led_Touche_DebitInjection=ON;
	Led_Touche_DureeInjection = ON;

	if(ValeurClavier != OldValeurClavier)
		{
		BUZZER = ON;
		switch(ValeurClavier)
			{
			
			case TOUCHE_AIGUILLE		:
										if(Etat.Aiguille < MENU_AIGUILLE_MAX)Etat.Aiguille++;
										else Etat.Aiguille = AIGUILLE_4MM;
										EcritureCharEeprom(ADD_AIGUILLE,Etat.Aiguille);
										CalculProfondeurMax(MENU_STANDARD);
										CalculZeroAiguille();
										FlagInit = ON;
										break;
			case TOUCHE_SERINGUE		:
										if(Etat.Seringue < MENU_SERINGUE_MAX) Etat.Seringue++;
										else Etat.Seringue = SERINGUE_1ML;
										EcritureCharEeprom(ADD_SERINGUE,Etat.Seringue);
										CalculNbPulseQuantiteStandard();
										break;
			case TOUCHE_PROFONDEUR		:
										if(Etat.Profondeur < Etat.ProfondeurMax) Etat.Profondeur++;
										else Etat.Profondeur = PROFONDEUR_0MM;
										EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
										//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM + VALEUR_GARDE - NB_FREIN_CH;
										//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
										
										break;
			case TOUCHE_DUREEINJECTION	:
										if(Etat.DureeInjection < MENU_DUREE_MAX) Etat.DureeInjection++;
										else Etat.DureeInjection = 0;
										EcritureCharEeprom(ADD_DUREE_INJECTION,Etat.DureeInjection); 
										//TempoInjection = Etat.DureeInjection * _1s;
										break;

			case TOUCHE_MESOPERFUSION	:Menu = MENU_MESOPERFUSION;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			//case TOUCHE_STANDARD		:Menu = MENU_STANDARD;break;
			case TOUCHE_RAFALE			:Menu = MENU_RAFALE;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_CONTINU			:Menu = MENU_CONTINU;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;

			default:BUZZER = OFF;
					break;
			}
		}
	else
		{
		if((ValeurClavier == TOUCHE_PROFONDEUR))FlagRetourChariot = 1;
		else FlagRetourChariot = 0;
		}

	OldValeurClavier = ValeurClavier;

	LectureBoutonPlusMoins(MENU_STANDARD);	
}
/********************************************************
			MODE MESO PERFUSION
*********************************************************/
void GestionMenuModeMesoPerfusion(void)
{
unsigned char ValeurProfondeurMax = 0;

	//Led_Touche_Fonction_Rafale = ON;
	//Led_Touche_Dose_Standard = ON;
	if(FctGachetteMeso==1)Led_Touche_Mesoperfusion = Blink_1s;
	else Led_Touche_Mesoperfusion = ON;
	//Led_Touche_Fonction_Continu = ON;
	
	Led_Touche_TempsEntreDose = ON;
	Led_Touche_Profondeur = OFF;

	if(ValeurClavier != OldValeurClavier)
		{
		BUZZER = ON;
		switch(ValeurClavier)
			{
			
			case TOUCHE_TEMPSENTREDOSE	:
										if(Etat.TempsEntreDose < MENU_TEMPSENTREDOSE_MAX)Etat.TempsEntreDose++;
										else Etat.TempsEntreDose = 0;
										EcritureCharEeprom(ADD_TEMPS_ENTRE_DOSE,Etat.TempsEntreDose);
										TempoEntreDoses = ((unsigned int)Etat.TempsEntreDose * _8s); 
										
										if (TempoEntreDoses == 0) TempoEntreDoses = _1s;
										break;
			//case TOUCHE_MESOPERFUSION	:Mode = MENU_MESOPERFUSION;break;
			case TOUCHE_STANDARD		:Menu = MENU_STANDARD;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_RAFALE			:Menu = MENU_RAFALE;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_CONTINU			:Menu = MENU_CONTINU;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_PROFONDEUR		:break;	
													
			default:BUZZER = OFF;
					break;
			}
		}
	else 
		{
		if((ValeurClavier == TOUCHE_PROFONDEUR))FlagRetourChariot = 1;
		else FlagRetourChariot = 0;
		}

	OldValeurClavier = ValeurClavier;

	LectureBoutonPlusMoins(MENU_MESOPERFUSION);

}
/********************************************************
			MENU RAFALE
*********************************************************/
void GestionMenuModeRafale(void)
{
unsigned char ValeurProfondeurMax = 0;

	 
	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_RAFALE;

	Led_Touche_Fonction_Rafale = ON;

	//Led_Touche_Dose_Standard = ON;
	//Led_Touche_Mesoperfusion = ON;
	//Led_Touche_Fonction_Continu = ON;
	Led_Touche_Aiguille = ON;
	Led_Touche_Seringue = ON;
	Led_Touche_Profondeur = ON;
	Led_Touche_FreqInjection = ON;

	if(ValeurClavier != OldValeurClavier)
		{
		BUZZER = ON;
		switch(ValeurClavier)
			{
			case TOUCHE_AIGUILLE		:
										if(Etat.Aiguille < MENU_AIGUILLE_MAX)Etat.Aiguille++;
										else Etat.Aiguille = AIGUILLE_4MM;;
										EcritureCharEeprom(ADD_AIGUILLE,Etat.Aiguille);
										Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;
										FlagInit = ON;
										CalculZeroAiguille();
										break;
			case TOUCHE_SERINGUE		:
										if(Etat.Seringue < MENU_SERINGUE_MAX) Etat.Seringue++;
										else Etat.Seringue = SERINGUE_1ML;
										EcritureCharEeprom(ADD_SERINGUE,Etat.Seringue);
										CalculFrequenceMax();
										if(Etat.Frequence > FrequenceMax)
											{
											Etat.Frequence = FREQUENCE_100;
											EcritureCharEeprom(ADD_FREQUENCE,Etat.Frequence);
											CalculTempoFrequence();
											}
										CalculNbPulseQuantiteRafale();
										break;
			case TOUCHE_PROFONDEUR		:
										//CalculProfondeurMax();
										Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;
										if(Etat.Profondeur < Etat.ProfondeurMax) Etat.Profondeur++;
										else Etat.Profondeur = PROFONDEUR_0MM;
										EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
										//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM + VALEUR_GARDE - NB_FREIN_CH;
										//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
										NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
										
										break;
			case TOUCHE_FREQUENCEINJECTION	:
										//FrequenceMax = CalculFrequenceMax();
										if(Etat.Frequence < FrequenceMax) Etat.Frequence++;
										else Etat.Frequence = FREQUENCE_100;
										EcritureCharEeprom(ADD_FREQUENCE,Etat.Frequence);
										CalculTempoFrequence(); 
										break;
			case TOUCHE_MESOPERFUSION	:Menu = MENU_MESOPERFUSION;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			case TOUCHE_STANDARD		:Menu = MENU_STANDARD;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
//			case TOUCHE_RAFALE			:Menu = MENU_RAFALE;break;
			case TOUCHE_CONTINU			:Menu = MENU_CONTINU;
										 InitDefaultValue(Menu);
										 FlagInit = ON;break;
			default:BUZZER = OFF;
					break;
			}
		}
	else
		{
		if((ValeurClavier == TOUCHE_PROFONDEUR))FlagRetourChariot = 1;
		else FlagRetourChariot = 0;
		}

	OldValeurClavier = ValeurClavier;

	LectureBoutonPlusMoins(MENU_RAFALE);
}				
/********************************************************
				Gestion des leds
*********************************************************/
void GestionLeds(void)
{

	switch(Etat.DebitInjection)
		{
		case 1:Led_Debit_3ml_mn = ON;break;
		case 2:Led_Debit_4ml_mn = ON;break;
		case 3:Led_Debit_5ml_mn = ON;break;
		case 4:Led_Debit_6ml_mn = ON;break;
		case 5:Led_Debit_7ml_mn = ON;break;
		case 6:Led_Debit_8ml_mn = ON;break;
		case 7:Led_Debit_9ml_mn = ON;break;
		case 8:Led_Debit_10ml_mn = ON;break;
		default:break;
		}

	switch(Etat.Aiguille)
		{
		case AIGUILLE_4MM	:Led_Aiguille_4mm = ON;break;
		case AIGUILLE_6MM	:Led_Aiguille_6mm = ON;break;
		case AIGUILLE_13MM	:Led_Aiguille_13mm = ON;break;
		default:break;
		}

	switch(Etat.Seringue)
		{
		case 1:Led_Seringue_1ml = ON;break;
		case 2:Led_Seringue_2ml = ON;break;
		case 3:Led_Seringue_3ml = ON;break;
		case 4:Led_Seringue_5ml = ON;break;
		case 5:Led_Seringue_10ml = ON;break;
		default:break;
		}

	switch(Etat.Profondeur)
		{
		case 0:Led_Profondeur_0mm = ON;break;
		case 1:Led_Profondeur_1mm = ON;break;
		case 2:Led_Profondeur_2mm = ON;break;
		case 3:Led_Profondeur_3mm = ON;break;
		case 4:Led_Profondeur_4mm = ON;break;
		case 5:Led_Profondeur_5mm = ON;break;
		case 6:Led_Profondeur_6mm = ON;break;
		case 7:Led_Profondeur_7mm = ON;break;
		case 8:Led_Profondeur_8mm = ON;break;
		case 9:Led_Profondeur_9mm = ON;break;
		default:break;//Etat.Profondeur = 0;
		}

	switch(Etat.Frequence)
		{
		case 1:Led_Frequence_100cps_mn = ON;break;
		case 2:Led_Frequence_150cps_mn = ON;break;
		case 3:Led_Frequence_200cps_mn = ON;break;
		case 4:Led_Frequence_250cps_mn = ON;break;
		case 5:Led_Frequence_300cps_mn = ON;break;
		default:break;
		}

	switch(Etat.DureeInjection)
		{
		case 0:Led_Duree_Injection_0s = ON;break;
		case 1:Led_Duree_Injection_1s = ON;break;
		case 2:Led_Duree_Injection_2s = ON;break;
		case 3:Led_Duree_Injection_3s = ON;break;
		default:break;
		}

	switch(Etat.TempsEntreDose)
		{
		case 0:Led_TempsEntreDose_0s = ON;break;
		case 1:Led_TempsEntreDose_8s = ON;break;
		case 2:Led_TempsEntreDose_16s = ON;break;
		case 3:Led_TempsEntreDose_24s = ON;break;
		case 4:Led_TempsEntreDose_32s = ON;break;
		case 5:Led_TempsEntreDose_40s = ON;break;
		case 6:Led_TempsEntreDose_48s = ON;break;
		default:break;
		}
	
		
}
/****************************************************************
						INITIALISATION
// Mode					//Seringue		//aiguille		//Frequence
1:Injection Continu		1: 1 ml			1 : 4 mm		1 : 100cps/mn
2:Dose Standard			2: 2 ml			2 : 6 mm		2 : 150cps/mn
3:MésoPerfusion			3: 3 ml			3 : 13 mm		3 : 200cps/mn
4:Rafale				4: 5 ml							4 : 250cps/mn
						5: 10 ml						5 : 300cps/mn

//profondeur	//Débit = 3ml à 10ml/min	//Temps entre dose	//Duree injection
0 : 0mm			1 :	3ml/min					0 : 0s				0 : 0s
1 : 1mm			2 :	4ml/min					1 : 8s				1 : 1s
2 : 2mm			3 :	5ml/min					2 : 16s				2 : 2s
3 : 3mm			4 :	6ml/min					3 : 24s				3 : 3s
4 : 4mm			5 : 7ml/min					4 : 32s
5 : 5mm			6 :	8ml/min					5 : 40s
6 : 6mm			7 :	9ml/min					6 : 48s
7 : 7mm			8 :	10ml/min
8 : 8mm			
9 : 9mm

******************************************************************/
void InitDefaultValue(unsigned char ModeT)
{

switch (ModeT)
	{
	case MENU_CONTINU ://1:Injection continu
        Etat.Aiguille = AIGUILLE_4MM;//4mm
		Etat.Seringue = SERINGUE_10ML;//10ml
		Etat.Profondeur = PROFONDEUR_2MM;//2mm
		Etat.DebitInjection = 3;//
		Etat.TempsEntreDose = 0xFF;
		Etat.Frequence = 0xFF;
		Etat.DureeInjection = 0xFF;
		CalculNbPulseQuantiteContinu();
		CalculProfondeurMax(MENU_CONTINU);
		CalculDebitInjectionMax();
		NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;

	case MENU_STANDARD ://2:injection standard                          
		Etat.Aiguille = AIGUILLE_4MM;//4mm
		Etat.Seringue = SERINGUE_10ML;//10ml
		Etat.Profondeur = PROFONDEUR_2MM;//2mm
		Etat.DebitInjection = 0xFF;
		Etat.TempsEntreDose = 0xFF;
		Etat.Frequence = 0xFF;
		Etat.DureeInjection = 0;
		TempoInjection = 0;
		CalculNbPulseQuantiteStandard();
		CalculProfondeurMax(MENU_STANDARD);
		NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;

	case MENU_MESOPERFUSION ://3:Mesoperfusion
		Etat.Aiguille = AIGUILLE_13MM;//13mm
		Etat.Seringue = SERINGUE_10ML;//10ml
		Etat.Profondeur = 0xFF;
		Etat.DebitInjection = 0xFF;
		Etat.TempsEntreDose = 1;
		Etat.Frequence = 0xFF;
		Etat.DureeInjection = 0xFF;
		NbPulseQuantiteAInjecter = NB_PULSE_MESO_PERFUSION;
		TempoEntreDoses = ((unsigned int)Etat.TempsEntreDose * _8s);
		if (TempoEntreDoses == 0) TempoEntreDoses = _1s; 
		CalculProfondeurMax(MENU_MESOPERFUSION);
		//NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur];
		break;

	case MENU_RAFALE ://4:rafale                         
		Etat.Aiguille = AIGUILLE_4MM;//4mm
		Etat.Seringue = SERINGUE_10ML;//10ml
		Etat.Profondeur = PROFONDEUR_2MM;//2mm
		Etat.DebitInjection = 0xFF;
		Etat.TempsEntreDose = 0xFF;
		Etat.Frequence = FREQUENCE_100;//100cp/mn
		Etat.DureeInjection = 0xFF;
		CalculNbPulseQuantiteRafale();
		CalculTempoFrequence();
		Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;
		CalculFrequenceMax();
		NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;
	
	default:break;
	}

//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM;
//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM + VALEUR_GARDE - NB_FREIN_CH;
//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];

CalculZeroAiguille();

MenuSauvegarde = ModeT;
SauvegardeMemoire();

}
/*****************************************************************
					Initialisation au demarrage
******************************************************************/
void InitDemarrage(void)
{

LectureMemoire();

switch (MenuSauvegarde)
	{
	case MENU_CONTINU ://1:Injection continu
		Etat.TempsEntreDose = 0xFF;
		Etat.Frequence = 0xFF;
		Etat.DureeInjection = 0xFF;
		CalculNbPulseQuantiteContinu();
		CalculProfondeurMax(MENU_CONTINU);
		CalculDebitInjectionMax();
		NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;

	case MENU_STANDARD ://2:injection standard 
		Etat.DebitInjection = 0xFF;
		Etat.TempsEntreDose = 0xFF;
		Etat.Frequence = 0xFF;  
		TempoInjection = Etat.DureeInjection * _1s;                       
		CalculNbPulseQuantiteStandard();
		CalculProfondeurMax(MENU_STANDARD);
		NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;

	case MENU_MESOPERFUSION ://3:Mesoperfusion
		Etat.Profondeur = 0xFF;
		Etat.DebitInjection = 0xFF;
		Etat.Frequence = 0xFF;
		Etat.DureeInjection = 0xFF;
		NbPulseQuantiteAInjecter = NB_PULSE_MESO_PERFUSION;
		TempoEntreDoses = ((unsigned int)Etat.TempsEntreDose * _8s);
		if (TempoEntreDoses == 0) TempoEntreDoses = _1s; 
		CalculProfondeurMax(MENU_MESOPERFUSION);
		break;

	case MENU_RAFALE ://4:rafale
		Etat.DebitInjection = 0xFF;
		Etat.TempsEntreDose = 0xFF;
		Etat.DureeInjection = 0xFF;                         
		CalculNbPulseQuantiteRafale();
		CalculTempoFrequence();
		Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;
		CalculFrequenceMax();
		NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
		break;
	
	default:break;
	}

//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM;
//NbPulseProfondeur = (unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM + VALEUR_GARDE - NB_FREIN_CH;
//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];


CalculZeroAiguille();

}
/**********************************************************
				RAZ des leds
***********************************************************/
void RAZLeds(void)
{

	LedU1A.c = 0xFF;
	LedU1B.c = 0xFF;
	LedU2A.c |= 0xF0;
	LedU2B.c = 0xFF;
	LedU3A.c = 0xFF;
	LedU3B.c = 0xFF;
	LedU4B.c = 0xFF;
	//Led_Touche_Profondeur = OFF;
}
/*************************************************************
Mode Continu: Avance chariot,Injection tant que gachette appuyee,puis retour chariot si gachette lachee 
Antigoutte = 1/2 tour = 224 impulsions
*************************************************************/
void GestionMoteurModeContinu(void)
{
static int NbPulse=0;

	CompteurCoups++;
	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_CONTINU;
	
	NbPulseChariot = 0;	
FlagHalt=1;
	while (NbPulseChariot <= NbPulseProfondeur) MoteurChariotFoward(0x00);
	while (NbPulseChariot <= NbPulseProfondeur) MoteurChariotFoward(0x00);
	MoteurChariotReverse(0x00);
	DelayMs(6);
	MoteurChariotBrake(0x00); 
FlagHalt=0;	
	while (GACHETTE == 1) 
		{
		ClrWdt();
		TempoSecondeInjection = _250Ms;
		NbPulseSeringue = 0;
		
		while(NbPulseSeringue < (NbPulseQuantiteAInjecter)) MoteurSeringueFoward(0x00);
		while(NbPulseSeringue < (NbPulseQuantiteAInjecter)) MoteurSeringueFoward(0x00);

		MoteurSeringueReverse(0x00);
		DelayMs(9);
		MoteurSeringueBrake(0x00);	
		
		while(TempoSecondeInjection > 0);
		NbSeFW = NbPulseSeringue;
		}
	

	NbPulse = ZeroAiguille + NbPulseFrein[Etat.Profondeur];

	MaxAiguille = CompteurPulseCh;// memoire
FlagHalt=1;
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);
	MoteurChariotFoward(0x00);
	DelayMs(6);
	MoteurChariotBrake(0x00);
	AjustePositionChariot();//
FlagHalt=0;	
//antigoutte
	NbPulseSeringue = 0;
	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0x00);
	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0x00);
	MoteurSeringueFoward(0x00);
	DelayMs(9);
	MoteurSeringueBrake(0x00);

	PositionZAiguille = CompteurPulseCh;
//	DelayMs(100);
	MoteurSeringueRL();

	MoteurChariotRL();

//	NbSeRW = NbPulseSeringue;	
//	NbChRW = NbPulse + NbPulseChariot; 
 	
}
/*********************************************************

**********************************************************/
/*************************************************************
Mode Standard = Appuy gachette unique ==>
Avance Chariot,injection seringue,attente x S, retour chariot,
1T = 448 impulsions
Antigoutte = 1/15T = 448/15 = 30
*************************************************************/
void GestionMoteurModeStandard(void)
{
static int NbPulse=0,DeriveCh;//

	//FLAGDEBUG = 1;

	CompteurCoups++;
	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_STANDARD;

	NbPulse = NbPulseProfondeur;

	NbPulseChariot = 0;
FlagHalt=1;
	while (NbPulseChariot <= NbPulse) MoteurChariotFoward(0);
	while (NbPulseChariot <= NbPulse) MoteurChariotFoward(0);

	MoteurChariotReverse(0);
	//DelayMs(6);
	DelayMs(6);

	MoteurChariotBrake(0); 

	MaxAiguille = CompteurPulseCh;// memoire
	NbPulseSeringue = 0;
	
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
	MoteurSeringueReverse(0);
	DelayMs(9);
	NbSeFW = NbPulseSeringue;

	MoteurSeringueBrake(0);	
	
	TempoInjection = Etat.DureeInjection * _1s; 
	while(TempoInjection > 0);

	NbChFW = NbPulseChariot;
	NbPulse = ZeroAiguille + NbPulseFrein[Etat.Profondeur];
	MaxAiguille = CompteurPulseCh;
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);
	MoteurChariotFoward(0);
//	DelayMs(6);
	DelayMs(6);

	MoteurChariotBrake(0);

	AjustePositionChariot();

//antigoutte
	NbSeFW1 = NbPulseSeringue;
	NbPulseSeringue = 0;

	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0);
	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0);
	MoteurSeringueFoward(0);
	DelayMs(9);

	MoteurSeringueBrake(0);
FlagHalt=0;

	PositionZAiguille = CompteurPulseCh;

	MoteurSeringueRL();
	
	MoteurChariotRL();

	//NbChRW = NbPulse + NbPulseChariot;
	//NbSeRW = NbPulseSeringue;

}
/**********************************************************
			Moteur en mode Meso
***********************************************************/
void GestionMoteurModeMesoPerfusion(void)
{
	Bip = NB_BIP;
	if(Etat.TempsEntreDose != 0)NbPulseQuantiteAInjecter = NB_PULSE_MESO_PERFUSION;
	else NbPulseQuantiteAInjecter = NB_PULSE_MESO_PERFUSION_1S;

	NbPulseSeringue = 0;
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
//	Nb1 = NbPulseSeringue;
	MoteurSeringueReverse(0);
	DelayMs(8);
	MoteurSeringueBrake(0);	

	NbSeFW = NbPulseSeringue;
	
	
}
/***********************************************************
			Moteur en mode rafale
************************************************************/
void GestionMoteurModeRafale(void)
{
static int NbPulse=0,DeriveCh,DeriveChRetour;

	CompteurCoups++;
	NbPulseAntigoutte = NB_PULSE_ANTIGOUTTE_RAFALE;

	DeriveCh = ZeroAiguille - CompteurPulseCh;//par rapport a la position chariot

	NbPulse = NbPulseProfondeur + DeriveCh;//dérive mécanique

	NbPulseChariot = 0;
FlagHalt=1;
	while (NbPulseChariot <= NbPulse) MoteurChariotFoward(0);
	while (NbPulseChariot <= NbPulse) MoteurChariotFoward(0);
	MoteurChariotReverse(0);
	DelayMs(6);
	MoteurChariotBrake(0); 

	MaxAiguille = CompteurPulseCh;// memoire

	NbPulseSeringue = 0;
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
	while (NbPulseSeringue <= NbPulseQuantiteAInjecter) MoteurSeringueFoward(0);
	NbSeFW = NbPulseSeringue;
	MoteurSeringueReverse(0);
	DelayMs(9);
	MoteurSeringueBrake(0);	

	//DeriveChRetour = PositionZAiguille - ZeroAiguille;
	NbPulse = ZeroAiguille + NbPulseFrein[Etat.Profondeur];

	//NbPulse = NbPulseProfondeur;
	NbPulseChariot = 0;
	MaxAiguille = CompteurPulseCh;
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);
	while (CompteurPulseCh > NbPulse) MoteurChariotReverse(0x00);

	MoteurChariotFoward(0);
	DelayMs(6);

	MoteurChariotBrake(0);

	PositionZAiguille = CompteurPulseCh;
FlagHalt=0;
//	NbChRW = NbPulse + NbPulseChariot;
//	NbSeFW1 = NbPulseSeringue; 

	FlagFinRafale = 1;

}
/************************************************************
				Stop goutte en rafale
************************************************************/
void StopGoutteRafale(void)
{

	NbPulseSeringue = 0;
	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0);
	while (NbPulseSeringue <= NbPulseAntigoutte) MoteurSeringueReverse(0);
	MoteurSeringueFoward(0);
	DelayMs(8);
	MoteurSeringueBrake(0);
	
	FlagStopGoutte = 0;
	DelayMs(100);
	MoteurSeringueRL();
	MoteurChariotRL();
	NbSeRW = NbPulseSeringue;
	FlagInit = ON;
}
/***********************************************************
Longueur vis 15mm --> 656 impulsions --> 44 impulsions/mm

************************************************************/
/*void InitPositionChariot(void)
{
FlagHalt=1;
	while ((FDC_CHARIOT == 1)&&(CourantChariot < SEUIL_SURCOURANT_CHARIOT)) MoteurChariotReverse(0x00);
//	CompteurPulseCh = 0;
//	NbPulseChariot = 0;
	Cpt=0;
	MoteurChariotBrake(0x00);
	DelayMs(100);
	CompteurPulseCh = 0;
	NbPulseChariot = 0;
//	NbChRW = NbPulseChariot;
//	CompteurPulseCh = 0;
//	NbPulseChariot = 0;

	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
	MoteurChariotReverse(0);
	DelayMs(6);
	MoteurChariotBrake(0);
FlagHalt=0;	
	DelayMs(100);
	MoteurSeringueRL();
	MoteurChariotRL();
	NbChFW = NbPulseChariot;
	
}*/

/*****************************************************************
La quantite est divisee par 4 car injection toute les 250ms/1s
******************************************************************/
void CalculNbPulseQuantiteContinu(void)
{

	switch(Etat.Seringue)
		{
		case 1:NbPulseQuantiteAInjecter = 786;break;//=1ml/786 --> 150=0,6 l/mn
		case 2:switch(Etat.DebitInjection)//2ml
					{
					case 1:NbPulseQuantiteAInjecter = 210;break;//3ml/mn
					case 2:NbPulseQuantiteAInjecter = 280;break;//4ml/mn
					case 3:NbPulseQuantiteAInjecter = 350;break;//5ml/mn
					case 4:NbPulseQuantiteAInjecter = 420;break;//6ml/mn
					case 5:NbPulseQuantiteAInjecter = 490;break;//7ml/mn
					case 6:NbPulseQuantiteAInjecter = 560;break;//8ml/mn
					default:NbPulseQuantiteAInjecter = 210;break;//
					}
				break;
		case 3:switch(Etat.DebitInjection)//3ml
					{
					case 1:NbPulseQuantiteAInjecter = 201;break;//3ml/mn
					case 2:NbPulseQuantiteAInjecter = 268;break;//4ml/mn
					case 3:NbPulseQuantiteAInjecter = 335;break;//5ml/mn
					case 4:NbPulseQuantiteAInjecter = 402;break;//6ml/mn
					case 5:NbPulseQuantiteAInjecter = 469;break;//7ml/mn
					case 6:NbPulseQuantiteAInjecter = 536;break;//8ml/mn
					default:NbPulseQuantiteAInjecter = 201;break;//
					}
				break;
		case 4:switch(Etat.DebitInjection)//5ml
					{
					case 1:NbPulseQuantiteAInjecter = 105;break;//3ml/mn
					case 2:NbPulseQuantiteAInjecter = 138;break;//4ml/mn
					case 3:NbPulseQuantiteAInjecter = 166;break;//5ml/mn
					case 4:NbPulseQuantiteAInjecter = 200;break;//6ml/mn
					case 5:NbPulseQuantiteAInjecter = 232;break;//7ml/mn
					case 6:NbPulseQuantiteAInjecter = 265;break;//8ml/mn
					case 7:NbPulseQuantiteAInjecter = 298;break;//9ml/mn
					case 8:NbPulseQuantiteAInjecter = 332;break;//10ml/mn
					default:NbPulseQuantiteAInjecter = 105;break;
					}
				break;
		case 5:switch(Etat.DebitInjection)//10ml
					{
					case 1:NbPulseQuantiteAInjecter = 76;break;//57=3ml/mn
					case 2:NbPulseQuantiteAInjecter = 94;break;//76=4ml/mn
					case 3:NbPulseQuantiteAInjecter = 115;break;//95=5ml/mn
					case 4:NbPulseQuantiteAInjecter = 137;break;//114=6ml/mn
					case 5:NbPulseQuantiteAInjecter = 155;break;//133=7ml/mn
					case 6:NbPulseQuantiteAInjecter = 177;break;//152=8ml/mn
					case 7:NbPulseQuantiteAInjecter = 196;break;//171=9ml/mn
					case 8:NbPulseQuantiteAInjecter = 215;break;//190=10ml/mn
					default:NbPulseQuantiteAInjecter = 76;break;//
					}
				break;
		default:break;
		}
	NbPulseQuantiteAInjecter -= NB_FREIN_SE;//non, ne marche pas
}
/****************************************************************
 calcule le nombre de pulse pour injecter en mode standard
*****************************************************************/
void CalculNbPulseQuantiteStandard(void)
{

	if(Etat.Seringue == SERINGUE_1ML)NbPulseQuantiteAInjecter = 1600;//1494;//1363;//1674;
	else if(Etat.Seringue == SERINGUE_2ML)NbPulseQuantiteAInjecter = 480;//510;//466;
		 else if(Etat.Seringue == SERINGUE_3ML)NbPulseQuantiteAInjecter = 471;//430;
			  else if(Etat.Seringue == SERINGUE_5ML)NbPulseQuantiteAInjecter = 233;//262;//202;
				   else NbPulseQuantiteAInjecter = 160;//170//130;//SERINGUE_10ML	

	NbPulseQuantiteAInjecter -= NB_FREIN_SE;
}
/****************************************************************
 calcule le nombre de pulse pour injecter en mode Rafale
*****************************************************************/
void CalculNbPulseQuantiteRafale(void)
{

	if(Etat.Seringue == SERINGUE_1ML)NbPulseQuantiteAInjecter = 270;//OK
	else if(Etat.Seringue == SERINGUE_2ML)NbPulseQuantiteAInjecter = 92;//OK
		 else if(Etat.Seringue == SERINGUE_3ML)NbPulseQuantiteAInjecter = 82;//OK
			  else if(Etat.Seringue == SERINGUE_5ML)NbPulseQuantiteAInjecter = 52;//45;//38;//OK
				   else NbPulseQuantiteAInjecter = 38;//30//24;//27;//SERINGUE_10ML	//OK

	NbPulseQuantiteAInjecter -= NB_FREIN_SE;
}
/***********************************************************
permet de calculer la profondeur max en fonction du type d'aiguille
************************************************************/
void CalculProfondeurMax(unsigned char ModeApp)
{

	switch(Etat.Aiguille)
		{
		case AIGUILLE_4MM	:Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;break;
		case AIGUILLE_6MM	:Etat.ProfondeurMax = PROFONDEUR_MAX_6MM;break;
		case AIGUILLE_13MM	:Etat.ProfondeurMax = PROFONDEUR_MAX_9MM;break;
		default:Etat.ProfondeurMax = PROFONDEUR_MAX_4MM;break;
		}
	if(Etat.Profondeur <= 9)
		{
		if(Etat.Profondeur > Etat.ProfondeurMax)//on reajuste la profondeur
			{
			Etat.Profondeur = PROFONDEUR_4MM;
			EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
			//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NB_FREIN_CH;
			//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
			//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];

			if(ModeApp == MENU_RAFALE)NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
			else if(ModeApp == MENU_STANDARD)NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
				 else NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];

			}
		}
}
/***********************************************************
permet de calculer le zero aiguille
************************************************************/
void CalculZeroAiguille(void)
{

	switch(Etat.Aiguille)
		{
		case AIGUILLE_4MM	:ZeroAiguille = ZERO_AIGUILLE_4MM ;break;
		case AIGUILLE_6MM	:ZeroAiguille = ZERO_AIGUILLE_6MM ;break;
		case AIGUILLE_13MM	:ZeroAiguille = ZERO_AIGUILLE_13MM ;break;
		default:ZeroAiguille = ZERO_AIGUILLE_4MM ;break;
		}

	PositionZAiguille = ZeroAiguille;	
}
/************************************************************

*************************************************************/
/*void CalculTempoFrequence(void)
{

	if(Etat.Frequence == FREQUENCE_100) TempoFrequence = 12;//100cp/mn = 0,6s
	else if(Etat.Frequence == FREQUENCE_150) TempoFrequence = 8;//150cp/mn = 0,4s
		 else if(Etat.Frequence == FREQUENCE_200) TempoFrequence = 6;//200cp/mn = 0,3s
			  else if(Etat.Frequence == FREQUENCE_250) TempoFrequence = 5;//250cp/mn = 0,24s
				   else TempoFrequence = 4;//FREQUENCE_300 = 300cp/mn = 0,2s
}*/
/************************************************************

*************************************************************/
void CalculTempoFrequence(void)
{

	if(Etat.Frequence == FREQUENCE_100) TempoFrequence = 24;//100cp/mn = 0,6s
	else if(Etat.Frequence == FREQUENCE_150) TempoFrequence = 16;//150cp/mn = 0,4s
		 else if(Etat.Frequence == FREQUENCE_200) TempoFrequence = 12;//200cp/mn = 0,3s
			  else if(Etat.Frequence == FREQUENCE_250) TempoFrequence = 9;//250cp/mn = 0,24s
				   else TempoFrequence = 8;//FREQUENCE_300 = 300cp/mn = 0,2s
}
/**************************************************************

***************************************************************/
void CalculFrequenceMax(void)
{

	if((Etat.Seringue == SERINGUE_5ML)||(Etat.Seringue == SERINGUE_10ML)) FrequenceMax = FREQUENCE_MAX_300;
	else
		{
		if((Etat.Seringue == SERINGUE_3ML)||(Etat.Seringue == SERINGUE_2ML))FrequenceMax = FREQUENCE_MAX_250;
		else FrequenceMax = FREQUENCE_MAX_150;//SERINGUE_1ML 
		} 

}
/***************************************************************

***************************************************************/
void CalculDebitInjectionMax(void)
{

	if((Etat.Seringue == SERINGUE_5ML)||(Etat.Seringue == SERINGUE_10ML)) DebitInjectionMax = DEBITINJECTION_10ML;
	else
		{
		if((Etat.Seringue == SERINGUE_3ML)||(Etat.Seringue == SERINGUE_2ML))DebitInjectionMax = DEBITINJECTION_8ML;
		else DebitInjectionMax = DEBITINJECTION_3ML;;//SERINGUE_1ML 
		} 

}
/**************************************************************

***************************************************************/
void GestionDefautCourant(void)
{
	Led_Touche_Profondeur = OFF;
	MoteurSeringueRL();
	switch(TempoDefautCourant)
		{
		case 0		:Bip = NB_BIP;break;
		case _200Ms	:Bip = NB_BIP;break;
		case _400Ms	:Bip = NB_BIP;break;
		case _600Ms	:Bip = NB_BIP;break;
		case _800Ms	:MARCHE = 0;break;
		default:break;
		}

}
/**************************************************************

***************************************************************/
void GestionDefautBatterie(void)
{
	
//	RAZLeds();
//	EcritureLeds();
	Led_Touche_Profondeur = OFF;
	switch(TempoDefautBatterie)
		{
		case 0		:Bip = NB_BIP;break;
		case _200Ms	:Bip = NB_BIP;break;
		case _400Ms	:Bip = NB_BIP;break;
		case _600Ms	:Bip = NB_BIP;break;
		case _800Ms	:MARCHE = 0;break;
		default:break;
		}

}
/***************************************************************

****************************************************************/
/*void GestionInit(void)
{
int Toto;

	FlagInit = OFF;
//	NbPulseChariot = 0;
FlagHalt=1;
	while ((FDC_CHARIOT == 1)&&(CourantChariot < SEUIL_SURCOURANT_CHARIOT)) MoteurChariotReverse(0x00);
	//while ((FDC_CHARIOT == 1)) MoteurChariotReverse(0x00);
	CompteurPulseCh = 0;
	NbPulseChariot = 0;
	MoteurChariotBrake(0x00);
	DelayMs(100);
	Delay10TCYx(4);
	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);

	MoteurChariotReverse(0);
	DelayMs(6);
	MoteurChariotBrake(0);
//	NbChFW = NbPulseChariot; 
FlagHalt=0;
	Bip = NB_BIP;
	DelayMs(100);
	Bip = NB_BIP;
	DelayMs(100);
	NbChFW = NbPulseChariot;
	MoteurSeringueRL();
	MoteurChariotRL();

	CompteurCoups = 0;
	
	
}*/
/**************************************************************
Lecture des bouton profondeur PLUS et Profondeur MOINS
***************************************************************/
void LectureBoutonPlusMoins(unsigned char ModeApp)
{

	if(((PLUS1 == 0)&&(OldPlus1 == 1))||((PLUS2 == 0)&&(OldPlus2 == 1)))
		{
		FlagPlus = 1;
		if(Etat.Profondeur < Etat.ProfondeurMax) Etat.Profondeur++;
		else Etat.Profondeur = PROFONDEUR_0MM;
		EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
		//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NB_FREIN_CH;
		//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
		//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
		if(ModeApp == MENU_RAFALE)NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
			else if(ModeApp == MENU_STANDARD)NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
				 else NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];


		Bip = NB_BIP;
		}
	OldPlus1 = PLUS1;
	OldPlus2 = PLUS2;

	//if((PLUS1 == 1)&&(PLUS2 == 1))

	if(((MOINS1 == 0)&&(OldMoins1 == 1))||((MOINS2 == 0)&&(OldMoins2 == 1))||(FlagCorrection))
		{
		FlagCorrection = 0;
		FlagPlus = 0;
		if(Etat.Profondeur > 0 ) Etat.Profondeur--;
		else Etat.Profondeur = Etat.ProfondeurMax;

		EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
		//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NB_FREIN_CH;
		//NbPulseProfondeur = ((unsigned int)Etat.Profondeur*NB_PULSE_PAR_MM)+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
		//NbPulseProfondeur = (NbPulseProf[Etat.Profondeur])+ VALEUR_GARDE - NbPulseFrein[Etat.Profondeur];
		if(ModeApp == MENU_RAFALE)NbPulseProfondeur = NbPulProRafale[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
			else if(ModeApp == MENU_STANDARD)NbPulseProfondeur = NbPulProStandard[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];
				 else NbPulseProfondeur = NbPulProContinu[Etat.Aiguille][Etat.Profondeur]- NbPulseFrein[Etat.Profondeur];

		Bip = NB_BIP;
		}
	OldMoins1 = MOINS1;
	OldMoins2 = MOINS2;
}
/**************************************************************

***************************************************************/
void BipBipArret(void)
{
	BUZZER = ON;
	DelayMs(100);
	BUZZER = OFF;
	DelayMs(100);
	BUZZER = ON;
	DelayMs(100);
	BUZZER = OFF;
	DelayMs(100);
	BUZZER = ON;
	DelayMs(100);
	BUZZER = OFF;
	DelayMs(100);
	BUZZER = ON;
	DelayMs(100);
	BUZZER = OFF;
	DelayMs(100);
	//Reset();
	MARCHE = 0;
}
/**************************************************************
		Lecture de la memoire
**************************************************************/
void LectureMemoire(void)
{

MenuSauvegarde = LectureCharEeprom(ADD_MENU_SAUVEGARDE);
if(MenuSauvegarde > 10) MenuSauvegarde = MENU_CONTINU;
Etat.Aiguille = LectureCharEeprom(ADD_AIGUILLE);
Etat.Seringue = LectureCharEeprom(ADD_SERINGUE);
Etat.Profondeur = LectureCharEeprom(ADD_PROFONDEUR);
Etat.DebitInjection = LectureCharEeprom(ADD_DEBIT_INJECTION);
Etat.TempsEntreDose = LectureCharEeprom(ADD_TEMPS_ENTRE_DOSE);
Etat.Frequence = LectureCharEeprom(ADD_FREQUENCE);
Etat.DureeInjection = LectureCharEeprom(ADD_DUREE_INJECTION);
}
/**************************************************************

***************************************************************/
void SauvegardeMemoire(void)
{
EcritureCharEeprom(ADD_MENU_SAUVEGARDE,MenuSauvegarde);
EcritureCharEeprom(ADD_AIGUILLE,Etat.Aiguille);
EcritureCharEeprom(ADD_SERINGUE,Etat.Seringue);
EcritureCharEeprom(ADD_PROFONDEUR,Etat.Profondeur);
EcritureCharEeprom(ADD_DEBIT_INJECTION,Etat.DebitInjection);
EcritureCharEeprom(ADD_TEMPS_ENTRE_DOSE,Etat.TempsEntreDose);
EcritureCharEeprom(ADD_FREQUENCE,Etat.Frequence);
EcritureCharEeprom(ADD_DUREE_INJECTION,Etat.DureeInjection);
}
/**************************************************************
		retour seringue si bloquage
***************************************************************/
void RetourSeringue(void)
{

	while (GACHETTE == 1) 
		{
		ClrWdt();
		MoteurSeringueReverse(0x00);
		}

	if(FlagPlus == 1) FlagCorrection = 1;

	MoteurSeringueRL();
	MoteurChariotRL();

}
/***************************************************************

****************************************************************/
void GestionInit(void)
{
int error;


	FlagInit = OFF;
//	NbPulseChariot = 0;
FlagHalt=1;
	while ((FDC_CHARIOT == 1)&&(CourantChariot < SEUIL_SURCOURANT_CHARIOT)) MoteurChariotReverse(0x00);
	//while ((FDC_CHARIOT == 1)) MoteurChariotReverse(0x00);

	MoteurChariotBrake(0x00);
	DelayMs(100);
	CompteurPulseCh = 0;
	NbPulseChariot = 0;
	//if(ZeroAiguille =! 0)
	{	
		while (NbPulseChariot < ZeroAiguille) 
	       	{
			SensChariot = FOWARD;
	       	error = (ZeroAiguille - NbPulseChariot);
	       	error *= 10;
	       	error += 100;   // mini
	       	if(error > 250) error = 250;
	       	IN4 = 0;
	       	IN3 = 1;             // Forward
			ENB = 1;
	       	Delay10TCYx(error);
	       	IN3 = 0;
	       	DelayMs(1);
	       	}
	
		while (NbPulseChariot > ZeroAiguille) 
	       	{
			SensChariot = REVERSE;
	       	error = ( NbPulseChariot - ZeroAiguille);
	       	error *= 10;
	       	error += 100;   // mini
	       	if(error > 250) error = 250;
	       	IN4 = 1;
	       	IN3 = 0;             // reverse
			ENB = 1;
	       	Delay10TCYx(error);
	       	IN4 = 0;
	       	DelayMs(1);
	       	}
	}	

//	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
//	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);

//	MoteurChariotReverse(0);
//	DelayMs(6);
//	MoteurChariotBrake(0);
 
FlagHalt=0;
	Bip = NB_BIP;
	DelayMs(100);
	Bip = NB_BIP;
	DelayMs(100);
	NbChFW = NbPulseChariot;
	MoteurSeringueRL();
	MoteurChariotRL();

	CompteurCoups = 0;
	
	
}
/***********************************************************
Longueur vis 15mm --> 656 impulsions --> 44 impulsions/mm

************************************************************/
void InitPositionChariot(void)
{
int error;


FlagHalt=1;
	while ((FDC_CHARIOT == 1)&&(CourantChariot < SEUIL_SURCOURANT_CHARIOT)) MoteurChariotReverse(0x00);


	MoteurChariotBrake(0x00);
	DelayMs(100);
	CompteurPulseCh = 0;
	NbPulseChariot = 0;

	while (NbPulseChariot <= ZeroAiguille) 
       	{
		SensChariot = FOWARD;
       	error = (ZeroAiguille - NbPulseChariot);
       	error *= 10;
       	error += 100;   // mini
       	if(error > 250) error = 250;
       	IN4 = 0;
       	IN3 = 1;             // Forward
		ENB = 1;
       	Delay10TCYx(error);
       	IN3 = 0;
       	DelayMs(1);
       	}

	while (NbPulseChariot >= ZeroAiguille) 
       	{
		SensChariot = REVERSE;
       	error = ( NbPulseChariot - ZeroAiguille);
       	error *= 10;
       	error += 100;   // mini
       	if(error > 250) error = 250;
       	IN4 = 1;
       	IN3 = 0;             // reverse
		ENB = 1;
       	Delay10TCYx(error);
       	IN4 = 0;
       	DelayMs(1);
       	}

/*	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
	while (NbPulseChariot <= ZeroAiguille) MoteurChariotFoward(0);
	MoteurChariotReverse(0);
	DelayMs(6);
	MoteurChariotBrake(0);*/

FlagHalt=0;	
	DelayMs(100);
	MoteurSeringueRL();
	MoteurChariotRL();
	NbChFW = NbPulseChariot;
	
}



/*****************************************************************************************
		Recherche/ajustement de la position autour du zéro de l'aiguille
******************************************************************************************/
void AjustePositionChariot(void)
{
int error;

	while (CompteurPulseCh >= ZeroAiguille) 
       	{
		SensChariot = REVERSE;
       	error = ( CompteurPulseCh - ZeroAiguille);
       	error *= 2;
       	error += 50;   // mini
       	if(error > 100) error = 100;
       	IN4 = 1;
       	IN3 = 0;             // reverse
		ENB = 1;
       	Delay10TCYx(error);
       	IN4 = 0;
       	DelayMs(1);
       	}

while (CompteurPulseCh <= ZeroAiguille)
       	{
		SensChariot = FOWARD;
       	error = (ZeroAiguille - CompteurPulseCh);
       	error *= 2;
       	error += 50;   // mini
       	if(error > 100) error = 100;
       	IN4 = 0;
       	IN3 = 1;             // Forward
		ENB = 1;
       	Delay10TCYx(error);
       	IN3 = 0;
       	DelayMs(1);
       	}
}
/*****************************************************************************
				Affichage pour debug
*****************************************************************************/
void AfficheI2C(void)
{

	Compteur++;
	switch(Compteur)
		{
		case 1:
				SetDdramAdresse(LIGNE1+11);
				ConvIntDecimal(ZeroAiguille);
				//AfficheCaractere(Tampon[0]);
				AfficheCaractere(Tampon[1]);
				AfficheCaractere(Tampon[2]);
				AfficheCaractere(Tampon[3]);
				AfficheCaractere(Tampon[4]);
				AfficheCaractere(' ');
				break;
		case 2:
				SetDdramAdresse(LIGNE2+11);
				ConvIntDecimal(CompteurPulseCh);
				//AfficheCaractere(Tampon[0]);
				AfficheCaractere(Tampon[1]);
				AfficheCaractere(Tampon[2]);
				AfficheCaractere(Tampon[3]);
				AfficheCaractere(Tampon[4]);
				AfficheCaractere(' ');
				break;
		case 3:
				SetDdramAdresse(LIGNE3+11);
				ConvIntDecimal(MaxAiguille-PositionZAiguille);
				//AfficheCaractere(Tampon[0]);
				AfficheCaractere(Tampon[1]);
				AfficheCaractere(Tampon[2]);
				AfficheCaractere(Tampon[3]);
				AfficheCaractere(Tampon[4]);
				AfficheCaractere(' ');
				break;
		case 4:
				SetDdramAdresse(LIGNE4+11);
				//ConvIntDecimal(TensionBatterie);
				ConvIntDecimal(PositionZAiguille);
				//AfficheCaractere(Tampon[0]);
				AfficheCaractere(Tampon[1]);
				AfficheCaractere(Tampon[2]);
				AfficheCaractere(Tampon[3]);
				AfficheCaractere(Tampon[4]);
				AfficheCaractere(' ');
				break;

		default:Compteur=0;break;
		}
}
void AfficheTexte(void)
{

/*	AfficheMessage("SERINGU FW:",11,LIGNE1);
	AfficheMessage("SERINGU FR:",11,LIGNE2);
	AfficheMessage("SERINGU FW:",11,LIGNE3);
	AfficheMessage("TENSION BA:",11,LIGNE4);*/

	AfficheMessage("ZERO AIGU :",11,LIGNE1);
	AfficheMessage("POS CHARIO:",11,LIGNE2);
	AfficheMessage("PROFONDEUR:",11,LIGNE3);
	AfficheMessage("POS RETOUR:",11,LIGNE4);
}
/*******************************************************/
