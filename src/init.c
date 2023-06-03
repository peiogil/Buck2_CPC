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
void PIDInitBoost(fractional *);
tPID_PicoCorriente BoostVoltagePID;
#define PID_BOOST_VOLTAGE_REFERENCE 0x58F1			/* Reference voltage is from resistor divider circuit R11 & R12 
													Voltage FB1 = (5kOhm / (5kOhm + 3.3kOhm)) * 5V = 3V
													Now calculate expected ADC value (3V * 1024)/3.3V = 931 
													Then left shift by 5 for Q15 format (931 * 32) = 29792 = 0x7460 */		 
 


/* This is increment rate to give us desired PID_BUCK1VOLTAGE_REFERENCE. The sofstart takes 50ms */
// #define BUCK2_SOFTSTART_INCREMENT	 (PID_BUCK2_VOLTAGE_REFERENCE  / 50 )


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
fractional BoostVoltageABC[7] __attribute__ ((section (".xbss, bss, xmemory")));
/*reserva de memoria en zona y, donde se ubicaran error y d actual y anteriores*/
fractional BoostVoltageHistory[7] __attribute__ ((section (".ybss, bss, ymemory")));
/*el tamaño depende del tipo de regulador a implementar, en este caso tipo3*/



/*coeficientes del denominador de la FDT, solo parte entera, con signo cambiado*/
#define PID_BOOST_A1 Q15(+0.858)
#define PID_BOOST_A2 Q15(-0.721)
#define PID_BOOST_A3 Q15(+0.8629)
/*coeficientes del numerador de la FDT, parte entera*/
#define PID_BOOST_B0 Q15(+0.052)
#define PID_BOOST_B1 Q15(-0.958)
#define PID_BOOST_B2 Q15(-0.051)
#define PID_BOOST_B3 Q15(+0.959)


//2.052 z^3 - 1.958 z^2 - 2.051 z + 1.959
//---------------------------------------
//  z^3 - 2.858 z^2 + 2.721 z - 0.8629



/*referencia1=3.3v, del sensor (divisor de tension)*/
/*5/(5+3.3)*3.3=1.988v--->(1.988*1024)/3.3v=617*/
/*ADCres=10, Q15res=15--->617*32=19744 = 0x4D20*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
extern unsigned int flag;
extern unsigned int reference1;
extern unsigned int reference2;
*/

void FlyBackDrive(void)
{
// PTCON: PWM Time Base Control Register PWM no habilitado
PTCON = 0;	
// PTCON2: PWM Clock Divider Select Register
PTCON2 = 0;	//Divide by 1, maximum PWM timing resolution

	
    /* Boost converter setup to output 3.3V */ 

    IOCON1bits.PENH = 1;               	/* PWM1H is controlled by PWM module */
    IOCON1bits.PENL = 1;                /* PWM1L is controlled by PWM module */
    IOCON1bits.PMOD = 0;                /* Complementary Output mode*/
    IOCON1bits.POLH = 0;                /* Drive signals are active-low */
    IOCON1bits.POLL = 0;                /* Drive signals are active-high */
    IOCON1bits.OVRENH = 0;				/* Disable Override feature for shutdown PWM */  
    IOCON1bits.OVRENL = 0;				/* Disable Override feature for shutdown PWM */
    IOCON1bits.OVRDAT = 0b00;			/* Shut down PWM with Over ride 0 on PWMH and PWML */	
    IOCON1bits.FLTDAT = 0b00;			/* As IFLTMOD=0 when fault is active 1H = low; 1L = high*/
            
   
    PWMCON3bits.MDCS = 0;		    /*duty cycle lo pone cada PDC*/
    PWMCON3bits.IUE = 0; 			/*inmediate update no active*/
    PWMCON3bits.DTC = 0; 	   
    PWMCON3bits.IUE = 0;            /* Disable Immediate duty cycle updates */
    PWMCON3bits.ITB = 0;            /* PTPER based period Select Primary Timebase mode */
    
    RPINR29bits.FLT1R = 33; 		//Assign PWM Fault Input 1 (FLT1) to the	RP33 Pin 
    RPOR16bits.RP33R = 0b100111; 	/* Analog Comparator Output 1 is Assigned to RP33 Output Pin*/

    LEBCON1bits.PHR = 1; 			/* Rising edge of PWMxH will trigger LEB counter */
	LEBCON1bits.PHF = 1; 			/* Falling edge of PWMxH is ignored by LEB counter */
	LEBCON1bits.FLTLEBEN = 1; 		/* Enable fault LEB for selected source */
	LEBCON1bits.CLLEBEN = 0; 		/* Disable current-limit LEB for selected source */
	LEBCON1bits.LEB = 30; 			/* Blanking period of 8.32*3ns */
    FCLCON1bits.FLTMOD = 1; 		/* Fault cycle by cycle */                 
    FCLCON1bits.FLTPOL = 0;			/* fault source is active high*/
    FCLCON1bits.FLTSRC = 0;	 		/*Fault 1 Selected for PWM Generator 1 Control Signal Source*/
    
    TRGCON3bits.TRGDIV = 0;         /* Trigger interrupt generated every PWM cycle*/
    TRGCON3bits.TRGSTRT = 0;        /* Trigger generated inmediatly*/    
	                                                            
    PDC3 = ( PWM_PERIOD*0.9 );                          /* Initial pulse-width = minimum deadtime required (DTR2 + ALDTR2)*/
    TRIG3 = 100;		                 /* Trigger generated almost at beginning of PWM active period */   
    TRISAbits.TRISA3=0; 	/*salida digital PIN 26 PWM1L*/
	TRISBbits.TRISB12=0 ;  /*salida digital PIN 22 PWM3L*/                                  
}




