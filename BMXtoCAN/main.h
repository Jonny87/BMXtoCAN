/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "I2C.h"
#include "can1_interface.h"
#include "stm32f4xx.h"
#include "filter.h"

#define setCycleTime 1
#define start_stop_CycleTime 2
#define MaxCycleTime	250

/*Externe Variablen f�r die Nutzung in anderen Modulen*/
extern __IO uint32_t TimingDelay;
extern __IO uint8_t  timerFlag_TIM7_1ms_cyclic;
extern __IO uint8_t  timerFlag_TIM7_10ms_cyclic;
extern __IO uint8_t  timerFlag_TIM7_50ms_cyclic;
extern __IO uint8_t  cycleTime;


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void Pin_Output_Init(void);
void Timer_Adjust (uint8_t cycleTime);


#endif /* __MAIN_H */