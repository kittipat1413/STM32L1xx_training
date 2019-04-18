#include "timer.h"

void TIM4_IRQHandler(void){

  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    GPIO_ToggleBits(GPIOA,GPIO_Pin_4);


  }
}

void TIMER4_Init(void){

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

  TIM_DeInit(TIM4);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);

  TIM_TimeBaseInitStruct.TIM_Prescaler        = 1-1;
  TIM_TimeBaseInitStruct.TIM_CounterMode      = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period           = 1000;
  TIM_TimeBaseInitStruct.TIM_ClockDivision    = TIM_CKD_DIV1;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  /* TIM IT enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM4, ENABLE);


  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

}

