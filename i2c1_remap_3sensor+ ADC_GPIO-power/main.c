/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SHT20.h"
#include "PH_OEM.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private setup functions(don't use) ---------------------------------------------------------*/
void RCC_setup_HSI(void);
void RCC_setup_MSI(void);
/* Private user define functions ---------------------------------------------------------*/
void delay(unsigned long ms);
void send_byte(uint8_t b);
void usart_puts(char* s);

void USART2_IRQHandler(void);

void USART_Config(void);
void RCC_setup(void);
void GPIO_conf(void);

void i2c1_Init(void);
void i2c1_Init_Remap(void);
void i2c1_deinit(void);
void i2c1_remap_deinit(void);
void ADC_Config(void);
void ADC_deinit(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

// delay 1 ms per count @ Crystal 16.0 MHz 
void delay(unsigned long ms)
{
  volatile unsigned long i,j;
  for (i = 0; i < ms; i++ )
  for (j = 0; j < 1227; j++ );
}

void send_byte(uint8_t b)
{
  /* Send one byte */
  USART_SendData(USART2, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
void usart_puts(char* s)
{
    while(*s) {
      send_byte(*s);
        s++;
    }
}


char buffer[80] = {'\0'};

int main(void)
{
  // RCC_setup_HSI();
  RCC_setup();
  GPIO_conf();
  USART_Config();
  // // i2c1_Init();
  GPIO_SetBits(GPIOB,GPIO_Pin_0);
  GPIO_SetBits(GPIOB,GPIO_Pin_1);
  GPIO_SetBits(GPIOB,GPIO_Pin_5);


  float rawTemperature = 0;
  float rawHumidity = 0;
  float rawPH = 0;
  double tempTemperature = 0.00;
  double tempHumidity = 0.00;
  double tempPH = 0.00;
  int adc_value;

  while (1)
  {
    i2c1_Init_Remap();
    usart_puts("i2c1-init\n");

    delay(5000);
    usart_puts("i2c1-ready\n");

  

    if(SHT20ReadTemperature(I2C1, &rawTemperature))
    {
      tempTemperature = rawTemperature; // * (175.72 / 65536.0) -46.85;
      sprintf(buffer, "Temp1: %f\r\n", tempTemperature);
      usart_puts(buffer);
    }
    if(SHT20ReadHumidity(I2C1, &rawHumidity))
    {
      tempHumidity = rawHumidity; // * (175.72 / 65536.0) -46.85;
      sprintf(buffer, "Humi1: %f\r\n", tempHumidity);
      usart_puts(buffer);
    }


    i2c1_remap_deinit();


    i2c1_Init();
    usart_puts("i2c1-remap-init\n");
    OEM_ACTIVE(I2C1);
    delay(5000);
    usart_puts("i2c1-remap-ready\n");


    if(SHT20ReadTemperature(I2C1, &rawTemperature))
    {
      tempTemperature = rawTemperature; // * (175.72 / 65536.0) -46.85;
      sprintf(buffer, "Temp2: %f", tempTemperature);
      usart_puts(buffer);
    }

    if(SHT20ReadHumidity(I2C1, &rawHumidity))
    {
      tempHumidity = rawHumidity; // * (175.72 / 65536.0) -46.85;
      sprintf(buffer, "    Humi2: %f", tempHumidity);
      usart_puts(buffer);
    }

    if(OEM_READ_PH(I2C1, &rawPH))
    {
      tempPH = rawPH; // * (175.72 / 65536.0) -46.85;
      sprintf(buffer, "    PH: %f\r\n", tempPH);
      usart_puts(buffer);
    }
    
    OEM_DEACTIVE(I2C1);
    delay(500);
    i2c1_deinit();
    delay(1000);
    
    ADC_Config();
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);
    delay(1000);
    adc_value = ADC_GetConversionValue(ADC1);
    sprintf(buffer, "ADC-VALUE : %d\r\n", adc_value);
    usart_puts(buffer);
    ADC_deinit();
    delay(1000);
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
/*---------------------Function----------------------------*/
void RCC_setup_MSI(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable Internal Clock HSI */
  RCC_MSIRangeConfig(RCC_MSIRange_0);
  RCC_MSICmd(ENABLE);
  /* Wait till HSI is Ready */
  while(RCC_GetFlagStatus(RCC_FLAG_MSIRDY)==RESET);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div2);
  FLASH_SetLatency(FLASH_Latency_0);
  /* Enable PrefetchBuffer */
  FLASH_PrefetchBufferCmd(ENABLE);
  /* Set HSI Clock Source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);
  /* Wait Clock source stable */
  while(RCC_GetSYSCLKSource()!=0x00);
}


void RCC_setup(void)
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

void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
  
  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  
  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
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
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
}

void ADC_Config(void)
{

  //ADC
  ADC_InitTypeDef ADC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  // input of ADC PB12 (it doesn't seem to be needed, as default GPIO state is floating input)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PB13 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode =  DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_18, 1, ADC_SampleTime_4Cycles);
  ADC_ContinuousModeCmd(ADC1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);//enable ADC1
    /* Wait until the ADC1 is ready */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }
  // start conversion
  ADC_SoftwareStartConv(ADC1);// start conversion (will be endless as we are in continuous mode)

}

void ADC_deinit(void){

  ADC_Cmd(ADC1, DISABLE);//enable ADC1
  ADC_DeInit(ADC1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);

  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}


void i2c1_Init(void)
{
  I2C_InitTypeDef   I2C_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  // i2c1_DeInit();
  /*!< LM75_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
  /*!< LM75_I2C_SCL_GPIO_CLK, LM75_I2C_SDA_GPIO_CLK 
       and LM75_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); 

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_ClockSpeed = 50000;
  I2C_InitStructure.I2C_Mode =  I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 =0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;


  I2C_Init(I2C1, &I2C_InitStructure);

  I2C_Cmd(I2C1, ENABLE);
}

void i2c1_deinit(void){

  GPIO_InitTypeDef  GPIO_InitStructure;

  I2C_Cmd(I2C1, DISABLE);
  I2C_DeInit(I2C1);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void i2c1_Init_Remap(void)
{
  I2C_InitTypeDef   I2C_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  // i2c1_DeInit();
  /*!< LM75_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
  /*!< LM75_I2C_SCL_GPIO_CLK, LM75_I2C_SDA_GPIO_CLK 
       and LM75_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1); 

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_ClockSpeed = 50000;
  I2C_InitStructure.I2C_Mode =  I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 =0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;


  I2C_Init(I2C1, &I2C_InitStructure);

  I2C_Cmd(I2C1, ENABLE);
}


void i2c1_remap_deinit(void){

  GPIO_InitTypeDef  GPIO_InitStructure;

  I2C_Cmd(I2C1, DISABLE);
  I2C_DeInit(I2C1);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}



void GPIO_conf(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
// not sure 1ms/(1/Clock)*0.7675 - 1
