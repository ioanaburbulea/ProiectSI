#include <DAVE.h> // Declarations from DAVE Code Generation (includes SFR declaration)

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

// LEDs Blinking:
#define ON 1
#define OFF 0

// SoftwareTimer:
#define ONESEC 1000000U

// ***** Global Variables: *****
// LEDs Blinking:
bool bLEDsBlinkingStatusGlobal = OFF; // used by the BLINKING INTERRUPT
bool bLEDsOnOffStatusGlobal = OFF; // used by the fLEDs_TOGGLE function
// Transmit:
const unsigned char ucArrayMenu[] =
			 "\n"
			 "1 ... LEDs ON\n"
			 "2 ... LEDs OFF\n"
			 "3 ... LEDs Toggle\n"
			 "4 ... LEDs Blinking\n"
			 "\n"
			 "your choice: ";

unsigned int ucStringLength1 = 0;
unsigned int ucStringLength2 = 0;

char mb[200];
unsigned int iMainMenuLoopCounter = 0;

bool bTransmitBusy = true;

// Receive:
uint8_t ucUartReceivedDataGlobal = '\0';
volatile unsigned int uiReceivedCounterGlobal = 0;
unsigned int uiReceivedCounterOLD_Global = 0;

// ***** function prototypes: *****
// Transmit:

void fMyPrintf(const unsigned char *p);
void fEndOfTransmitCallback(void);

// LEDs:
void fLEDs_ON (void);
void fLEDs_OFF (void);
void fLEDs_TOGGLE (void);

// LEDs Blinking:
void fEverySecond (void);

int main(void)
{
	uint32_t SoftwareTimer;
	 SYSTIMER_STATUS_t statusSYSTIMER;

	DAVE_Init(); // Initialization of DAVE APPs

	SoftwareTimer =(uint32_t)SYSTIMER_CreateTimer(ONESEC,SYSTIMER_MODE_PERIODIC,(void*)fEverySecond,NULL);
	 if (SoftwareTimer != 0U)
	 {
		 ; // Software timer is created successfully
	 }
	 else
	 {
		 while (1); // Software timer creation is failed
	 }
	 statusSYSTIMER = SYSTIMER_StartTimer(SoftwareTimer);
	 if (statusSYSTIMER == SYSTIMER_STATUS_SUCCESS)
	 {
		 ; // Software timer is running
	 }
	 else
	 {
		 while (1); // Error during Software timer start operation
	 }

	iMainMenuLoopCounter++;
	sprintf(mb,"\n\n\n*** MainMenuLoopCounter = %d ***",iMainMenuLoopCounter); // Write formatted data to string mb (MesssageBuffer)
	fMyPrintf(mb);
	while (bTransmitBusy);
	fMyPrintf(ucArrayMenu);
	while (bTransmitBusy);
 while(1U)
 {

	 if (uiReceivedCounterOLD_Global != uiReceivedCounterGlobal)
	  {
		 switch (ucUartReceivedDataGlobal)
		  {
		  case '1': bLEDsBlinkingStatusGlobal = OFF, fLEDs_ON(),fMyPrintf("\n*** LEDs ON !!!***\n");   while(bTransmitBusy);
		  break;
		  case '2': bLEDsBlinkingStatusGlobal = OFF, fLEDs_OFF(),fMyPrintf("\n*** LEDs OFF !!! ***\n"); while (bTransmitBusy);
		  break;
		  case '3': bLEDsBlinkingStatusGlobal = OFF, fLEDs_TOGGLE(),fMyPrintf("\n***  LEDs TOGGLED !!! ***\n"); while (bTransmitBusy);
		  break;
		  case '4': bLEDsBlinkingStatusGlobal = ON,fMyPrintf("\n***  LEDs BLINKING !!! ***\n"); while (bTransmitBusy);
		  break;
		  case '5': ;
		  break;
		  default : ;
		  break;
		  }
	  uiReceivedCounterOLD_Global = uiReceivedCounterGlobal;
	  iMainMenuLoopCounter++;
	  sprintf(mb,"\n\n\n*** MainMenuLoopCounter = %d***",iMainMenuLoopCounter); // Write formatted data to string mb
	  fMyPrintf(mb);
	  while (bTransmitBusy);
	  fMyPrintf(ucArrayMenu);
	  while (bTransmitBusy);
	 }
 }
 }

void fLEDs_ON (void)
{
	DIGITAL_IO_SetOutputHigh(&LED_P5_P8);
	DIGITAL_IO_SetOutputHigh(&LED_P5_P9);

	bLEDsOnOffStatusGlobal = ON;
}
void fLEDs_OFF (void)
{
	DIGITAL_IO_SetOutputLow(&LED_P5_P8);
	DIGITAL_IO_SetOutputLow(&LED_P5_P9);
	bLEDsOnOffStatusGlobal = OFF;
}
void fLEDs_TOGGLE (void)
{
		if (bLEDsOnOffStatusGlobal == ON)
		{
			fLEDs_OFF();
		}
		else
		{
			fLEDs_ON();
		}
}
void fEverySecond (void)
{
	if (bLEDsBlinkingStatusGlobal == ON)
	{
		fLEDs_TOGGLE();
	}
}
void INT_UART_Receive_ISR (void)
{
	uiReceivedCounterGlobal++;
	UART_Receive(&UART_0, &ucUartReceivedDataGlobal, 1);
}
 void fEndOfTransmitCallback(void)
 {
	 bTransmitBusy = false;
 }
 void fMyPrintf(const unsigned char *p)
 {
	 unsigned char *helpStartTextPointer = p;
	 bTransmitBusy = true; // will be cleared by fEndOfTransmitCallback
	 ucStringLength1 = 0;
	 ucStringLength2 = strlen(p);
	 while(*p)
	 {
		 ucStringLength1++;
		 *p++;
	 }
	 if(ucStringLength1 == ucStringLength2)
	 {
		 UART_Transmit(&UART_0, helpStartTextPointer, ucStringLength2);
	 }
 }
