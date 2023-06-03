#include <p33FJ16GS502.h>
#include "uart.h"
#include "dsp.h"

extern tPID	Buck2VoltagePID;
int cont_RX_bits=0,cont_TX_bits=0,cont_abcCoef=0,outputVoltage, referenciaVoltageL;
int parametroL;
char outputVoltageH,OnOffFuente=0;
extern unsigned int Buck2ReferenceNew;
/*
#define PID_BUCK2_A2 -344 //-2^4+...+=-0.083984375_>binario 1xxx.xxx...xxx
#define PID_BUCK2_A1 4440  //binario=1000101011000 &Q3.12=1.000101011000=1.083984375
//coeficientes del numerador de la FDT, parte entera+decimal enn Q3.12
#define PID_BUCK2_B0 7778 //En Q3.12=1.89892578125
#define PID_BUCK2_B1 433
#define PID_BUCK2_B2 -7344
*/

enum  {		NO_ORDEN		= 0X00,
			ON_BUCK2		= 0XA0, 	
			OFF_BUCK2		= 0XA1,
			REF_BUCK2   	= 0XA2,
			TENSION_SALIDA_BUCK2	= 0XA3
            
}ordenUSB;
 

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) 
{


//int outputVoltage;
char outputVoltageL,limpiarRevReg;

if (ordenUSB==NO_ORDEN)
{
      while (DataRdyUART1()!=1);
ordenUSB = U1RXREG; //identifica el tipo de orden. Se mantiene hasta el final de la gestion
cont_RX_bits=0;
cont_TX_bits=0;
}
switch (ordenUSB)
{
case ON_BUCK2:
   
    if(cont_RX_bits==0)
    {
        cont_abcCoef=0;
        cont_RX_bits=1;
        while(BusyUART1());	
        U1TXREG=ON_BUCK2; //le pide al s 18f2450 el byte L del 1er parámetro
    }
    
    else
    {
        if( cont_RX_bits<11)
        {
        
            if((cont_RX_bits==1)||(cont_RX_bits==3)||(cont_RX_bits==5)||(cont_RX_bits==7)||(cont_RX_bits==9))
            {
                while (DataRdyUART1()!=1);
                parametroL=U1RXREG;
               
            }
            else
            {
            while (DataRdyUART1()!=1);
                Buck2VoltagePID.abcCoefficients[cont_abcCoef]=U1RXREG;
                Buck2VoltagePID.abcCoefficients[cont_abcCoef]=Buck2VoltagePID.abcCoefficients[cont_abcCoef]<<8;
                Buck2VoltagePID.abcCoefficients[cont_abcCoef]=Buck2VoltagePID.abcCoefficients[cont_abcCoef]|parametroL;
                cont_abcCoef++;
            }
            cont_RX_bits++;
            while(BusyUART1());	
            U1TXREG=ON_BUCK2;//orden para que el 18F mande el byte L de la referencia
// se aclara que podrá mandar cualquier dato, porque el tipo de orden se gestiona
// en el lado del 18f. Solo interesa que la EUSART provoque interrupción
        }
        else
            {
            while (DataRdyUART1()!=1);//este while SOLO siempre antes de leer el registro. Si no , fallo.
            limpiarRevReg=U1RXREG;
             //if (U1RXREG==1)  /* PTPER = ((1 / 300kHz) / 1.04ns) = 3155, where 300kHz */
						PTPER=3155;/* is the desired switching frequency and 1.04ns is PWM resolution. */            
             //else /*(U1RXREG==2) si hubiera mas fuentes para hacer switch-case*/
                        //PTPER=9614; // PTPER = ((1 / 100kHz) / 1.04ns) = 9614                
                        
          /*                                                                                 
            Buck2VoltagePID.abcCoefficients[0] = PID_BUCK2_B0;
            Buck2VoltagePID.abcCoefficients[1] = PID_BUCK2_B1;
            Buck2VoltagePID.abcCoefficients[2] = PID_BUCK2_B2;
            Buck2VoltagePID.abcCoefficients[3] = PID_BUCK2_A1;
            Buck2VoltagePID.abcCoefficients[4] = PID_BUCK2_A2;
           */
          
            
            
                ordenUSB=NO_ORDEN; //cuando se acaban de recibir todos los parámetros
                cont_RX_bits=0;
                cont_abcCoef=0;
                
                PTCONbits.PTEN = 1;/* Enable the PWM */
            }
    }

break;

case OFF_BUCK2:
    //limpiarRevReg=U1RXREG;
PTCONbits.PTEN = 0;	/* Disaable the PWM */
ordenUSB=NO_ORDEN;
OnOffFuente=0;						
break;

case REF_BUCK2:

if (cont_RX_bits==0)
{ //..primer byte recibido
cont_RX_bits=1;
  while(BusyUART1());	
U1TXREG=REF_BUCK2; //orden para que el 18F mande el byte L de la referencia
// se aclara que podrá mandar cualquier dato, porque el tipo de orden se gestiona
// en el lado del 18f. Solo interesa que la EUSART provoque interrupción 
}

if(cont_RX_bits==1)
{
    cont_RX_bits=2;
    while (DataRdyUART1()!=1);
referenciaVoltageL=U1RXREG;
while(BusyUART1());	
U1TXREG=REF_BUCK2;//orden para que el 18F mande el byte H de la referencia
}

if(cont_RX_bits==2)
{
        while (DataRdyUART1()!=1);
Buck2ReferenceNew=U1RXREG;
Buck2ReferenceNew=Buck2ReferenceNew<<8;
Buck2ReferenceNew=(Buck2ReferenceNew|referenciaVoltageL);
cont_RX_bits=0;
ordenUSB=NO_ORDEN;
}


break;

case TENSION_SALIDA_BUCK2:
 
//en vez de volver a hacer la medida se aprovecha que esta en Buck2VoltagePID.measuredOutput
//primer acceso 
if (cont_TX_bits==0)
{
cont_TX_bits=1;
if (PTCONbits.PTEN == 1)
{
    
outputVoltageL=(char)(Buck2VoltagePID.measuredOutput>>2);
outputVoltageH=(char)(Buck2VoltagePID.measuredOutput>>10);
}
else

{
outputVoltageL=0x0;
outputVoltageH=0x0;


}

//manda el byte ALTO y en la siguiente int el byte BAJO 
while(BusyUART1());						
U1TXREG=outputVoltageH;
}
/*
if (PTCONbits.PTEN == 1){
while (ADSTATbits.P1RDY ==0); 	//Mientras no hay dato de la tension de salida espera (se supone que poco tiempo)			
outputVoltage = ADCBUF3;						// Read input Voltage 16 bits
outputVoltageL=(char)outputVoltage; 			//2 de 8 bits para enviar al 18F2450
outputVoltageH=(char)(outputVoltage>>8);
}
}
*/
else //segundo acceso
{
   while (DataRdyUART1()!=1);
limpiarRevReg=U1RXREG; 
while(BusyUART1());
U1TXREG=outputVoltageL;
cont_TX_bits=0;
ordenUSB=NO_ORDEN;
}
//ADSTATbits.P2RDY = 0;						// Clear the ADC pair ready bit	
break;

/*
default:
{
    if (OnOffFuente==0)
    {
    PTCONbits.PTEN = 1;// Enable the PWM 
OnOffFuente=1;
    }
    else
        {
    PTCONbits.PTEN = 0;// Enable the PWM 
OnOffFuente=0;
    }
    ordenUSB=NO_ORDEN;
}
*/
}	


	IFS0bits.U1RXIF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
	IFS0bits.U1TXIF = 0;

}	
void InitUART1() {
	// This is an EXAMPLE, so brutal typing goes into explaining all bit sets

	


	U1MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//U2MODEbits.notimplemented;	// Bit14
	U1MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U1MODEbits.IREN = 0;	// Bit12 No IR translation
	U1MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	U1MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U1MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U1MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U1MODEbits.URXINV=0; 		//Bit5 idle state is 1
	U1MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
	U1MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U1MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	// Load a value into Baud Rate Generator.  Example is for 9600.
	// See section 19.3.1 of datasheet.
	//  U1BRG = (Fcy/(16*BaudRate))-1
	//  U1BRG = (40M/(16*9600))-1= 259
	 U1BRG = 259;
//el usart del 18f2450 esta trabajando a 9600 baudios
//con U1BRG = 216; comprobado que lee bien el dato de 8 bits. En el 2023 no ¿?
	//U1BRG = 216;	// 40MHz osc, 9600 Baud

	// Load all values in for U1STA SFR
	U1STAbits.UTXISEL1 = 1;	//Bit15 Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 1;	//Bit14 U1TX Nivel alto en estado de reposos
	U1STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
	U1STAbits.UTXBRK = 0;	//Bit11 Disabled
	U1STAbits.UTXEN = 0;	//Bit10 TX pins  controlled by periph
	U1STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	U1STAbits.TRMT = 0;		//Bit8 *Read Only bit*
	U1STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U1STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	U1STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	U1STAbits.PERR = 0;		//Bit3 *Read Only Bit*
	U1STAbits.FERR = 0;		//Bit2 *Read Only Bit*
	U1STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	U1STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

	IPC7 = 0x4400;	// Mid Range Interrupt Priority level, no urgent reason

	IFS0bits.U1TXIF = 0;	// Clear the Transmit Interrupt Flag
	IEC0bits.U1TXIE = 0;	// Disable Transmit Interrupts
	IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC0bits.U1RXIE = 1;	// Enable Recieve Interrupts

//	RPOR9bits.RP101R = 3;		//RF5 as U2TX
//Pin 18 LINKED WITH UART1 RECEIVE

//PPSOutput(PPS_U1TX,PPS_RP6); /*U1TX on RP5*/
//TRISBbits.TRISB6=0;// pin 7 salida digital
//__builtin_write_OSCCONL(OSCCON & ~(1<<6));	//unLOCK peripheral Pin Select ds70190E PAGE 30.12	
//__builtin_write_OSCCONL(OSCCON & 0XDF);
//__builtin_write_OSCCONL(OSCCON & 0X46);
//__builtin_write_OSCCONL(OSCCON|0XDF);
RPINR18bits.U1RXR=7;
RPOR3bits.RP6R=3;// RB6 (pin 17)  U1TX

//__builtin_write_OSCCONL(OSCCON|0X40);  //LOCK peripheral Pin Select


U1MODEbits.UARTEN = 1;	// And turn the  on
U1STAbits.UTXEN = 1;  //estas 2 instrucciones en este orden, si no, no se habilita el puerto.


}


