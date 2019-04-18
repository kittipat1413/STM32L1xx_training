#include "clock_controller.h"


/*---------------------Function----------------------------*/
void RCC_setup_HSI(void)
{
  /* RCC system reset(for debug purpose) */
  // RCC_DeInit();
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
void RCC_setup_MSI(uint32_t msi_speed)
{
  /* RCC system reset(for debug purpose) */
  // RCC_DeInit();
  /* Enable Internal Clock HSI */
  RCC_MSIRangeConfig(msi_speed);
  RCC_MSICmd(ENABLE);
  /* Wait till HSI is Ready */
  while(RCC_GetFlagStatus(RCC_FLAG_MSIRDY)==RESET);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div1);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  FLASH_SetLatency(FLASH_Latency_0);
  /* Enable PrefetchBuffer */
  FLASH_PrefetchBufferCmd(ENABLE);
  /* Set HSI Clock Source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);
  /* Wait Clock source stable */
  while(RCC_GetSYSCLKSource()!=0x00);
}
