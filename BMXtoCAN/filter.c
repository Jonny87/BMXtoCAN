/* Includes ------------------------------------------------------------------*/
#include "filter.h"

void addValueToHist(BMX_VALUES* struct_data){

   uint8_t i;
   
   i =  struct_data->hist_inc;
   struct_data->hist_x[i] =  struct_data->raw_value_x;
   struct_data->hist_y[i] =  struct_data->raw_value_y;
   struct_data->hist_z[i] =  struct_data->raw_value_z;
    
   struct_data->hist_inc++;
   
   if(struct_data->hist_inc == FILTER_NUMBER)
        struct_data->hist_inc = 0;
}

#ifdef COMPLEX 

void filter(BMX_VALUES* struct_data){
 
    struct_data->mean_value_x = calc_running_mean(struct_data->hist_x);
	struct_data->mean_value_y = calc_running_mean(struct_data->hist_y);
	struct_data->mean_value_z = calc_running_mean(struct_data->hist_z); 	

}

static int16_t calc_running_mean(int16_t hist[]){

	uint32_t sum;
	int8_t i;
	sum = 0;

		for(i=0;i<FILTER_NUMBER;i++){
			if(hist[i]==0)
				break;
				sum += (uint32_t)hist[i];
		}
		
	return (sum>>4); //Mittelwert = (Aufsummierte Messwerte) / (16 [FILTER_NUMBER])
}  
#endif
 
#ifdef SIMPLY

void filter(BMX_VALUES* struct_data){ 
	calc_mean(struct_data);	
}

static void calc_mean(BMX_VALUES* struct_data){

    /*y(t) = a * (y(t-1)) + (1-a) * x

	y(t): neuer Schätzwert
	y(t-1): alter Schätzwert
	x: neuer Messwert
	a: Gewicht im Bereich 0..1*/
	
	//a = 1/2
    /*struct_data ->mean_value_x  =  (struct_data ->mean_value_x + struct_data ->raw_value_x ) >> 1;
    struct_data ->mean_value_y  =  (struct_data ->mean_value_y + struct_data ->raw_value_y ) >> 1;
    struct_data ->mean_value_z  =  (struct_data ->mean_value_z + struct_data ->raw_value_z ) >> 1; */
	
	//a = 2/3
	struct_data ->mean_value_x  =  ((struct_data ->mean_value_x * 3) >> 2) + (struct_data ->raw_value_x  >> 2);
    struct_data ->mean_value_y  =  ((struct_data ->mean_value_y * 3) >> 2) + (struct_data ->raw_value_y  >> 2);
    struct_data ->mean_value_z  =  ((struct_data ->mean_value_z * 3) >> 2) + (struct_data ->raw_value_z  >> 2);

	//a = 1/3
	/*struct_data ->mean_value_x  =  (struct_data ->mean_value_x >> 2) + ((struct_data ->raw_value_x * 3)  >> 2);
    struct_data ->mean_value_y  =  (struct_data ->mean_value_y >> 2) + ((struct_data ->raw_value_y * 3)  >> 2);
    struct_data ->mean_value_z  =  (struct_data ->mean_value_z >> 2) + ((struct_data ->raw_value_z * 3)  >> 2);*/
}
#endif




/**
  * @}
  */


/****END OF FILE****/
