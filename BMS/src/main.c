/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "serial.h"
#include "delay.h"


u16 data[5];  // data from adc
float s;      // scaling factor of the battery

void DMA1_Channel1_IRQHandler(void)//DMA1 IRQ HANDLER
{

if(DMA1->ISR&(1<<1))              //CHECKING THE FLAG
{

DMA1->IFCR|=0x03;}                // CLEARING THE TRANSFER

if(DMA1->ISR&(1<<3))              //CHECKING THE FLAG
{print("fail \n");
sendhex(DMA1->ISR);

DMA1->IFCR|=0x03;                 // CLEARING THE TRANSFER
DMA1 ->IFCR|=(1<<3);

sendhex(DMA1->ISR);
}

}


   /*GPIOC ->ODR |=(1);
	     GPIOC ->ODR &=~(1);

	     GPIOC ->ODR |=(1<<1);
	     GPIOC ->ODR &=~(1<<1);

	     GPIOC ->ODR |=(1<<2);
	     GPIOC ->ODR &=~(1<<2);

	     GPIOC ->ODR |=(1<<3);
	     GPIOC ->ODR &=~(1<<3);
	 */
void channel (u8 c ,u8 b)
 {
	GPIOC ->ODR &=~(1);    // SETTING ALL CHANNELS TO ZERO
    GPIOC ->ODR &=~(1<<1);
    GPIOC ->ODR &=~(1<<2);
    GPIOC ->ODR &=~(1<<3);

    if(b==1)
    {
	   switch (c)
       {
          case 1 : s=9.54;
        	       break;                //0

          case 2 : s=8.11;
        	       GPIOC ->ODR |=(1);    //1
                   break;

          case 3 : s=6.783;
        	       GPIOC ->ODR |=(1<<1);  //2
                   break;

          case 4 : s=4.85;
        	       GPIOC ->ODR |=(1);    //3
                   GPIOC ->ODR |=(1<<1);
                   break;

          case 5 : s=3.26;
        	       GPIOC ->ODR |=(1<<2); //4
                   break;

          case 6 : s=1.59;
        	       GPIOC ->ODR |=(1<<2); //5
                   GPIOC ->ODR |=(1);
                   break;
        }
    }
    else
    {
    	switch (c)
    	  {
    	    case 1 :s=9.54;
    	    	    GPIOC ->ODR |=(1<<2);  //7
    	            GPIOC ->ODR |=(1<<1);
    	            GPIOC ->ODR |=(1);
    	    	    break;

    	    case 2 : s=8.11;
    	    	     GPIOC ->ODR |=(1<<3); //8
    	             break;

    	    case 3 : s=6.608;
    	    	     GPIOC ->ODR |=(1<<3); //9
    	             GPIOC ->ODR |=(1);
    	             break;

    	    case 4 : s=4.85;
    	    	     GPIOC ->ODR |=(1<<3); //10
    	             GPIOC ->ODR |=(1<<1);
    	             break;

    	    case 5 : s=3.26;
    	    	     GPIOC ->ODR = (1<<1) | (1<<2); //6
    	             break;

    	    case 6 : s=1.59;
    	    	     GPIOC ->ODR |=(1<<2); //12
    	             GPIOC ->ODR |=(1<<3);
    	             break;
    	   }

    }

 }
/*GPIOC ->ODR |=(1);
	     GPIOC ->ODR &=~(1);1

	     GPIOC ->ODR |=(1<<1);2
	     GPIOC ->ODR &=~(1<<1);

	     GPIOC ->ODR |=(1<<2);4
	     GPIOC ->ODR &=~(1<<2);

	     GPIOC ->ODR |=(1<<3);8
	     GPIOC ->ODR &=~(1<<3);
	 */


