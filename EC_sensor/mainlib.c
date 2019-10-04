#include "main.h"
#include "CircularBuffer.h"
#include "ATparser.h"
#include "libEC/EC_rs485.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

circular_buf_t cbuf;
atparser_t parser;

void USART1_IRQHandler(void);

void init_usart1(void);
void init_usart2(void);
void RCC_setup_HSI(void);
void GPIO_setup(void);

void send_byte1(uint8_t b);
void usart_puts1(char* s);
void send_byte2(uint8_t b);
void usart_puts2(char* s);

void delay(unsigned long ms);


int readFunc(uint8_t *data);
int writeFunc(uint8_t *buffer, size_t size);
bool readableFunc(void);
void sleepFunc(int us);




uint8_t data_BUF[9] = {'\0'};


void USART1_IRQHandler(void)
{
    uint8_t b;
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {

          b =  USART_ReceiveData(USART1);
          circular_buf_put(&cbuf, b);

        }
}



void delay(unsigned long ms)
{
  volatile unsigned long i,j;
  for (i = 0; i < ms; i++ )
  for (j = 0; j < 1460; j++ );
}

void send_byte1(uint8_t b)
{
  /* Send one byte */
  USART_SendData(USART1, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
void usart_puts1(char* s)
{
    while(*s) {
      send_byte1(*s);
        s++;
    }
}
void send_byte2(uint8_t b)
{
  /* Send one byte */
  USART_SendData(USART2, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
void usart_puts2(char* s)
{
    while(*s) {
      send_byte2(*s);
        s++;
    }
}

/*---------------------Function----------------------------*/
void RCC_setup_HSI(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable Internal Clock HSI */
  RCC_HSICmd(ENABLE);
  /* Wait till HSI is Ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div2);
  FLASH_SetLatency(FLASH_Latency_0);
  /* Enable PrefetchBuffer */
  FLASH_PrefetchBufferCmd(ENABLE);
  /* Set HSI Clock Source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait Clock source stable */
  while(RCC_GetSYSCLKSource()!=0x04);
}

void GPIO_setup(void)
{
  /* GPIO Sturcture */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable Peripheral Clock AHB for GPIOB */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
  /* Configure PC13 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void init_usart1(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Enable USART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  
  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   //USARTx configuration ----------------------------------------------------
  //  USARTx configured as follow:
  // - BaudRate = 230400 baud  
  // - Word Length = 8 Bits
  // - One Stop Bit
  // - No parity
  // - Hardware flow control disabled (RTS and CTS signals)
  // - Receive and transmit enabled
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  

  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}

void init_usart2(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  
  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  
  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);

  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follow:
  - BaudRate = 230400 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  

  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
}


// ATparser callback function
int readFunc(uint8_t *data)
{
  // printf("<");
  return circular_buf_get(&cbuf, data);
}

int writeFunc(uint8_t *buffer, size_t size)
{
  size_t i = 0;
  for (i = 0; i < size; i++)
  {
    send_byte1(buffer[i]);
  }
  return 0;
}

bool readableFunc()
{
  return !circular_buf_empty(&cbuf);
}

void sleepFunc(int ms)
{
  delay(ms);
}




int main(void)
{
  
  RCC_setup_HSI();
  init_usart1();
  init_usart2();
  GPIO_setup();

  GPIO_SetBits(GPIOB, GPIO_Pin_13);
  usart_puts2("\r\nInit OK\r\n");
  
  circular_buf_init(&cbuf);
  atparser_init(&parser, readFunc, writeFunc, readableFunc, sleepFunc);
  EC_rs485_init(&parser, &data_BUF[0]);


  char buffer[100];
  uint16_t Temp;
  uint16_t Humi;
  uint16_t Salt;
  uint16_t EC;
  

  while (1)
  { 
    if(EC_rs485_readTempHumi(&Temp,&Humi)){
    
      sprintf(buffer,"\nHumid: %d ",Humi);
      usart_puts2(buffer);
      sprintf(buffer," Temp: %d \n",Temp);
      usart_puts2(buffer);      
      }
    else{
      usart_puts2("Read temp&humi fail\n");
      atparser_flush(&parser); 
    }


    if(EC_rs485_readSalt(&Salt)){

      sprintf(buffer,"\nSalt(mg/L): %d ",Salt);
      usart_puts2(buffer);   
      }
    else{
      usart_puts2("Read salt fail\n");
      atparser_flush(&parser);
    }


    if(EC_rs485_readEC(&EC)){
  
      sprintf(buffer,"\nEC(us/cm): %d ",EC);
      usart_puts2(buffer);      
      }
    else{
      usart_puts2("Read ec fail\n");
      atparser_flush(&parser);
    }
  
  }
}

