/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* Radio module | TARVOS3 */
#define CHANNEL_SELECTED  0x6C
#define CONTINUE_COMMAND  0xF2
#define ABORT_COMMAND     0xF1
#define ACCELEROMETER_IDENTIFIER  0X41
#define GYROSCOPE_IDENTIFIER      0X47
#define MAGNETOMETER_IDENTIFIER   0X4D
#define PRESSURE_IDENTIFIER       0X50
#define GNNS_IDENTIFIER           0X4C
/* Transmit comands */
#define START_SIGNAL          0x02
#define CMD_DATA_REQ          0x00
#define CMD_DATAEX_REQ        0x01
#define CMD_SET_MODE_REQ      0x04
#define CMD_RESET_REQ         0x05
#define CMD_SET_CHANNEL_REQ   0x06
#define CMD_SET_DESTNETID_REQ 0x07
#define CMD_SET_DESTADDR_REQ  0x08
#define CMD_SET_REQ           0x09
#define CMD_GET_REQ           0x0A
#define CMD_RSSI_REQ          0x0D
#define CMD_SHUTDOWN_REQ      0x0E
#define CMD_STANDBY_REQ       0x0F
#define CMD_SET_PAPOWER_REQ   0x11
#define CMD_FACTORY_RESET_REQ 0x12  

/* IMU */
#define MPU6050_ADDR 0xD0
#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
