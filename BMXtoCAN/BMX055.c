#include "BMX055.h"
/*
Die Sensordaten liegen in Zweierkomplementdarstellung in 
den Datenregistern der Sensoren. Daher müssen die Registerwerte in eine Variable 
mit dem Shift Operator zusammengeführt werden.
*/

void Read_BMX_ACC(BMX_VALUES* acc)
{
		data_struct acc_raw;

		acc_raw = Read_Multiple_Bytes(I2C1,BMX_ACC,0x02,6);	
		
		if(!(acc_raw.value[1]&0x0800)){
			if(!(acc_raw.value[0]&0x80))
				acc -> raw_value_x = ((uint16_t)acc_raw.value[1] << 4 | ((uint16_t)acc_raw.value[0] >> 4)); 
            else
				acc -> raw_value_x = ((uint16_t)acc_raw.value[1] << 4 | ((acc_raw.value[0]&0x00F0) >> 4));
		}
		else {
			if(!(acc_raw.value[0]&0x80))
				acc -> raw_value_x = ((int16_t)acc_raw.value[1] << 4 | ((uint16_t)acc_raw.value[0] >> 4));
			else
				acc -> raw_value_x = ((int16_t)acc_raw.value[1] << 4 | ((acc_raw.value[0]&0x00F0) >> 4));			
		}
       
		if(!(acc_raw.value[3]&0x0800)){
			if(!(acc_raw.value[2]&0x80))
				acc -> raw_value_y = ((uint16_t)acc_raw.value[3] << 4 | ((uint16_t)acc_raw.value[2] >> 4)); 
            else
				acc -> raw_value_y = ((uint16_t)acc_raw.value[3] << 4 | ((acc_raw.value[2]&0x00F0) >> 4));
		}
		else {
			if(!(acc_raw.value[2]&0x80))
				acc -> raw_value_y = ((int16_t)acc_raw.value[3] << 4 | ((uint16_t)acc_raw.value[2] >> 4));
			else
				acc -> raw_value_y = ((int16_t)acc_raw.value[3] << 4 | ((acc_raw.value[2]&0x00F0) >> 4));		
		}

        if(!(acc_raw.value[5]&0x0800)){
			if(!(acc_raw.value[4]&0x80))
				acc -> raw_value_z = ((uint16_t)acc_raw.value[5] << 4 | ((uint16_t)acc_raw.value[4] >> 4)); 
            else
				acc -> raw_value_z = ((uint16_t)acc_raw.value[5] << 4 | ((acc_raw.value[4]&0x00F0) >> 4));
		}
		else {
			if(!(acc_raw.value[4]&0x80))
				acc -> raw_value_z = ((int16_t)acc_raw.value[5] << 4 | ((uint16_t)acc_raw.value[4] >> 4));
			else
				acc -> raw_value_z = ((int16_t)acc_raw.value[5] << 4 | ((acc_raw.value[4]&0x00F0) >> 4));			
		}

		if(acc -> status == 0 && acc_raw.status == 0)
			acc -> status = acc_raw.status;
}


void Read_BMX_GYR(BMX_VALUES* gyro)
{
    data_struct gyr_raw;
		
    gyr_raw = Read_Multiple_Bytes(I2C1,BMX_GYR,0x02,6);
    
    /* Sort and  prepare read values */
    gyro -> raw_value_x = (gyr_raw.value[1] << 8) | (uint8_t)gyr_raw.value[0];
    gyro -> raw_value_y = (gyr_raw.value[3] << 8) | (uint8_t)gyr_raw.value[2];
    gyro -> raw_value_z = (gyr_raw.value[5] << 8) | (uint8_t)gyr_raw.value[4];
    gyro -> status = gyr_raw.status;   
}

void Read_BMX_MAG(BMX_VALUES* mag)
{
    data_struct mag_raw;
			
		mag_raw = Read_Multiple_Bytes(I2C1,BMX_MAG,0x42,6);
		
		/* Sort and  prepare read values */
		if(!(mag_raw.value[1]&0x0800)){
			if(!(mag_raw.value[0]&0x80))
				mag -> raw_value_x = ((uint16_t)mag_raw.value[1] << 5 | ((uint16_t)mag_raw.value[0] >> 3)); 
            else
				mag -> raw_value_x = ((uint16_t)mag_raw.value[1] << 5 | ((mag_raw.value[0]&0x00F8) >> 3));
		}
		else {
			if(!(mag_raw.value[0]&0x80))
				mag -> raw_value_x = ((int16_t)mag_raw.value[1] << 5 | ((uint16_t)mag_raw.value[0] >> 3));
			else
				mag -> raw_value_x = ((int16_t)mag_raw.value[1] << 5 | ((mag_raw.value[0]&0x00F8) >> 3));			
		}
		
		
		if(!(mag_raw.value[3]&0x0800)){
			if(!(mag_raw.value[2]&0x80))
				mag -> raw_value_y = ((uint16_t)mag_raw.value[3] << 5 | ((uint16_t)mag_raw.value[2] >> 3)); 
            else
				mag -> raw_value_y = ((uint16_t)mag_raw.value[3] << 5 | ((mag_raw.value[2]&0x00F8) >> 3));
		}
		else {
			if(!(mag_raw.value[2]&0x80))
				mag -> raw_value_y = ((int16_t)mag_raw.value[3] << 5 | ((uint16_t)mag_raw.value[2] >> 3));
			else
				mag -> raw_value_y = ((int16_t)mag_raw.value[3] << 5 | ((mag_raw.value[2]&0x00F8) >> 3));			
		}
		

		if(!(mag_raw.value[5]&0x0800)){
			if(!(mag_raw.value[4]&0x80))
				mag -> raw_value_z = ((uint16_t)mag_raw.value[5] << 7 | ((uint16_t)mag_raw.value[4] >> 1)); 
            else
				mag -> raw_value_z = ((uint16_t)mag_raw.value[5] << 7 | ((mag_raw.value[4]&0x00F8) >> 1));
		}
		else {
			if(!(mag_raw.value[4]&0x80))
				mag -> raw_value_z = ((int16_t)mag_raw.value[5] << 7 | ((uint16_t)mag_raw.value[4] >> 1));
			else
				mag -> raw_value_z = ((int16_t)mag_raw.value[5] << 7 | ((mag_raw.value[4]&0x00FE) >> 1));			
		}

        
    mag -> status = mag_raw.status;
}