int main(void)
{    float m=0; // variable used to manipulate digital value
     float a=0;
     u8 k=0;
   	// INITIALIZATION OF THE GPIO
	 RCC ->AHBENR |=(1<<19); // ENABLING CLOCK FOR PORT C
	 GPIOC ->MODER |=0X55; // TYPE OF GPIO
	// GPIOC ->PUPDR |=0XAA; // PULLED DOWN

   //ADC

	 // ALL INITIALIZATION
	 	serial_init();
	 	delay_init();

	 // RCC INITIALIZATION AND GPIO INITIALIZATION
	     RCC ->AHBENR|=(1<<28)|(1<<17);
	     GPIOA->MODER|=0XF;  // ANALOG CHANNEL 1 AND 2 PA0 AND PA1

	 //  INITIALIZE VOLTAGE REGULATOR
	     ADC1->CR =0x00;
	     delay(5);
	     ADC1->CR =(1<<28);
	     delay(5);


	 //  ALL THE CHANNELS ARE SELECTED TO SINGLE ENDED INPUT NOT DIFFERENTIAL INPUT
	 //  ENABLING INTERRUPT
	    // ADC1->IER|=(1<<2)|(1<<3);          // 2 :EOC 3: EOS INTERRUPT ENABLE




	 //  CHANNEL A0 AND THEN A1 IS SAMPLED
	     ADC1  ->SQR1=0X40;//0X2041
	     ADC1_2->CCR|=0X12000;

	 //  SAMPLING TIME SMPR1 SMPR2
         ADC1->SMPR1=0x38; // 111 601.5 +12.5 ADC CLOCK CYCLES


	 //  SELECTING SINGLE SCAN MODE CONT =0 CONTINOUS CONT =1
	     ADC1->CFGR|=(1<<13);   // COUNTINOUS MODE
	     ADC1->CFGR|=(1<<1);    // CIRCULAR MODE
	     ADC1->CFGR|=1;         // DMA ENABLE


	 // INITIALIZING THE CALIBRATION FRACTOR  --- 1 CALIBRATION CYCLE
	     ADC1 ->CR |=(1<<31);                    // INITIALIZING CALIBRATION
	     print("initializing calibration......");
	     while((ADC1->CR)&(1<<31));
	     print("calibration complete ");
	     print("\n");

	 //INITIALIZATION OF THE DMA1 CHANNEL 1
	     RCC->AHBENR|=0X1;                       // DMA1 ENABLE
	     DMA1_Channel1->CPAR=(uint32_t)&ADC1->DR; //ADDRESS INITIALIZATION OF THE ADC IN DMA REGISTER
	     DMA1_Channel1->CMAR=(uint32_t)data;    //ADDRESS OF THE MEMORY
	     DMA1_Channel1->CNDTR=1;              //NUMBER OF DATA TRANSFER
	     DMA1_Channel1->CCR|=0X35AA;             //CIRCULAR MODE AND ANOTHER CONFIGURATIONS
	 // ENABLING INTERRUPT FOR DMA
	     NVIC_EnableIRQ(DMA1_Channel1_IRQn);


	 // INITIALIZING DMA INTERRUPT

	     DMA1_Channel1->CCR|=0x01;               //ENABLING THE DMA CHANNEL

	 //NVIC_EnableIRQ(ADC1_2_IRQn);       // ENABLING INTERRUPT ON NVIC LINE

	 // INITIALIZING ADC
	     ADC1->CR |=0X01;
	     while(!(ADC1->ISR & 1));
	     delay(10);


	 // STARTING THE CONVERSION
	     ADC1->CR|=(1<<2);

	     while(1)

	     {


	     for( k=1;k<=6;k++)
	     {
	    	 channel(k,2);
	    	 print ("channel  ");
	    	 sendint(k);
	    	 print(":  ");

	    		       a=0;

	    		      for (int i=0;i<100;i++)
	    		   	    {
	    		   	    m=data[0];
	    		   	     m=m/4095;
	    		   	     m=m*2.9;
	    		   	     m=m*s;
	    		   	     a=a+m;
	    		   	    }
	    		         m=a*0.01;
/*
	    		         m=data[0];
	    		         	    		   	     m=m/4095;
	    		         	    		   	     m=m*2.9;

*/


	    		   	    sendint(m);
	    		   	    print("\n");
	     }



	     }


}
