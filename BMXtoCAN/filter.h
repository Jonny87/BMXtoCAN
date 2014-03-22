#ifndef __FILTER_H
#define __FILTER_H

#include "stm32f4xx_conf.h"
#include "BMX055.h"
#include "settings.h" 


	void filter(BMX_VALUES* struct_data);
	void addValueToHist(BMX_VALUES* struct_data);
		static int16_t calc_running_mean(int16_t hist[]);
	static void calc_mean(BMX_VALUES* struct_data);

#endif
