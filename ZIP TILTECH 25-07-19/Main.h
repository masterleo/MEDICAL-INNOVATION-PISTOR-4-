/****************************************************************************
		 	 PROGRAMME: main.h

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME :header file Module programme principal   
 
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/
//adresse Memoire pour sauvegarde
#define ADD_MENU_SAUVEGARDE		50
#define ADD_AIGUILLE			51
#define ADD_SERINGUE			52
#define ADD_PROFONDEUR			53
#define ADD_DEBIT_INJECTION		54
#define ADD_TEMPS_ENTRE_DOSE	55
#define ADD_FREQUENCE			56
#define ADD_DUREE_INJECTION		57

struct bit_def 	{
        char    b0:1;
        char    b1:1;
        char    b2:1;
        char    b3:1;
        char    b4:1;
        char    b5:1;
        char    b6:1;
        char    b7:1;
				};
union byte_def{
    unsigned char c;
    struct bit_def  b;
			  };

//definition des touches
#define TOUCHE_CONTINU				0x6F
#define TOUCHE_MESOPERFUSION		0x7D
#define TOUCHE_STANDARD				0x7B
#define TOUCHE_RAFALE				0x77
#define TOUCHE_SERINGUE				0xDB
#define TOUCHE_AIGUILLE				0xD7
#define TOUCHE_PROFONDEUR			0xDD
#define TOUCHE_DUREEINJECTION		0xBD
#define TOUCHE_DEBITINJECTION		0xAF
#define TOUCHE_TEMPSENTREDOSE		0xBB
#define TOUCHE_FREQUENCEINJECTION	0xB7


//definition des Leds
extern union byte_def LedU1A;
#define Led_Frequence_300cps_mn			LedU1A.b.b0//D9
#define Led_TempsEntreDose_32s			LedU1A.b.b1//D14
#define Led_TempsEntreDose_40s		    LedU1A.b.b2//D15
#define Led_TempsEntreDose_48s		    LedU1A.b.b3//D16
#define Led_TempsEntreDose_24s		    LedU1A.b.b4//D13
#define Led_TempsEntreDose_16s	    	LedU1A.b.b5//D12
#define Led_TempsEntreDose_8s			LedU1A.b.b6//D11
#define Led_TempsEntreDose_0s	        LedU1A.b.b7//D10
	
extern union byte_def LedU1B;
#define Led_Touche_Fonction_Rafale		LedU1B.b.b0//D1
#define Led_Touche_Dose_Standard        LedU1B.b.b1//D2
#define Led_Touche_Mesoperfusion		LedU1B.b.b2//D3
#define Led_Touche_Fonction_Continu	    LedU1B.b.b3//D4
#define Led_Frequence_100cps_mn		    LedU1B.b.b4//D5
#define Led_Frequence_150cps_mn			LedU1B.b.b5//D6
#define Led_Frequence_200cps_mn		    LedU1B.b.b6//D7
#define Led_Frequence_250cps_mn		    LedU1B.b.b7//D8

extern union byte_def LedU2A;
#define Led_Batterie_25pourcent			LedU2A.b.b0//D32
#define Led_Batterie_50pourcent	        LedU2A.b.b1//D31
#define Led_Batterie_75pourcent			LedU2A.b.b2//D30
#define Led_Batterie_100pourcent    	LedU2A.b.b3//D29
#define Led_Debit_10ml_mn			    LedU2A.b.b4//D28
#define Led_Debit_9ml_mn				LedU2A.b.b5//D27
#define Led_Debit_8ml_mn			    LedU2A.b.b6//D26
#define Led_Debit_7ml_mn			    LedU2A.b.b7//D25

extern union byte_def LedU2B;
#define Led_Duree_Injection_0s			LedU2B.b.b0//D17
#define Led_Duree_Injection_1s       	LedU2B.b.b1//D18
#define Led_Duree_Injection_2s			LedU2B.b.b2//D19
#define Led_Duree_Injection_3s		    LedU2B.b.b3//D20
#define Led_Debit_3ml_mn		    	LedU2B.b.b4//D21
#define Led_Debit_4ml_mn				LedU2B.b.b5//D22
#define Led_Debit_5ml_mn		    	LedU2B.b.b6//D23
#define Led_Debit_6ml_mn		  	  	LedU2B.b.b7//D24

extern union byte_def LedU3A;
#define Led_Profondeur_0mm				LedU3A.b.b0//D41
#define Led_Profondeur_1mm	        	LedU3A.b.b1//D42
#define Led_Profondeur_2mm				LedU3A.b.b2//D43
#define Led_Profondeur_3mm    			LedU3A.b.b3//D44
#define Led_Profondeur_4mm			    LedU3A.b.b4//D45
#define Led_Profondeur_5mm				LedU3A.b.b5//D46
#define Led_Profondeur_6mm			    LedU3A.b.b6//D47
#define Led_Profondeur_7mm			    LedU3A.b.b7//D48

extern union byte_def LedU3B;
#define Led_Aiguille_4mm				LedU3B.b.b0//D33
#define Led_Aiguille_6mm       			LedU3B.b.b1//D34
#define Led_Aiguille_13mm				LedU3B.b.b2//D35
#define Led_Seringue_1ml		    	LedU3B.b.b3//D36
#define Led_Seringue_2ml		    	LedU3B.b.b4//D37
#define Led_Seringue_3ml				LedU3B.b.b5//D38
#define Led_Seringue_5ml		    	LedU3B.b.b6//D39
#define Led_Seringue_10ml		  	  	LedU3B.b.b7//D40

extern union byte_def LedU4B;
#define Led_Profondeur_8mm				LedU4B.b.b0//D49
#define Led_Profondeur_9mm       		LedU4B.b.b1//D50
#define Led_Touche_FreqInjection		LedU4B.b.b2//D51
#define Led_Touche_TempsEntreDose  		LedU4B.b.b3//D52
#define Led_Touche_DureeInjection  		LedU4B.b.b4//D53
#define Led_Touche_DebitInjection		LedU4B.b.b5//D54
#define Led_Touche_Aiguille	    		LedU4B.b.b6//D55
#define Led_Touche_Seringue  	  		LedU4B.b.b7//D56



extern unsigned char Led_Touche_Profondeur;

