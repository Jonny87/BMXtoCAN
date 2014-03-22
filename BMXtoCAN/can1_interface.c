/* Includes ------------------------------------------------------------------*/
#include "can1_interface.h"

/*Extern Variables --------------------------*/
CanRxMsg CAN1_steeringDataModulRx;
CanTxMsg CAN1_BMXyawrate;
CanTxMsg CAN1_BMXacc;
CanTxMsg CAN1_BMXMagnetometer;


/*Private Variables-----------------------------------------------*/
CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;


void CAN1_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* CAN GPIOs configuration **************************************************/

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN1_GPIO_CLK, ENABLE);

  /* Connect CAN pins to AF9 (CAN1) from PB*/
  GPIO_PinAFConfig(CAN1_GPIO_PORT, CAN1_RX_SOURCE, GPIO_AF_CAN1);
  GPIO_PinAFConfig(CAN1_GPIO_PORT, CAN1_TX_SOURCE, GPIO_AF_CAN1); 
  
  /* Configure CAN RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN1_RX_PIN | CAN1_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN1_GPIO_PORT, &GPIO_InitStructure);

  /* CAN configuration ********************************************************/  
  /* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN1_CLK, ENABLE);
  
  /* CAN register init */
  CAN_DeInit(CAN1);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    
  /* CAN Baudrate = 500 kBps (CAN clocked at 42 MHz) */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_14tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;

  //CAN_InitStructure.CAN_Prescaler=2;        // 1000 kbit/s
  CAN_InitStructure.CAN_Prescaler=4;        //  500 kbit/s
  //CAN_InitStructure.CAN_Prescaler=5;        //  400 kbit/s
  //CAN_InitStructure.CAN_Prescaler=8;        //  250 kbit/s
  //CAN_InitStructure.CAN_Prescaler=10;       //  200 kbit/s
  //CAN_InitStructure.CAN_Prescaler=16;       //  125 kbit/s
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/*Initalize the Messages from CAN1 Interface -----------------*/

void CAN1_Init_TxMessages ()
{
	  /* Transmit Structure preparation */

	CAN1_BMXyawrate.ExtId = 0x01;
	CAN1_BMXyawrate.RTR = CAN_RTR_DATA;
	CAN1_BMXyawrate.IDE = CAN_ID_STD  ;
	CAN1_BMXyawrate.StdId = 0x400;
	CAN1_BMXyawrate.DLC = 6;
	
	CAN1_BMXacc.ExtId = 0x01;
	CAN1_BMXacc.RTR = CAN_RTR_DATA;
	CAN1_BMXacc.IDE = CAN_ID_STD  ;
	CAN1_BMXacc.StdId = 0x401;
	CAN1_BMXacc.DLC = 6;
	
	CAN1_BMXMagnetometer.ExtId = 0x01;
	CAN1_BMXMagnetometer.RTR = CAN_RTR_DATA;
	CAN1_BMXMagnetometer.IDE = CAN_ID_STD  ;
	CAN1_BMXMagnetometer.StdId = 0x402;
	CAN1_BMXMagnetometer.DLC = 6;
}

void CAN1_Init_RxMessages()
{
  uint8_t ubCounter = 0;
	
	CAN1_steeringDataModulRx.StdId = 0x10;
	CAN1_steeringDataModulRx.ExtId = 0x01;
	CAN1_steeringDataModulRx.IDE = CAN_ID_STD;
	CAN1_steeringDataModulRx.DLC = 1;
	CAN1_steeringDataModulRx.FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    CAN1_steeringDataModulRx.Data[ubCounter] = 0x00;
  }
}

void CAN1_SendBMXyawrate(__IO BMX_VALUES* gyro)
{
	CAN1_BMXyawrate.Data[0] = gyro -> mean_value_x;
	CAN1_BMXyawrate.Data[1] = gyro -> mean_value_x >> 8;
	
	CAN1_BMXyawrate.Data[2] = gyro -> mean_value_y;
	CAN1_BMXyawrate.Data[3] = gyro -> mean_value_y >> 8;
	
	CAN1_BMXyawrate.Data[4] = gyro -> mean_value_z;
	CAN1_BMXyawrate.Data[5] = gyro -> mean_value_z >> 8;
 
	CAN_Transmit(CAN1, &CAN1_BMXyawrate);
}

void CAN1_SendBMXacc(__IO BMX_VALUES* acc)
{
	CAN1_BMXacc.Data[0] = acc -> mean_value_x;
	CAN1_BMXacc.Data[1] = acc -> mean_value_x >> 8;
	
	CAN1_BMXacc.Data[2] = acc -> mean_value_y;
	CAN1_BMXacc.Data[3] = acc -> mean_value_y >> 8;
	
	CAN1_BMXacc.Data[4] = acc -> mean_value_z;
	CAN1_BMXacc.Data[5] = acc -> mean_value_z >> 8;

	CAN_Transmit(CAN1, &CAN1_BMXacc);
}

void CAN1_SendBMXMagnetometer(__IO BMX_VALUES* mag)
{
	CAN1_BMXMagnetometer.Data[0] = mag -> mean_value_x;
	CAN1_BMXMagnetometer.Data[1] = mag -> mean_value_x >> 8;
	
	CAN1_BMXMagnetometer.Data[2] = mag -> mean_value_y;
	CAN1_BMXMagnetometer.Data[3] = mag -> mean_value_y >> 8;
	
	CAN1_BMXMagnetometer.Data[4] = mag -> mean_value_z;
	CAN1_BMXMagnetometer.Data[5] = mag -> mean_value_z >> 8;
	
	CAN_Transmit(CAN1, &CAN1_BMXMagnetometer);
}

/**
Configures the NVIC (Interrupt) for CAN.
  */
void CAN1_Interrupt_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;


  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



