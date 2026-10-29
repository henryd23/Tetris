/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include <stdlib.h>
#include <time.h>
#include "game.h" 

#define ROWS 20
#define COLS 10

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

volatile int flow;
volatile int field[ROWS][COLS];
volatile int high_score;
volatile int score;
volatile int cleared_lines;

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();									/* LCD Initialization              		*/
		
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	
	init_RIT(0x002DC6C0);									/* RIT Initialization 250 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	
	init_timer(0, 0x00B71B00);						/* TIMER Initialization 1 sec					*/
	enable_timer(0);											/* TIMER enabled											*/
	
	start();	
	
	srand(LPC_TIM0->TC ^ LPC_RIT->RICOUNTER);

	LPC_SC->PCON |= 0x1;                  /* power-down    mode                 */
  LPC_SC->PCON &= ~(0x2);
					
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
