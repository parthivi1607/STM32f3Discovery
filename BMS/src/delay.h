/*
 * delay.h
 *
 *  Created on: 23-Jul-2019
 *      Author: ayush
 */

#ifndef DELAY_H_
#define DELAY_H_


#include "stm32f30x.h"
#include "stm32f3_discovery.h"


void delay_init(void)
{
	// CLOCK TIMER 6 CONFIGURATION FOR THE DELAY
	RCC ->APB1ENR|=RCC_APB1ENR_TIM6EN;  // ENABLING CLOCK FOR TIMER 6
	TIM6 ->PSC=0X8CA0;                  // 2 KHZ FREQUENCY prescaler value 36000 means 2000 counts for 1 second
	TIM6 ->CR1|=TIM_CR1_ARPE;           // ENABLING CLOCK PRESCALAR
	TIM6 ->CR1|=TIM_CR1_OPM|TIM_CR1_URS ;
	

	}


void delay (u16 ms)
{
	 TIM6 ->ARR=ms*2; // COUNTER WILL COUNT UPTO ARR 
         TIM6 ->EGR  =1;                    // CREATES AN UPDATE EVENT WHICH INITIALIZES THE REGISTERS
	 TIM6 ->CR1|=TIM_CR1_CEN;
				  while(!((TIM6 ->SR)&1));
                  TIM6 ->SR =0X0000;
				  TIM6 ->CNT=0;
}




#endif /* DELAY_H_ */
