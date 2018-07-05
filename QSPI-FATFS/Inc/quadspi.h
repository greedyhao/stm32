/**
  ******************************************************************************
  * File Name          : QUADSPI.h
  * Description        : This file provides code for the configuration
  *                      of the QUADSPI instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __quadspi_H
#define __quadspi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
//#define  sFLASH_ID                       0XEF4017     //W25Q64
#define  sFLASH_ID                         0XEF4018     //W25Q128

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)


/* W25Q128FV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            23     /* 地址总线宽度访问整个内存空间 */
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< 闪存大小 */
  uint32_t EraseSectorSize;    /*!< 擦除操作的扇区大小 */
  uint32_t EraseSectorsNumber; /*!< 擦除操作的扇区数 */
  uint32_t ProgPageSize;       /*!< 编程操作的页面大小 */
  uint32_t ProgPagesNumber;    /*!< 编程操作的页面数 */
} QSPI_Info;

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
/** 
  * @brief  W25Q128FV配置 
  */  
#define W25Q128FV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q128FV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q128FV_DUMMY_CYCLES_READ           4
#define W25Q128FV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q128FV_BULK_ERASE_MAX_TIME         250000
#define W25Q128FV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME    800

/** 
  * @brief  W25Q128FV 指令  
  */  
/* 复位操作 */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* 识别操作 */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* 读操作 */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* 写操作 */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* 寄存器操作 */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* 编程操作 */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12

/* 擦除操作 */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* 状态寄存器标志 */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
/*命令定义-结尾*******************************/


/*QSPI接口定义-开头****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __QSPI_CLK_ENABLE()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2                  
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB                       
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8                
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF                   
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7                
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6                
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_6               
#define QSPI_FLASH_CS_GPIO_PORT           GPIOB                   
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF10_QUADSPI

/* USER CODE END Private defines */

extern void _Error_Handler(char *, int);

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);


static uint8_t QSPI_ResetMemory          (void);
static uint8_t QSPI_WriteEnable          (void);
static uint8_t QSPI_AutoPollingMemReady  (uint32_t Timeout);

uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ quadspi_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
