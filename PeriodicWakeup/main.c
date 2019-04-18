/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "clock_controller.h"
#include "timer.h"

/** @addtogroup Template_Project
  * @{
  */

void RCC_Config(void);
void USART2_setup(void);
void RTC_WKUP_IRQHandler(void);
void EXTI20_Config(void);
void RTC_Wakeup_Init(void);


void usart1_putc(char c);
void usart1_puts(char *s);
/* Private user define functions ---------------------------------------------------------*/
void delay(unsigned long ms);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

RTC_InitTypeDef   RTC_InitStructure;
RTC_TimeTypeDef   RTC_TimeStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;


void USART2_setup(void)
{
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2,ENABLE);

  /* GPIO Sturcture */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable Peripheral Clock AHB for GPIOB */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
  /* Configure PC13 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void usart1_putc(char c)
{
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET); //wait until transmittion readdy
  USART_SendData(USART2,(uint8_t)c);
}
void usart1_puts(char *s)
{
  while(*s) // Check end of string
  {
    usart1_putc(*s++); //send character 1 time
  }
}

void RTC_WKUP_IRQHandler(void){

  if(RTC_GetFlagStatus(RTC_FLAG_WUTF) != RESET){

    RTC_ClearFlag(RTC_FLAG_WUTF);
  }


}

void EXTI20_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


  RTC_ITConfig(RTC_IT_WUT,ENABLE);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void RTC_Wakeup_Init(void){

  PWR_ClearFlag(PWR_FLAG_WU);
  EXTI20_Config();
  RTC_OutputConfig(RTC_Output_WakeUp, RTC_OutputPolarity_High);

  /* Clear PWR WakeUp flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Clear RTC Alarm A flag */ 
  RTC_ClearFlag(RTC_FLAG_WUTF);
  
  RTC_WakeUpCmd(DISABLE);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(30);
  RTC_WakeUpCmd(ENABLE);

}

uint8_t uart_buffer[200];
int main(void)
{
  // RCC_setup_HSI();

  SystemCoreClockUpdate();
  delay(1);

  USART2_setup();


  usart1_puts("RCC\r\n");
  RCC_Config();

  RTC_Wakeup_Init();

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  sprintf(uart_buffer,"%d:%d:%d Alarm = %d:%d:%d \r\n",RTC_TimeStructure.RTC_Hours,
                                      RTC_TimeStructure.RTC_Minutes, 
                                      RTC_TimeStructure.RTC_Seconds,
                                      RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours,
                                      RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, 
                                      RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
  usart1_puts(uart_buffer);

  // delay(300);
  PWR_EnterSTANDBYMode();

  while(1){
  usart1_puts("Fuck\r\n");
  }
}


void RCC_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC Domain */
  PWR_RTCAccessCmd(ENABLE);

  /* Check if the StandBy flag is set */
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB);


  // usart1_puts("C_SB\r\n");
    /* Turn on LED2 */
    // STM_EVAL_LEDOn(LED2); ////DO SOMETHING
    
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    /* No need to configure the RTC as the RTC config(clock source, enable,
       prescaler,...) are kept after wake-up from STANDBY */
  }else if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET){

    PWR_ClearFlag(PWR_FLAG_WU);

  // usart1_puts("C_WU\r\n");
    RTC_WaitForSynchro();

  }
  else
  {
    /* RTC Configuration ******************************************************/
    /* Reset RTC Domain */
    RCC_RTCResetCmd(ENABLE);
    RCC_RTCResetCmd(DISABLE);

  // usart1_puts("Reset_\r\n");
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);
    
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Set the RTC time base to 1s */
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0x00FF;
    RTC_Init(&RTC_InitStructure);


    /* Set the time to 01h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x21;
    RTC_TimeStructure.RTC_Minutes = 0x07;
    RTC_TimeStructure.RTC_Seconds = 0x40;  
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
    delay (50);
  }

  /* Clear RTC Alarm A flag */ 
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}

// delay 1 ms per count @ Crystal 16.0 MHz 
void delay(unsigned long ms)
{
  volatile unsigned long i,j;
  for (i = 0; i < ms; i++ )
  for (j = 0; j < 1227; j++ );
}

