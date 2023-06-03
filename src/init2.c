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

tPID_PicoCorriente Buck2VoltagePID;
#define PID_BUCK2_VOLTAGE_REFERENCE 0x3319			/* Reference voltage is from resistor divider circuit R11 & R12 
													Voltage FB1 = (5kOhm / (5kOhm + 3.3kOhm)) * 3V = 1,8V
													Now calculate expected ADC value (1,8V * 1024)/3.3V = 560
													Then left shift by 5 for Q15 format (560 * 32) = 17945 */		 

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
fractional Buck2VoltageABC[5] __attribute__ ((section (".xbss, bss, xmemory")));
/*reserva de memoria en zona y, donde se ubicaran error y d actual y anteriores*/
fractional Buck2VoltageHistory[5] __attribute__ ((section (".ybss, bss, ymemory")));
/*el tamaño depende del tipo de regulador a implementar, en este caso tipo3*/


/*coeficientes del denominador de la FDT, solo parte entera, con signo cambiado*/
#define PID_BUCK2_A1 Q15(+0.676)
#define PID_BUCK2_A2 Q15(-0.676)
/*coeficientes del numerador de la FDT, parte entera*/
#define PID_BUCK2_B0 Q15(+0.054)
#define PID_BUCK2_B1 Q15(+0.05642)
#define PID_BUCK2_B2 Q15(-0.998)

//2.054 z^2 + 0.05642 z - 1.998
//-----------------------------
  // z^2 - 1.676 z + 0.6763


/*referencia1=3.3v, del sensor (divisor de tension)*/
/*5/(5+3.3)*3.3=1.988v--->(1.988*1024)/3.3v=617*/
/*ADCres=10, Q15res=15--->617*32=19744 = 0x4D20*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

//extern unsigned int TimerInterruptCount;
/*
extern unsigned int flag;
extern unsigned int reference1;
extern unsigned int reference2;
*/

void Buck2Drive(void)
{
//comparator configuration
//CMPCON1bits.RANGE=1 => 1,65V
//DACOUT AVAILABLE
CMPCON1=0xA081; 

// PTCON: PWM Time Base Control Register PWM no habilitado
PTCON = 0;	
// PTCON2: PWM Clock Divider Select Register
PTCON2 = 0;

	
	
    /* Buck2 converter setup to output 3.3V */ 

    IOCON2bits.PENH = 1;               	/* PWM2H is controlled by PWM module */
    IOCON2bits.PENL = 0;                /* PWM2L is not controlled by PWM module GIOP*/
    IOCON2bits.PMOD = 3;                /* 0 Complementary Mode generator 2, 3 indepent mode */
    IOCON2bits.POLH = 0;                /* Drive signals are active-high */
    IOCON2bits.POLL = 0;                /* Drive signals are active-high */
    IOCON2bits.OVRENH = 0;				/* Disable Override feature for shutdown PWM */  
    IOCON2bits.OVRENL = 1;				/* Disable Override feature for shutdown PWM */
    IOCON2bits.OVRDAT = 0b00;			/* Shut down PWM with Over ride 0 on PWMH and PWML */	
    IOCON2bits.FLTDAT = 0x01;			/* 1H = low; 1L = high*/
            
    PWMCON2bits.DTC = 0;            /* Peio Positive Deadtime enabled */
    
    DTR2    = 0x18;               	/* DTR = (25ns / 1.04ns), where desired dead time is 25ns. 
									     Mask upper two bits since DTR<13:0> */
    ALTDTR2 = 0x30;            		/* ALTDTR = (50ns / 1.04ns), where desired dead time is 50ns. 
									     Mask upper two bits since ALTDTR<13:0> */
    PWMCON2bits.MDCS = 0;		    /*duty cycle lo pone cada PDC*/
    PWMCON2bits.IUE = 0; 			/*inmediate update no active*/
    PWMCON2bits.DTC = 0; 	   
    PWMCON2bits.IUE = 0;            /* Disable Immediate duty cycle updates */
    PWMCON2bits.ITB = 0;            /* PTPER based period Select Primary Timebase mode */
    
    RPINR29bits.FLT1R = 33; 		//Assign PWM Fault Input 1 (FLT1) to the Corresponding RP33 Pin 
    RPOR16bits.RP33R = 0b100111; 	/* Analog Comparator Output 1 is Assigned to RP33 Output Pin*/

    LEBCON2bits.PHR = 1; /* Rising edge of PWMxH will trigger LEB counter */
	LEBCON2bits.PHF = 0; /* Falling edge of PWMxH is ignored by LEB counter */
	LEBCON2bits.PLR = 0; 			/* Rising edge of PWMxL will trigger LEB counter */
	LEBCON2bits.PLF = 0; 			/* Falling edge of PWMxL is ignored by LEB counter */
	LEBCON2bits.FLTLEBEN = 1; /* Enable fault LEB for selected source */
	LEBCON2bits.CLLEBEN = 0; /* Disable current-limit LEB for selected source */
	LEBCON2bits.LEB = 15; /* Blanking period of 8.04*15 ns */
    FCLCON2bits.FLTMOD = 1; 		/* Fault cycle by cycle */                 
    FCLCON2bits.FLTPOL = 0;		/* fault source is active high*/
    FCLCON2bits.FLTSRC = 0;	 		/*Fault 1 Selected for PWM Generator 2 Control Signal Source*/
    FCLCON2bits.IFLTMOD = 0;
	FCLCON2bits.CLSRC=3;
    TRGCON2bits.TRGDIV = 2;             /* Trigger interrupt generated every PWM cycle*/
    TRGCON2bits.TRGSTRT = 30;            /* Trigger generated from the begging of operation*/    
	                                                            
   PDC2 = ( PWM_PERIOD *0.8);                          
   TRIG2 = 200;		                 /* Trigger generated almost at beginning of PWM active period */
	
	TRISBbits.TRISB12=0;			/*Load como output digital */                                      
}




