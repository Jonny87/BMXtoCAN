/*Includes-----------------------------*/
#include "main.h"


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN1_INTERFACE_H
#define __CAN1_INTERFACE_H



#define CAN1_CLK                    RCC_APB1Periph_CAN1
#define CAN1_RX_PIN                 GPIO_Pin_0
#define CAN1_TX_PIN                 GPIO_Pin_1
#define CAN1_GPIO_PORT           	GPIOD
#define CAN1_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define CAN1_RX_SOURCE              GPIO_PinSource0
#define CAN1_TX_SOURCE              GPIO_PinSource1 


/*Extern Variables -----------------------*/


/*Funktionsprototypen-----------------------------*/
void CAN1_Config(void);
void CAN1_Interrupt_Config(void);
void CAN1_Init_TxMessages(void);
void CAN1_Init_RxMessages(void);
void CAN1_SendBMXyawrate(__IO BMX_VALUES* gyro);
void CAN1_SendBMXacc(__IO BMX_VALUES* acc);
void CAN1_SendBMXMagnetometer(__IO BMX_VALUES* mag);


#endif /*__CAN1_INTERFACE_H*/