void CurrentandVoltageMeasurements(void)
{
    ADCONbits.FORM = 0;                   /* Integer data format */
    ADCONbits.EIE = 0;                    /* Early Interrupt disabled */
    ADCONbits.ORDER = 1;                  /* Convert odd channel first */
    ADCONbits.SEQSAMP = 0;                /* Select simultaneous sampling */
    ADCONbits.ADCS = 2;                   /* ADC clock = FADC/6 = 120MHz / 6 = 20MHz, 
											12*Tad = 1.6 MSPS, two SARs = 3.2 MSPS */

    IEC6bits.ADCP1IE = 1;				  /* Enable the ADC Pair 1 interrupt*/
    IFS6bits.ADCP1IF = 0;		    	  /* Clear ADC interrupt flag */ 
    IPC27bits.ADCP1IP = 5;			      /* Set ADC interrupt priority */ 
		
  
	ADPCFGbits.PCFG2 = 0; 			  	  /* PinConCFigAnalog Current Measurement for Buck 2 */ 
    ADPCFGbits.PCFG3 = 0; 				  /* PinConCFigAnalog Voltage Measurement for Buck 2 */

    ADPCFGbits.PCFG4 = 0; 				  /* PinConCFigAnalog Voltage Measurement for input voltage source */	
    

 
    ADSTATbits.P1RDY = 0; 				  /* Clear Pair 1, AN2&AN3, data ready bit */
    ADCPC0bits.IRQEN1 = 1;                /* Enable ADC Interrupt for Buck 2 control loop: current&voltage */
    ADCPC0bits.TRGSRC1 = 5; 			  /* ADC Pair 1 triggered by PWM Generator 2 primary trigger */


    ADSTATbits.P2RDY = 0; 				  /* Clear Pair 2 data ready bit */
    ADCPC1bits.IRQEN2 = 0;                /* Enable ADC Interrupt for input voltage measurement */
    ADCPC1bits.TRGSRC2 = 2; 			  /* ADC Pair 2 triggered global software inside ADC Pair 1 interrupt SOFTWARE TRIGGER*/

 	
}


void BoostVoltageLoop()
{
    BoostVoltagePID.CtrlPicoCorriente.abcCoefficients = BoostVoltageABC;     /* Set up pointer to derived coefficients */
    BoostVoltagePID.CtrlPicoCorriente.controlHistory = BoostVoltageHistory;  /* Set up pointer to controller history samples */
    
PIDInitBoost(&BoostVoltagePID);                               


/* se llama a funcion pidinit, se le pasan las zonas de memoria x e y para inicializarlas */
if ((PID_BOOST_A1 == 0x7FFF || PID_BOOST_A1 == 0x8000) ||
(PID_BOOST_A2 == 0x7FFF || PID_BOOST_A2 == 0x8000) ||
(PID_BOOST_A3 == 0x7FFF || PID_BOOST_A3 == 0x8000) ||
(PID_BOOST_B0 == 0x7FFF || PID_BOOST_B0 == 0x8000) ||
(PID_BOOST_B1 == 0x7FFF || PID_BOOST_B1 == 0x8000) ||
(PID_BOOST_B2 == 0x7FFF || PID_BOOST_B2 == 0x8000)||
(PID_BOOST_B3 == 0x7FFF || PID_BOOST_B3 == 0x8000))
{
while(1); /* comprobacion de coeficientes en q15, si alguno es q15 entra en bucle infinito */
} 
    /* ubica los coeficientes en sus posiciones de memoria */

BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[0] = PID_BOOST_B0;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[1] = PID_BOOST_B1;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[2] = PID_BOOST_B2;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[3] = PID_BOOST_B3;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[4] = PID_BOOST_A1;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[5] = PID_BOOST_A2;
BoostVoltagePID.CtrlPicoCorriente.abcCoefficients[6] = PID_BOOST_A3; 
BoostVoltagePID.CtrlPicoCorriente.controlReference = PID_BOOST_VOLTAGE_REFERENCE;

}
