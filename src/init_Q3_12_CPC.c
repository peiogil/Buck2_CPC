/**********************************************************************
* © 2008 Microchip Technology Inc.
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
*******************************************************************************/

#include "p33FJ16GS502.h"
#include "dsp.h"
#include "Functions.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~  PID Variable Definitions  ~~~~~~~~~~~~~~~~~~~~~~~ */

/* Variable Declaration required for each PID controller in the application. */
//void PIDInitBuck2(fractional *);
tPID Buck2VoltagePID;





/* These data structures contain a pointer to derived coefficients in X-space and 
   pointer to controler state (history) samples in Y-space. So declare variables 
   for the derived coefficients and the controller history samples */

/*1) C attributes, designated by the __attribute__ keyword, provide a *
* means to specify various characteristics of a variable or *
* function, such as where a particular variable should be placed *
* in memory, whether the variable should be aligned to a certain *
* address boundary, whether a function is an Interrupt Service *
* Routine (ISR), etc. If no special characteristics need to be *
* specified for a variable or function, then attributes are not *
* required. For more information about attributes, refer to the *
* C30 User's Guide. *
* *
* 2) The __section__(".xbss") and __section__(".ybss") attributes are *
* used to place a variable in X data space and Y data space, *
* respectively. Variables accessed by dual-source DSP instructions *
* must be defined using these attributes.
* 3) The aligned(k) attribute, used in variable definitions, is used *
* to align a variable to the nearest higher 'k'-byte address *
* boundary. 'k' must be substituted with a suitable constant *
* number when the ModBuf_X(k) or ModBuf_Y(k) macro is invoked. *
* In most cases, variables are aligned either to avoid potential *
* misaligned memory accesses, or to configure a modulo buffer. *
* *
* 4) The __interrupt__ attribute is used to qualify a function as an *
* interrupt service routine. An interrupt routine can be further *
* configured to save certain variables on the stack, using the *
* __save__(var-list) directive. *
* *
* 5) The __shadow__ attribute is used to set up any function to *
* perform a fast context save using shadow registers. *
* *
* 6) Note the use of double-underscores (__) at the start and end of *
* all the keywords mentioned above. *
* *
**********************************************************************/



/*reserva de memoria en zona x, donde se ubicaran los coeficientes de la fdt*/
fractional Buck2VoltageABC[5] __attribute__ ((section (".xbss, bss, xmemory")));
/*reserva de memoria en zona y, donde se ubicaran error y d actual y anteriores*/
fractional Buck2VoltageHistory[5] __attribute__ ((section (".ybss, bss, ymemory")));
/*el tamaño depende del tipo de regulador a implementar, en este caso tipo3*/



/*coeficientes del denominador de la FDT,
 en formato Q3.12 expresada en decimal 
 (parte entera+fracción) con signo cambiado*/
/*
#define PID_BUCK2_A2 -3948  //En Q3.12 -2^3+...+=-0.9638_>binario 11111100010010000
                            //este binario como 2'compl a decimal -3948
#define PID_BUCK2_A1 8045  //1.465 
//coeficientes del numerador de la FDT, parte entera+decimal enn Q3.12
#define PID_BUCK2_B0 117 //En Q3.12=2.643
#define PID_BUCK2_B1 0 //En Q3.12=0.04485
#define PID_BUCK2_B2 -117
*/
/*
// fc=1000
 3.34 z^2 + 0.03076 z - 3.309
  ----------------------------
     z^2 - 1.955 z + 0.9546
*/
#define PID_BUCK2_A2 -3948  //En Q3.12 -2^3+...+=-0.9638_>binario 11111100010010000
                            //este binario como 2'compl a decimal -1904
#define PID_BUCK2_A1 8045  //1.465 
//coeficientes del numerador de la FDT, parte entera+decimal enn Q3.12
#define PID_BUCK2_B0 13681 //En Q3.12=2.643
#define PID_BUCK2_B1 126 //En Q3.12=0.04485
#define PID_BUCK2_B2 -13554

/*
 * 0.3068 z^2 + 0.006992 z - 0.2998
  --------------------------------
       z^2 - 1.928 z + 0.9281
 *  fc=100Hz
 *  0.02857 z^2 + 4.065e-05 z - 0.02853
  -----------------------------------
         z^2 - 1.997 z + 0.997
 * #define PID_BUCK2_B0 117 //En Q3.12=2.643
#define PID_BUCK2_B1 0 //En Q3.12=0.04485
#define PID_BUCK2_B2 -117
*/

