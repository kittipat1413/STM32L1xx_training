/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_conf.h"


/* Private setup functions ---------------------------------------------------------*/


void TIMER4_Init(void);

void TIM4_IRQHandler(void);



#endif 

