#include "button.h"
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "../game.h"

extern volatile int key1;
extern volatile int key2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);        			/* disable Button interrupts  */
  LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection 				*/
  key1=1;
	LPC_SC->EXTINT &= (1 << 1);    						/* clear pending interrupt    */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);        			/* disable Button interrupts  */
  LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection 				*/
  key2=1;
  LPC_SC->EXTINT &= (1 << 2);     					/* clear pending interrupt    */    
}


