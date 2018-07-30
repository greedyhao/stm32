
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDRAM_H
#define __SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f7xx_hal_sdram.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup SDRAM
  * @{
  */

/** @defgroup SDRAM_Exported_Types
  * @{
  */
   
   
/** @defgroup SDRAM_Driver_structure
  * @{
  */ 
typedef struct
{
  void     (*Init)(SDRAM_HandleTypeDef*);
  void     (*WriteBuffer)(uint32_t*, uint32_t, uint32_t);
  void     (*ReadBuffer)(uint32_t*, uint32_t, uint32_t);
  uint8_t  (*Test)(void);
}SDRAM_DrvTypeDef;
/**
  * @}
  */
   
/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif /* __SDRAM_H */

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT greedyhao *****END OF FILE****/
