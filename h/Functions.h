/****************************************************************************
*
* functions.h
* Interface to the DSP Library for the dsPIC30F.
*
****************************************************************************/
#include "dsp.h"
#define	G_PWM_FREQUENCY	100.0e3
//#define	PWM_PERIOD	(1.0e9/(1.04 * G_PWM_FREQUENCY))
void Buck1Drive(void);
void Buck2Drive(void);
void Buck2DriveCPC(void);
void CurrentandVoltageMeasurements(void);
void Buck1VoltageLoop(void);
void Buck2VoltageLoop(void);
void BoostVoltageLoop(void);
void Buck1SoftStartRoutine(void);
void Buck2SoftStartRoutine(void);
void BoostSoftStartRoutine(void);
void Delay_ms(unsigned int delay);
void PIDInitBuck2(tPID *);
void Buck2RefVoltValPotIntr(void);
void Buck2ReferenceRoutine(void);
void InitUART1(void);
