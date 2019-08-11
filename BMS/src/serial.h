/*
 * serial.h
 *
 *  Created on: 26-Jul-2019
 *      Author: ayush
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "string.h"
#include "stdio.h"

void serial_init()
    {
    	  // UART5 CONFIGURATION

    	       //GPIO FOR UART5
    	       RCC ->AHBENR |=RCC_AHBENR_GPIOCEN;           // ENABLING CLOCK FOR PORT C
    	       RCC ->APB1ENR|=(1<<20);                      // ENABLING CLOCK FOR USART 5
    	       GPIOC->AFR[1] |=(1<<18)|(1<<16);             // alternate function 5 for usart from datasheet
    	       GPIOC ->MODER |=(1<<25);                     // PC-12 tx pin


    	       // UART5 configurations
    	       //  1 start ,stop bit 8 wordlength lsb first
    	       UART5 ->BRR = 0xEA6;             //115200 BAUD 0X271 // 96000 x0EA6
    	       UART5 ->CR1 |=USART_CR1_TE ;     //transmission enabled
    	       UART5 ->CR1 |=USART_CR1_UE ;     // usart enable
    }



    void print(char* abc)         // TO PRINT STRING
    {   u32 k=strlen(abc);
    	for(u32 i=0;i<k;i++)
    	{
    		 while (!((UART5 ->ISR) & USART_ISR_TXE));
    		 UART5 ->TDR = abc[i];
    	}


    	abc='\0';

    }



    void sendint(float a)        // TO PRINT INTEGER
    {
    	char abc[10];
    	snprintf(abc,5,"%f",a);
    	print(abc);
    }


    void sendhex(u32 n )        // TO PRINT HEXADECIMAL INTEGER
    {
    	char hex[10]="\0";
        char m;
        u8 i = 0;
        u8 j=0;
        u8 temp  = 0;
      while(n!=0)
      {
        temp = n % 16;
        if(temp < 10)
        {
            hex[i] = temp + 48;
            i++;
        }
        else
        {
            hex[i] = temp + 55;
            i++;
        }
        n = n/16;
      }

        temp=i/2;

       	for( j=0;j<temp;j++)
       	{ m=hex[j];
       	  hex[j]=hex[i-1-j];
       	hex[i-1-j]=m;
    	print(hex);
       	}

    }



#endif /* SERIAL_H_ */
