/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t tx_data[100]={};
uint8_t rx_data[100]={};
uint8_t BCD_data[100]={};
uint8_t checksum;

//Sensor de presion
int16_t h, p, temperatura,p_rel;

/* IMU variables */
int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;

int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void f2BCD(int16_t fdata);
void TARVOS3_Receive(void);
void TARVOS3_Transmit(void);
void TARVOS3_Init(void);
void Init_Seq(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  TARVOS3_Init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
  MX_USB_OTG_HS_HCD_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Transmit_IT(&huart1, (uint8_t*)tx_data, sizeof(tx_data));
  HAL_UART_Transmit_IT(&huart5, (uint8_t*)tx_data, sizeof(tx_data));
  HAL_UART_Receive_IT(&huart1, (uint8_t*)tx_data, sizeof(tx_data));
  HAL_UART_Receive_IT(&huart5, (uint8_t*)rx_data, sizeof(rx_data));
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
	  HAL_Delay(500);
	  HAL_UART_Transmit_IT(&huart1, "HOLA MUNDO",13);
	  //Init_Seq();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Init_Seq(void)
{
  	while(HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)==0)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		HAL_Delay(500);
	}
  	checksum = START_SIGNAL ^ sizeof(CONTINUE_COMMAND) ^ CONTINUE_COMMAND;
  	sprintf(tx_data, "%02X %02X %02X %02X %02X", START_SIGNAL, CMD_DATA_REQ, sizeof(CONTINUE_COMMAND), CONTINUE_COMMAND, checksum);
  	TARVOS3_Transmit();
}


void TARVOS3_Init(void)
{
  checksum = START_SIGNAL ^ sizeof(CHANNEL_SELECTED) ^ CHANNEL_SELECTED;
  sprintf(tx_data, "%02X %02X %02X %02X %02X", START_SIGNAL, CMD_SET_CHANNEL_REQ, sizeof(CHANNEL_SELECTED), CHANNEL_SELECTED, checksum);
  TARVOS3_Transmit();
}

void TARVOS3_Transmit(void)
{
  HAL_UART_Transmit_IT(&huart5, (uint8_t*)tx_data, sizeof(tx_data));
}

void TARVOS3_Receive(void)
{
  HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data));
}

void f2BCD(int16_t fdata)
{
	uint16_t i=3,dig;
	for(i=0;fdata!=0;i++)
	{
		fdata/=10;
		dig = i;
	}
	for(i=0;i>=dig;i++)
	{
		BCD_data[i] = fdata/(10^(dig-i));
	}

}



/*CALLBACK FUNCT*/


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart1)
  {
	  tx_data[0] = START_SIGNAL;
	  tx_data[1] = CMD_DATA_REQ;
	  tx_data[2] = sizeof(CHANNEL_SELECTED);
	  tx_data[3] = rx_data[0];
	  TARVOS3_Transmit();
  }

  if(huart == &huart5)
	{
	  tx_data[0] = rx_data[3];
	  tx_data[1] = rx_data[4];
	  tx_data[2] = rx_data[5];
	  tx_data[3] = rx_data[6];
	  HAL_UART_Transmit_IT(&huart1, (uint8_t*)tx_data,sizeof(tx_data));
	}


  HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data));
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	memset(tx_data, 0, strlen(tx_data));
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
