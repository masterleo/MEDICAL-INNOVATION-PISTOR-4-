/****************************************************************************
		 	 PROGRAMME: I2Crxtx.H

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME :header file du Module I2C de base
 
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/

extern void I2c_init(void);

extern void I2c_waitForIdle(void);

extern void I2c_start(void);

extern void I2c_repStart(void);

extern void I2c_stop(void);

extern int I2c_read( unsigned char Ack );

extern unsigned char I2c_write( unsigned char I2cWriteData );
extern unsigned char FlagErrI2C ;

/******************************************************************************************/


