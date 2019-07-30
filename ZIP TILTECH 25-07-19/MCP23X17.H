/****************************************************************************
		 	 PROGRAMME: MCP23X17.H

 CREE PAR : 	FRAZAO		
 Modifie par :					

 DATE DE CREATION : 15.02.2016			
 DATE DE MODIFICATION :				

 PROJET :  	PISTOR4
 VERSION: 	V2

 OBJET DU PROGRAMME : Header file pour MCP23X17 (carte affichage)  
 
 ENVIRONEMENT:MPLAB IDE v8.76
 COMPILATEUR :MPLAB C18 V3.46 
***************************************************************************/
#define IODIRA   	0x00//IO DIRECTION REGISTER 	--> 1=input, 0=output
#define IODIRB   	0x01//
#define IPOLA    	0x02//INPUT POLARITY REGISTER	--> 1=GPIO REFLET OPOSITE LOGIC, 0= SAME LOGIC
#define IPOLB    	0x03
#define GPINTENA 	0x04//INT ON CHANGE PINS		--> 1=ENABLE, 0=DISABLE
#define GPINTENB 	0x05
#define DEFVALA  	0x06//DEFAULT VALUE REG for Interrupt on change
#define DEFVALB  	0x07
#define INTCONA  	0x08//INTERRUPT ON CHANGE CONTROL REG
#define INTCONB  	0x09
#define IOCONA   	0x0A//I/O EXPANDER CONFIGURATION REG (BANK,MIRR
#define IOCONB   	0x0B
#define GPPUA    	0x0C//PULL UP RESISTOR CONF REG
#define GPPUB    	0x0D
#define INTFA    	0x0E//INTERRUPT FLAG REG (on a port PIN)
#define INTFB    	0x0F
#define INTCAPA  	0x010//INTERRUPT CAPTURE REG	--> 1=Logic high, 0= Logic LOW
#define INTCAPB  	0x011
#define GPIOA    	0x012//PORT REGISTER
#define GPIOB    	0x013
#define OLATA    	0x014//OUTPUT LATCH REGISTER
#define OLATB    	0x015
