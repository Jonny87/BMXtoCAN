#ifndef __BMX055_H
#define __BMX055_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "I2C.h"
#include "settings.h" 

#define BMX_ACC		0x30 
#define BMX_GYR		0xD0
#define BMX_MAG		0x20
/* Typedef declarations */

typedef struct
{
	int16_t raw_value_x;
	int16_t raw_value_y;
	int16_t raw_value_z;
	int8_t  temp;
	int8_t	status;
	
	int16_t mean_value_x;
	int16_t mean_value_y;
	int16_t mean_value_z;
	
	int16_t hist_x[FILTER_NUMBER];
	int16_t hist_y[FILTER_NUMBER];
	int16_t hist_z[FILTER_NUMBER];
	uint8_t hist_inc;
	
} BMX_VALUES;


/* Function declaration */
void Read_BMX_ACC(BMX_VALUES* acc);
void Read_BMX_GYR(BMX_VALUES* gyro);
void Read_BMX_MAG(BMX_VALUES* mag);

#endif