#define CMP1C 0b100111                                  /*COMP1C asociado a pin 4*/
#define PID_BUCK2_VOLTAGE_REFERENCE 0xE88				/* Reference voltage is from resistor divider circuit R29 & R30
													    	Voltage FB2 = (5kOhm / (5kOhm + 3.3kOhm)) * 3.3V = 1.988V
														    Now calculate expected ADC value (1.988V * 1023)/3.3V = 617 
															Then left shift by 2 for Q3_12 format (617 * 4) = 2468 = 0x9A4
                                                            5V=>3A2*4=774 */
																	  
#define PID_BUCK2_VOLTAGE_REF_MIN 	  0x120			/* Minimum reference voltage is total dead time (72) left shifted by 2 bits*/

/* This is increment rate to give us desired PID_BUCK1VOLTAGE_REFERENCE. The sofstart takes 50ms */
 #define BUCK2_SOFTSTART_INCREMENT	 (PID_BUCK2_VOLTAGE_REFERENCE  / 50 )

/*referencia1=3.3v, del sensor (divisor de tension)*/
/*5/(5+3.3)*3.3=1.988v--->(1.988*1023)/3.3v=617*/
/*ADCres=10, Q15res=15--->617*32=19744 = 0x4D20*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern unsigned int TimerInterruptCount;

unsigned int Buck2ReferenceNew,Buck2ReferenceOld ;

void Buck2DriveCPC(void)
{
//comparator configuration para control pico de corriente
CMPCON1=0x8081; 
//CMPON ? CMPSIDL ? ? ? ? DACOE INSEL1 INSEL0 EXTREF ? CMPSTAT ? CMPPOL RANGE
//  1   0    0    0 0 0 0   0     1      0      0     0   0     0   0     1 
//INSEL1 INSEL0
// 1        0  LA ENTRADA ISENSE ES EL PIN 4=> CMP1C=comparador 1 C (pin4)
//En U-0 idle  hyst flter  X   X  CMP1
// PTCON: PWM Time Base Control Register PWM no habilitado
PTCON = 0;	
// PTCON2: PWM Clock Divider Select Register
PTCON2 = 0;	//Divide by 1, maximum PWM timing resolution
//Esta instrucción es la que permite que la comunicación serie, y con ella la USB
//no se bloqueara.
// su gemela con control en modo tensión Q3.12 no la necesita INTCON1bits.NSTDIS=0;
INTCON1bits.NSTDIS=1;  //deshabilitado anidamiento de interrupciones
    /* BUCK2 converter setup to output 3.3V */ 

    IOCON2bits.PENH = 1;               	/* PWM2H is controlled by PWM module */
    IOCON2bits.PENL = 1;                /* PWM2L is controlled by PWM module */
    IOCON2bits.PMOD = 0;                /* Complementary Output mode*/
    IOCON2bits.POLH = 0;                /* Drive signals are active-high */
    IOCON2bits.POLL = 0;                /* Drive signals are active-high */
    IOCON2bits.OVRENH = 0;				/* Disable Override feature for shutdown PWM */  
    IOCON2bits.OVRENL = 0;				/* Disable Override feature for shutdown PWM */
    IOCON2bits.OVRDAT = 0b00;			/* Shut down PWM with Over ride 0 on PWMH and PWML */	
    IOCON2bits.FLTDAT = 0b01;			/* As IFLTMOD=0 when fault is active 1H = low; 1L = high*/
    DTR2    = 0x60;                	  	/* DTR = (100ns / 1.04ns), where desired dead time is 25ns. 
									     Mask upper two bits since DTR<13:0> */
    ALTDTR2 = 0x60;            		  	/* ALTDTR = (50ns / 1.04ns), where desired dead time is 50ns. 
									     Mask upper two bits since ALTDTR<13:0> */        
    PWMCON2bits.FLTIEN=1;               /*Fault interrupt is enabled*/
    PWMCON2bits.ITB = 0;                /* PTPER based period Select Primary Timebase mode */
    PWMCON2bits.MDCS = 0;               /*duty cycle lo pone cada PDC*/	       
    PWMCON2bits.DTC = 0; 
    PWMCON2bits.IUE = 0;                /* Disable Immediate duty cycle updates */
    
    RPINR29bits.FLT1R = 33; 		//Assign PWM Fault Input 1 (FLT1) to the RP33 Pin 
    //RPOR16bits.RP33R = 0b100111; 	/* Analog Comparator Output 1 is Assigned to RP33 Output Pin*/
    RPOR16bits.RP33R =CMP1C;
    LEBCON2bits.PHR = 1; 			/* Rising edge of PWMxH will trigger LEB counter */
	LEBCON2bits.PHF = 1; 			/* Falling edge of PWMxH is ignored by LEB counter */
	LEBCON2bits.FLTLEBEN = 1; 		/* Enable fault LEB for selected source */
	LEBCON2bits.CLLEBEN = 0; 		/* Disable current-limit LEB for selected source */
	LEBCON2bits.LEB = 30; 			/* Blanking period of 8.32*30=249,6ns */
    
    FCLCON2bits.IFLTMOD=0;          /*Normal Fault mode*/
    FCLCON2bits.CLMOD = 0;	        /* Current-limit function is disabled */
    FCLCON2bits.FLTPOL = 0;			/* fault source is active high*/
    FCLCON2bits.FLTSRC = 0;	 		/*Fault 1 Selected for PWM Generator 2 Control Signal Source*/
    FCLCON2bits.FLTMOD = 1; 		/* Fault cycle by cycle */   
    TRGCON2bits.TRGDIV = 0;         /* Trigger interrupt generated every PWM cycle
    TRGCON2bits.TRGSTRT = 0;        /* Trigger generated inmediatly*/    
	PDC2 =0x950;                        /*0x6C0 Dmax=0,5    maximum duty cycle                               
   //PDC2 = ( PWM_PERIOD*0.9 );      /* Initial pulse-width = minimum deadtime required (DTR2 + ALDTR2)*/
    TRIG2 = 100;		             /* Trigger generated almost at beginning of PWM active period */  
    /*Inicializa el Timer1 para los delays*/
    PR1 = 0x9C40;						//(1ms / 25ns) = 40,000 = 0x9C40 
    IPC0bits.T1IP = 4;				 	//Set Interrupt Priority lower then ADC
    IEC0bits.T1IE = 1;					//Enable Timer1 interrupts 

    //TRISAbits.TRISA3=0; 	/*salida digital PIN 26 PWM1L*/ 
                            //AHORA PIN2H&PWM2L PERO TRIS NO HACE FALTA, vale con IOCON2bits.PENH&PENL 
	//TRISAbits.TRISA4=0 ;  /*salida digital PIN 25 PWM1H*/  AHORA PIN24 PWM2H                                
}




