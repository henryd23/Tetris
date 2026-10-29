/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "GLCD/GLCD.h"
#include "../game.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int priority = 0;
volatile int key1 = 0;
volatile int key2 = 0;

void RIT_IRQHandler(void) {
		LPC_RIT->RICTRL  |= 0x1;
    static int up = 0;
		static int down = 0;
		static int left = 0;
		static int right = 0;
	
		/* Joytick UP pressed */
		if((LPC_GPIO1->FIOPIN & (1 << 29)) == 0){					
				up++;
				if(up == 1 || up % 4 == 0){
						if (priority == 0) {
								priority = 1;
								rotate_piece();
								priority = 0;
						}					
				}
		}
		else up = 0;
		
		/* Joytick DOWN pressed */
		if((LPC_GPIO1->FIOPIN & (1 << 26)) == 0){				
				down++;
				if(down == 1 || down % 4 == 0){
						if (priority == 0) {
								priority = 1;
								piece_fall(2);
								priority = 0;		
						}										
				}
		}
		else down = 0;		
		
		/* Joytick LEFT pressed */
		if((LPC_GPIO1->FIOPIN & (1 << 27)) == 0){			
				left++;
				if (left == 1 || left % 4 == 0) {
						if (priority == 0) {
								priority = 1;
								shift_piece(0);
								priority = 0;		
						}					
				}
		}
		else left=0;
		
		/* Joytick RIGHT pressed */
		if((LPC_GPIO1->FIOPIN & (1 << 28)) == 0){				
				right++;
				if (right == 1 || right % 4 == 0) {
							if (priority == 0) {
								priority = 1;
								shift_piece(1);
								priority = 0;		
						}	
				}
		}
		else right=0;
		
		/* button management */
		if(key1 != 0){
				if((LPC_GPIO2->FIOPIN & (1 << 11)) == 0){	/* KEY1 pressed */
						switch(key1){				
								case 2:
										if (flow == 0){
												play();
										}else if (flow == 1){
												pause();
										}else if (flow == 2){
												start();
										}
										break;
								default:
										break;
						}
						key1++;
				} else {
						key1=0;			
						NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
						LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
				}
		} else {
				if(key1 == 1)
						key1++;
		}
		
		if(key2 != 0){
				if((LPC_GPIO2->FIOPIN & (1 << 12)) == 0){	/* KEY1 pressed */
						switch(key2){				
								case 2:
										if (flow == 1){
												hard_drop();
										}
										break;
								default:
										break;
						}
						key2++;
				} else {
						key2=0;			
						NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
						LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
				}
		} else {
				if(key2 == 1)
						key2++;
		}
				
		reset_RIT();
		
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
		
		return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