void CurrentandVoltageMeasurements(void)
{
    ADCONbits.FORM = 0;                   /* Integer data format */
    ADCONbits.EIE = 1;                    /* Early Interrupt disabled */
    ADCONbits.ORDER = 1;                  /* Convert odd channel first */
    ADCONbits.SEQSAMP = 0;                /* Select simultaneous sampling */
    ADCONbits.ADCS = 1;                   /* ADC clock = FADC/6 = 120MHz / 6 = 20MHz, 
											12*Tad = 1.6 MSPS, two SARs = 3.2 MSPS */

    IEC6bits.ADCP1IE = 1;				  /* Enable the ADC Pair 1 interrupt*/
    IFS6bits.ADCP1IF = 0;		    	  /* Clear ADC interrupt flag */ 
    IPC27bits.ADCP1IP = 5;			      /* Set ADC interrupt priority */ 
		
  
	ADPCFGbits.PCFG2 = 0; 			  	  /* PinConCFigAnalog Current Measurement for Buck 2 */ 
    ADPCFGbits.PCFG3 = 0; 				  /* PinConCFigAnalog Voltage Measurement for Buck 2 */

    ADPCFGbits.PCFG4 = 0; 				  /* PinConCFigAnalog Voltage Measurement for input voltage source */	
    

 
    ADSTATbits.P1RDY = 0; 				  /* Clear Pair 1, AN2&AN3, data ready bit */
    ADCPC0bits.IRQEN1 = 1;                /* Enable ADC Interrupt for Buck 2 control loop: current&voltage */
    ADCPC0bits.TRGSRC1 = 5; 			  /* ADC Pair 1 triggered by PWM2 */


    ADSTATbits.P2RDY = 0; 				  /* Clear Pair 2 data ready bit */
    ADCPC1bits.IRQEN2 = 0;                /* Disable ADC Interrupt for input voltage measurement */
    ADCPC1bits.TRGSRC2 = 4; 			  /* ADC Pair 2 triggered by PWM2 */
 	;ADCPC1bits.TRGSRC2=0;				/*La conversion se inicia por SWTRG*/

}


void Buck2VoltageLoop(void)
{
    Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients = Buck2VoltageABC;     /* Set up pointer to derived coefficients */
    Buck2VoltagePID.CtrlPicoCorriente.controlHistory = Buck2VoltageHistory;  /* Set up pointer to controller history samples */
    
PIDInitBuck2(&Buck2VoltagePID);                               


/* se llama a funcion pidinit, se le pasan las zonas de memoria x e y para inicializarlas */
if ((PID_BUCK2_A1 == 0x7FFF || PID_BUCK2_A1 == 0x8000) ||
(PID_BUCK2_A2 == 0x7FFF || PID_BUCK2_A2 == 0x8000) ||
(PID_BUCK2_B0 == 0x7FFF || PID_BUCK2_B0 == 0x8000) ||
(PID_BUCK2_B1 == 0x7FFF || PID_BUCK2_B1 == 0x8000) ||
(PID_BUCK2_B2 == 0x7FFF || PID_BUCK2_B2 == 0x8000))
{
while(1); /* comprobacion de coeficientes en q15, sialguno no es q15 entra en bucle infinito */
} 
    /* ubica los coeficientes en sus posiciones de memoria */

Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients[0] = PID_BUCK2_B0;
Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients[1] = PID_BUCK2_B1;
Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients[2] = PID_BUCK2_B2;
Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients[3] = PID_BUCK2_A1;
Buck2VoltagePID.CtrlPicoCorriente.abcCoefficients[4] = PID_BUCK2_A2;
  
	Buck2VoltagePID.CtrlPicoCorriente.controlReference = PID_BUCK2_VOLTAGE_REFERENCE;

}
