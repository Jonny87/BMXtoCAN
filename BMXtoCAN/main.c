/*############################################################################
F+E Projekt Sensoritegration am E-Kart
Datum: 21.03.2014
Autor: Jochen Rieger 
Beschreibung:
Diese Software liest über I2C die Beschleunigungs-, Drehraten- und Magnetometerdaten 
aus dem BMX055 von Bosch aus und sendet diese Daten (gefiltert) auf den CAN Bus.

In der Timer-ISR (alle 1ms) werden 3 Flags gesetzt. 1ms Flag, nach 10 Überläufen 
10ms (CAN) Flag und 50 Überläufen (MAG) Flag. Ist das ms-Flag gesetzt, werden der 
ACC und der GYRO ausgelesen. Ist das CAN-Flag gesetzt werden die ACC- und GYRO-Daten
auf den CAN gesendet. Ist das MAG-Flag gesetzt, wird das Magnetometer ausgelesen und 
auf den CAN gesendet. Nach jedem Auslesen eines Sensors, werden die Daten in ein Array 
mit maximal 16 Werten gelegt und gespeichert. 

Bevor die Sensordaten auf den CAN gesendet werden, wird ein gleitender Mittelwert der 
Daten berechnet. Hierzu stehen verschiedene Filtermethoden zur Ausahl, die unter 
Settings.h geändert werden kann.
##############################################################################
*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Extern variables ---------------------------------------------------------*/
__IO uint8_t  timerFlag_TIM7_1ms_cyclic;
__IO uint8_t  timerFlag_TIM7_10ms_cyclic;
__IO uint8_t  timerFlag_TIM7_50ms_cyclic;

__IO uint8_t cycleTime;
__IO uint32_t TimingDelay;
__IO uint8_t sensor_req_flag;

/* Private variables ---------------------------------------------------------*/
BMX_VALUES acc;
BMX_VALUES gyro;
BMX_VALUES mag;
     
__IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
	void Init_TimeBase (void);
	

int main(void){

	SystemInit();
	Pin_Output_Init();
	 						
	/* Initialize I2C peripherie */
	I2C2_Initialisation();

	CAN1_Interrupt_Config();
	CAN1_Config();
	CAN1_Init_TxMessages();
	CAN1_Init_RxMessages();
	Init_TimeBase();

	cycleTime = 10; //in ms

    
	Write_One_Byte(I2C1,BMX_MAG,0x4B,0x01); //Magnetometer  power-on Bit setzen
	Delay(1000000);
	Write_One_Byte(I2C1,BMX_MAG,0x4C,0x28); //Magnetometer von Suspend in Normal Mode setzen
			
	TIM_Cmd(TIM7, ENABLE); 
    
    while (1) {
		
		if(timerFlag_TIM7_50ms_cyclic) {			
			Read_BMX_MAG(&mag);
			addValueToHist(&mag);            filter(&mag);
			CAN1_SendBMXMagnetometer(&mag); // Identifier = 0x402
			
			timerFlag_TIM7_50ms_cyclic = 0;
		}
		
        if(timerFlag_TIM7_1ms_cyclic) {
			
			Read_BMX_GYR(&gyro);					
			Read_BMX_ACC(&acc);

            addValueToHist(&gyro);
			addValueToHist(&acc);
			
			timerFlag_TIM7_1ms_cyclic = 0;
		}
		
        if(timerFlag_TIM7_10ms_cyclic) {
	    
            Read_BMX_GYR(&gyro);					
			Read_BMX_ACC(&acc);
			
			addValueToHist(&gyro);
			addValueToHist(&acc);
						
			filter(&gyro);			filter(&acc);
			            		
            CAN1_SendBMXacc(&acc);          //Identifier = 0x401
            CAN1_SendBMXyawrate(&gyro);     //Identifier = 0x400                
            timerFlag_TIM7_10ms_cyclic = 0; 		         	
	}

		/*printf("\nX-Achse:  %f", acc.mean_value_x*0.00098);
		printf("\tY-Achse:  %f", acc.mean_value_y*0.00098);
		printf("\tZ-Achse:  %f", acc.mean_value_z*0.00098); */ 

        /*printf("\nX-Achse:  %f", gyro.mean_value_x*0.06103);
		printf("\tY-Achse:  %f", gyro.mean_value_y*0.06103);
        printf("\tZ-Achse:  %f", gyro.mean_value_z*0.06103); */

		/* printf("\nX-Achse:  %d", mag.mean_value_x*16);
		printf("\tY-Achse:  %d", mag.mean_value_y*16);
		printf("\tZ-Achse:  %d", mag.mean_value_z*16); */
	}
 }
 


int fputc(int c, FILE *stream) {
   return(ITM_SendChar(c));
} 

void Init_TimeBase (void)
{	 
    NVIC_InitTypeDef NVIC_InitStructure;
    /* TIM7 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    // Timer disable
    TIM_Cmd(TIM7, DISABLE);

    /*Timer7 configuration------------------------------------------------
    Zeit errechnet sich wie folgt:

    Timerclock/(Prescaler * Autoreload) = 1/Time

    Time = 1/(84 MHz / (2 * 42000)) = 1ms
    */

    TIM_PrescalerConfig(TIM7, 1, TIM_PSCReloadMode_Update); /* 84MHz/2 = 42 MHz Timer Taktung */
    TIM_SetAutoreload(TIM7, 42000); /*42000 entspricht ca. ms*/
    /* Enable TIM7 Update interrupt */
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    /* Enable the TIM7 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 	
}


void Pin_Output_Init(void)  {
   GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable Reset GPIO Clock */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

   /* Audio reset pin configuration -------------------------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOD, &GPIO_InitStructure);
 }


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */

void Delay(__IO uint32_t nTime)
{
   while(nTime--){
		}
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @}
  */


/****END OF FILE****/
