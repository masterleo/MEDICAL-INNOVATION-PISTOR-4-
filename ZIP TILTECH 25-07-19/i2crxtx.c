/****************************************************************************
		 	 PROGRAMME: I2Crxtx.C

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME :Fonction de base I2C  
 
  ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/
#include "p18f46K22.h"
#include "i2crxtx.h"
#include "DELAY.h"
#include "Main.h"


void I2c_init(void);
void I2c_waitForIdle(void);
void I2c_start(void);
unsigned char I2c_write( unsigned char I2cWriteData );
int I2c_read( unsigned char Ack );
void I2c_stop(void);
void I2c_repStart(void);
/*****************************************************************
		Init Bus I2C
*****************************************************************/
void I2c_init(void)
{

  TRISCbits.TRISC3=1;		    // set SCL pin as input
  TRISCbits.TRISC4=1;		    // set SDA pin as input

  SSPCON1 = 0x28;	    // set I2C master mode
  SSPCON2 = 0x00;

////clock = (Fosc/(4*(SSPADD+1))pour QUARTZ=20Mhz
// SSPADD = 9;		    // 100k at 4Mhz clock
// SSPADD = 1;		    // 400k at 4Mhz clock
// SSPADD = 19;			// 100k at 8Mhz clock
 //SSPADD = 9;			//400K at 16Mhz clock
 //SSPADD = 3;		    // 400k at 8Mhz clock
// SSPADD = 2;		    // 431k at 6Mhz clock
// SSPADD = 10; 	    // 400k at 20Mhz clock
// SSPADD = 9;		  // 500k at 20Mhz clock
// SSPADD = 24;		  // 200k at 20Mhz clock
 SSPADD = 49;		  // 100k at 20Mhz clock
// SSPADD = 99;		  // 100k at 40Mhz clock
//SSPADD = 5; 	    // 400k at 10Mhz clock
 //SSPADD = 24;		  // 400k at 40Mhz clock
 SSPSTATbits.CKE=1;		    // Data transmitted on falling edge of SCK
 SSPSTATbits.SMP=1;		    // disable slew rate control

 PIR1bits.SSPIF=0;		    // clear SSPIF interrupt flag
 PIR2bits.BCLIF=0;		    // clear bus collision flag
}
/******************************************************************************************
		Attente bus libre
*******************************************************************************************/
void I2c_waitForIdle(void)
{
 while ( SSPSTATbits.R_W | ((SSPCON2 & 0x1F )!=0) ) {}; // wait for idle and not writing
}

/******************************************************************************************
		Initialise un start
*******************************************************************************************/
void I2c_start(void)
{
 I2c_waitForIdle();	    //Wait for the idle condition
 SSPCON2bits.SEN =1; 		    //Initiate START conditon.
}

/******************************************************************************************
		Repete un start
*******************************************************************************************/
void I2c_repStart(void)
{
unsigned char i;
 I2c_waitForIdle();	    //Wait for the idle condition
 Delay10Us();
 //for (i=0;i<100;i++);
 SSPCON2bits.RSEN=1;		    //initiate Repeated START condition

}
/******************************************************************************************
	Ecrit un char sur bus I2C et retourne 1 si OK
*******************************************************************************************/
unsigned char I2c_write( unsigned char I2cWriteData )
{
unsigned char i;
 I2c_waitForIdle();	    //Wait for the idle condition
 Delay10Us();
 //for (i=0;i<100;i++);
 SSPBUF = I2cWriteData;     //Load SSPBUF with i2cWriteData (the value to be transmitted)
	
 return ( ! SSPCON2bits.ACKSTAT  );     // function returns '1' if transmission is acknowledged
}
/******************************************************************************************
		Lit un char sur bus I2C
******************************************************************************************/
int I2c_read( unsigned char Ack )
{
 unsigned char I2cReadData,i;

 I2c_waitForIdle();	    //Wait for the idle condition
 Delay10Us();
//for (i=0;i<100;i++);
 SSPCON2bits.RCEN=1;		    //Enable receive mode

 I2c_waitForIdle();	    //Wait for the idle condition

 I2cReadData = SSPBUF;	    //Read SSPBUF and put it in i2cReadData

 I2c_waitForIdle();	    //Wait for the idle condition

 if ( Ack )		    //if ack=1 (from i2c_read(ack))
  {
  SSPCON2bits.ACKDT=0;		    //then transmit an Acknowledge
  }
 else
  {
  SSPCON2bits.ACKDT=1;		    //otherwise transmit a Not Acknowledge
  }
 //I2c_waitForIdle();	    //Wait for the idle condition
 SSPCON2bits.ACKEN=1;		    // send acknowledge sequence
 return( I2cReadData );     //return the value read from SSPBUF
}
/******************************************************************************************
	Genere un stop condition
**************************************************************************************/
void I2c_stop(void)
{
unsigned char i;
 I2c_waitForIdle();	    //Wait for the idle condition
 Delay10Us();
//for (i=0;i<100;i++);
 SSPCON2bits.PEN=1; 		    //Initiate STOP condition
}
/******************************************************************************************/

