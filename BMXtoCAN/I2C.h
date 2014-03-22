#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"


#define I2C1_SCL_PIN        GPIO_Pin_6
#define I2C1_SDA_PIN        GPIO_Pin_7

#define I2C1_MULTIBYTE_ANZ   8       // anzahl der Bytes


/* Typedef declarations */

typedef struct   
{
	__IO  int8_t value[I2C1_MULTIBYTE_ANZ];
	__IO  int8_t status;

}data_struct;

/* Function declaration */
void I2C2_Initialisation(void);


data_struct Read_One_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr);
data_struct Read_Multiple_Bytes(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr, uint8_t cnt);
data_struct Write_One_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr, uint8_t RegisterValue);
int8_t Read_One_Simple_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr);
void Start_I2C1(void);
data_struct TIMEOUT_UserCallback(data_struct data);

void wait(__IO uint32_t nTime);

#endif
