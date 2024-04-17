/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>

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
/* USER CODE BEGIN Variables */
/* TARVOS3 variables */
uint8_t checksum;
uint8_t rx_data[750];
char tx_data[750];
uint8_t size_of_data;

/* IMU variables */
int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;

int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;

float Ax, Ay, Az, Gx, Gy, Gz;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MPU6050_Read_Accel (void);
void MPU6050_Read_Gyro (void);
void MPU6050_Init (void);
void TARVOS3_Receive(void);
void TARVOS3_Transmit(void);
void TARVOS3_Init(void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  MPU6050_Init();

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    MPU6050_Read_Accel();
    HAL_Delay(500);
    MPU6050_Read_Gyro();
    HAL_Delay(500);
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TARVOS3_Transmit(void)
{
  HAL_UART_Transmit_IT(&huart1, (uint8_t*)tx_data, sizeof(tx_data));
  osDelay(1);
}

void TARVOS3_Receive(void)
{
  while(1){
    HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data));
    HAL_Delay(100);
    if(*rx_data == CONTINUE_COMMAND){
      break;
    }
  }
}

void TARVOS3_Init(void)
{
  checksum = START_SIGNAL ^ sizeof(CHANNEL_SELECTED) ^ CHANNEL_SELECTED;
  sprintf(tx_data, "%02X %02X %02X %02X %02X", START_SIGNAL, CMD_SET_CHANNEL_REQ, sizeof(CHANNEL_SELECTED), CHANNEL_SELECTED, checksum);
  TARVOS3_Transmit();
}

void MPU6050_Init(void)
{
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR,WHO_AM_I_REG,1, &check, 1, 1000);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1,&Data, 1, 1000);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 250 °/s
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
	}
}


void MPU6050_Read_Accel(void)
{
	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from ACCEL_XOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/

  size_of_data = sizeof(ACCELEROMETER_IDENTIFIER) + sizeof((uint8_t*)&Ax) + sizeof((uint8_t*)&Ay) + sizeof((uint8_t*)&Az);
  checksum = START_SIGNAL ^ size_of_data ^ ACCELEROMETER_IDENTIFIER ^ *(int8_t*)&Ax ^ *(int8_t*)&Ay ^ *(int8_t*)&Az;
  sprintf(tx_data, "%02X %02X %02X %02X %02X %02X %02X %02X", START_SIGNAL, CMD_DATA_REQ, size_of_data, ACCELEROMETER_IDENTIFIER, *(uint8_t*)&Ax, *(uint8_t*)&Ay, *(uint8_t*)&Az, checksum);
  TARVOS3_Transmit();

	Ax = Accel_X_RAW/16384.0;
	Ay = Accel_Y_RAW/16384.0;
	Az = Accel_Z_RAW/16384.0;
}


void MPU6050_Read_Gyro(void)
{
	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from GYRO_XOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	/*** convert the RAW values into dps (°/s)
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 131.0
	     for more details check GYRO_CONFIG Register              ****/

	Gx = Gyro_X_RAW/131.0;
	Gy = Gyro_Y_RAW/131.0;
	Gz = Gyro_Z_RAW/131.0;

  size_of_data = sizeof(ACCELEROMETER_IDENTIFIER) + sizeof((uint8_t*)&Gx) + sizeof((uint8_t*)&Gy) + sizeof((uint8_t*)&Gz);
  checksum = START_SIGNAL ^ size_of_data ^ ACCELEROMETER_IDENTIFIER ^ *(uint8_t*)&Gx ^ *(uint8_t*)&Gy ^ *(uint8_t*)&Gz;
  sprintf(tx_data, "%02X %02X %02X %02X %02X %02X %02X %02X", START_SIGNAL, CMD_DATA_REQ, size_of_data, ACCELEROMETER_IDENTIFIER, *(uint8_t*)&Gx, *(uint8_t*)&Gy, *(uint8_t*)&Gz, checksum);
  TARVOS3_Transmit();
}

/* Callback functions -----------------------------------------------*/
/**
  * @brief  UART receive complete callback, checks the UART port if it is the GNNS module sets the flag, else it checks for an abort command
  * @param  huart : UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart1)
  {
	  if(*rx_data == ABORT_COMMAND)
	  {
      if(Flight == TRUE)
      {
        HAL_GPIO_TogglePin(GPIOA, MOSFET1_Pin);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, MOSFET1_Pin);
        HAL_GPIO_TogglePin(GPIOB, MOSFET2_Pin);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOB, MOSFET2_Pin);
        HAL_NVIC_SystemReset();
      }
      else
      {
        HAL_NVIC_SystemReset();
      }
	  }
  }
  else if (huart == &huart3)
  {
    erinome_flag = 1;
  }
  HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data));
}

/**
  * @brief  UART transmit complete callback, empty the transmission data buffer
  * @param  huart : UART handle
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	memset(tx_data, 0, strlen(tx_data));
}

/* USER CODE END Application */