void CurrentandVoltageMeasurements(void)
{
    ADCONbits.FORM = 0;                   /* Integer data format */
    ADCONbits.EIE = 0;                    /* Early Interrupt disabled */
    ADCONbits.ORDER = 1;                  /* Convert odd channel first */
    ADCONbits.SEQSAMP = 0;                /* Select simultaneous sampling */
    ADCONbits.ADCS = 2;                   /* ADC clock = FADC/6 = 120MHz / 6 = 20MHz, 
											12*Tad = 1.6 MSPS, two SARs = 3.2 MSPS */

    IFS6bits.ADCP1IF = 0;		    	  /* Clear ADC interrupt flag */ 
    IPC27bits.ADCP1IP = 5;			      /* Set ADC interrupt priority */ 
	IEC6bits.ADCP1IE = 1;				  /* Enable the ADC Pair 1 interrupt*/
  
	ADPCFGbits.PCFG2 = 0; 			  	  /* PinConCFigAnalog Current Measurement for Buck 2 */ 
    ADPCFGbits.PCFG3 = 0; 				  /* PinConCFigAnalog Voltage Measurement for Buck 2 */

    ADPCFGbits.PCFG4 = 0; 				  /* PinConCFigAnalog Voltage Measurement for input voltage source */	
    

 
    ADSTATbits.P1RDY = 0; 				  /* Clear Pair 1, AN2&AN3, data ready bit */
    ADCPC0bits.IRQEN1 = 1;                /* Enable ADC Interrupt for Buck 2 control loop: current&voltage */
    ADCPC0bits.TRGSRC1 = 5; 			  /* ADC Pair 1 triggered by PWM Generator 2 ¿1? primary trigger */
/*lo que se entiende de las hojas es para PWM generator 2 ADCPC0bits.TRGSRC1 = 5 ¿?*/

    ADSTATbits.P2RDY = 0; 				  /* Clear Pair 2 data ready bit */
    ADCPC1bits.IRQEN2 = 0;                /* Enable ADC Interrupt for input voltage measurement */
    ADCPC1bits.TRGSRC2 = 5; 			 /* ADC Pair 2 triggered by PWM Generator 2 ¿1 con trgsrc1=4? primary trigger */

 	
}


void Buck2VoltageLoop()
{
    Buck2VoltagePID.abcCoefficients = Buck2VoltageABC;     /* Set up pointer to derived coefficients */
    Buck2VoltagePID.controlHistory = Buck2VoltageHistory;  /* Set up pointer to controller history samples */
    
      PIDInitBuck2(&Buck2VoltagePID);    /*pone todas las variables de error y anteriores de control a cero*/                        


/* se llama a funcion pidinit, se le pasan las zonas de memoria x e y para inicializarlas 
if (
(PID_BUCK2_B0 == 0x7FFF || PID_BUCK2_B0 == 0x8000) ||
(PID_BUCK2_B1 == 0x7FFF || PID_BUCK2_B1 == 0x8000))
{
while(1); //comprobacion de coeficientes en q15, si alguno es q15 entra en bucle infinito 
} 
*/
    /* ubica los coeficientes en sus posiciones de memoria */

Buck2VoltagePID.abcCoefficients[0] = PID_BUCK2_B0;
Buck2VoltagePID.abcCoefficients[1] = PID_BUCK2_B1;
Buck2VoltagePID.abcCoefficients[2] = PID_BUCK2_B2;
Buck2VoltagePID.abcCoefficients[3] = PID_BUCK2_A1;
Buck2VoltagePID.abcCoefficients[4] = PID_BUCK2_A2;
  
	Buck2VoltagePID.controlReference = PID_BUCK2_VOLTAGE_REF_MIN;

    Buck2VoltagePID.measuredOutput = 0;           
    //Buck2ReferenceOld=PID_BUCK2_VOLTAGE_REFERENCE;
    //Buck2ReferenceNew=PID_BUCK2_VOLTAGE_REFERENCE;
}

void Buck2ReferenceRoutine(void)
{
  /* This routine increments the control reference until the reference reaches 
     the desired output voltage reference. In this case the we have a softstart of 50ms 
also in this case controReference stars from 0.
   */
if (Buck2VoltagePID.controlReference<Buck2ReferenceOld)
{
	while (Buck2VoltagePID.controlReference <= Buck2ReferenceOld)
	{
		Delay_ms(1);
		Buck2VoltagePID.controlReference += BUCK2_SOFTSTART_INCREMENT;
	}
}
else
{
	while (Buck2VoltagePID.controlReference > Buck2ReferenceOld)
	{
		Delay_ms(1);
		Buck2VoltagePID.controlReference -= BUCK2_SOFTSTART_INCREMENT;
	}

}

}
void Buck2RefVoltValInit(void)
{
/*Inicializa la interrupcion por cambio en el pin 14/RB para poder cambiar la referencia
con el potenciometro*/
TRISBbits.TRISB8=1; //Pin 14 conectado a sw1 es una entrada digital.
IEC1bits.CNIE = 1; //Permite las interrupciones por Change Notification//
CNEN1bits.CN8IE=1;  //Cuando se detecte una transicion en 8 se genera la interrupcion CN
CNPU1bits.CN8PUE=1; //PullUP activado (no hace falta con sw1)
IPC4bits.CNIP=4; //Prioridad 4, inferior a la de PMW y conv AD
IFS1bits.CNIF=0; 	//Habilita interrupciones tipo Change Notification
}

void Delay_ms (unsigned int delay)
{
	TimerInterruptCount = 0;		//Clear Interrupt counter flag 
//PR1 = 0x9C40;						//(1ms / 25ns) = 40,000 = 0x9C40 
//IPC0bits.T1IP = 4;				 	//Set Interrupt Priority lower then ADC
//IEC0bits.T1IE = 1;					//Enable Timer1 interrupts 

T1CONbits.TON = 1;					//Enable Timer1 

while (TimerInterruptCount < delay); //Wait for Interrupt counts to equal delay

T1CONbits.TON = 0;					//Disable the Timer 
}

