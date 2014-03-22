#include "I2C.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_conf.h"
#include "main.h"


#define FLAG_TIMEOUT             ((uint32_t)0x1000)
#define LONG_TIMEOUT             ((uint32_t)(300 * FLAG_TIMEOUT))

__IO uint32_t  CODECTimeout = LONG_TIMEOUT;


void I2C2_Initialisation(void)
{		
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Modul clocks activate				
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// I2C1 output settings
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL_PIN  | I2C1_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Set alternate function for I2C1 output
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_I2C1);
	
	// Start I2C-Modul 
	Start_I2C1();

}

void Start_I2C1(void)
{
  I2C_InitTypeDef  I2C_InitStructure;

  // I2C2 Modul settings 	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x34;
	
	// Modul on
	I2C_Cmd(I2C2, ENABLE);	
	I2C_Init(I2C1, &I2C_InitStructure);
}


  int8_t Read_One_Simple_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr)
{
	int8_t wert;

  /*!< While the bus is busy */
  CODECTimeout = LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
	  if((CODECTimeout--) == 0) {return -1;}
  }

  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
	  if((CODECTimeout--) == 0) {return -2;}
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	  if((CODECTimeout--) == 0) {return -3;}
  }

  /* Transmit the register address to be read */
  I2C_SendData(I2Cx, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
    if((CODECTimeout--) == 0){return -4;}
  }

  /*!< Send START condition a second time */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  CODECTimeout = FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
    if((CODECTimeout--) == 0){return -5;}
  }

  /*!< Send Codec address for read */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Receiver);

	/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET) {
    if((CODECTimeout--) == 0) {return -6;}
  }

  /*!< Disable Acknowledgment */
  I2C_AcknowledgeConfig(I2Cx, DISABLE);

  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)I2Cx->SR2;

  /*!< Send STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Wait for the byte to be received */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
    if((CODECTimeout--) == 0){return -7;}
  }

  /*!< Read the byte received from the Codec */
  wert = I2C_ReceiveData(I2Cx);

  /* Wait to make sure that STOP flag has been cleared */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2Cx->CR1 & I2C_CR1_STOP) {
    if((CODECTimeout--) == 0){return -8;}
  }

  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  /* Clear AF flag for next communication */
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

  /* Return the byte read from Codec */
  return wert;
}


data_struct Read_One_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr)
{
	data_struct data;
	
  /*!< While the bus is busy */
  CODECTimeout = LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
	  if((CODECTimeout--) == 0) {data.status=-1; return TIMEOUT_UserCallback(data);}
  }

  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
	  if((CODECTimeout--) == 0) {data.status=-2; return TIMEOUT_UserCallback(data);}
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	  if((CODECTimeout--) == 0) {data.status=-3; return TIMEOUT_UserCallback(data);}
  }

  /* Transmit the register address to be read */
  I2C_SendData(I2Cx, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
    if((CODECTimeout--) == 0){ data.status=-4; return TIMEOUT_UserCallback(data);}
  }

  /*!< Send START condition a second time */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  CODECTimeout = FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
    if((CODECTimeout--) == 0){ data.status=-5; return TIMEOUT_UserCallback(data);}
  }

  /*!< Send Codec address for read */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Receiver);

	/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET) {
    if((CODECTimeout--) == 0) {data.status=-6; return TIMEOUT_UserCallback(data);}
  }

  /*!< Disable Acknowledgment */
  I2C_AcknowledgeConfig(I2Cx, DISABLE);

  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)I2Cx->SR2;

  /*!< Send STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Wait for the byte to be received */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
    if((CODECTimeout--) == 0){ data.status=-7; return TIMEOUT_UserCallback(data);}
  }

  /*!< Read the byte received from the Codec */
  data.value[0] = (int16_t)I2C_ReceiveData(I2Cx);

  /* Wait to make sure that STOP flag has been cleared */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2Cx->CR1 & I2C_CR1_STOP) {
    if((CODECTimeout--) == 0){ data.status=-8; return TIMEOUT_UserCallback(data);}
  }

  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  /* Clear AF flag for next communication */
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

  /* Return the byte read from Codec */
  return data;
}



data_struct Read_Multiple_Bytes(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr, uint8_t cnt)
{
  data_struct data;
  uint8_t value,n;

  if(cnt==0){ data.status=-9; return (data);}
  if(cnt>I2C1_MULTIBYTE_ANZ){ data.status=-10; return (data);}
	
	/*!< While the bus is busy */
  CODECTimeout = LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
	  if((CODECTimeout--) == 0){ data.status=-1; return TIMEOUT_UserCallback(data);}
  }

  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
	  if((CODECTimeout--) == 0) { data.status=-2; return TIMEOUT_UserCallback(data);}
  }

  if(cnt==1) {
    // ACK disable
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
  }
  else {
		// ACK enable
		I2C_AcknowledgeConfig(I2Cx, ENABLE);
  }
	
	/* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	  if((CODECTimeout--) == 0) { data.status=-3; return TIMEOUT_UserCallback(data);}
  }

  /* Transmit the register address to be read */
  I2C_SendData(I2C1, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
    if((CODECTimeout--) == 0) { data.status=-4; return TIMEOUT_UserCallback(data);}
  }

  /*!< Send START condition a second time */
  I2C_GenerateSTART(I2C1, ENABLE);

  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  CODECTimeout = FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
    if((CODECTimeout--) == 0) { data.status=-5; return TIMEOUT_UserCallback(data);}
  }

  /*!< Send Codec address for read */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Receiver);

   /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  CODECTimeout = FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET) {
    if((CODECTimeout--) == 0) { data.status=-6; return TIMEOUT_UserCallback(data);}
  }

  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
	(void)I2C1->SR2;
		
	for(n=0;n<cnt;n++) {

		if((n+1)>=cnt) {
			// ACK disable
			I2C_AcknowledgeConfig(I2Cx, DISABLE);
			// Stop-Sequenz
			I2C_GenerateSTOP(I2Cx, ENABLE);
		}
		
		/* Wait for the byte to be received */
		CODECTimeout = FLAG_TIMEOUT;
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
			if((CODECTimeout--) == 0) { data.status=-7; return TIMEOUT_UserCallback(data);}
		}

		// Read data
		value = I2C_ReceiveData(I2Cx);
		// Write data in array
		data.value[n]=value;
	}
	
  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  /* Clear AF flag for next communication */
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

  /* Return the byte read from Codec */
  return data;
} 

/**
  * @brief  Write one Byte from selected Device/Register
  * @param  Device address 
  * @note		8 Bit address
  * @param  The Register to be write
  * @param	The value to be write
  * @retval result whether the writing was passed/failed
  */
data_struct Write_One_Byte(I2C_TypeDef* I2Cx, uint8_t device, uint8_t RegisterAddr, uint8_t RegisterValue)
{
  data_struct data;
	
  /*!< While the bus is busy */
  CODECTimeout = LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
    if((CODECTimeout--) == 0) { data.status=-1; return TIMEOUT_UserCallback(data);}
  }

  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) {data.status=-2; return TIMEOUT_UserCallback(data);}
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx, device, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) { data.status=-3; return TIMEOUT_UserCallback(data);}
  }

  /* Transmit the first address for write operation */
  I2C_SendData(I2Cx, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((CODECTimeout--) == 0) { data.status=-4; return TIMEOUT_UserCallback(data);}
  }

  /* Prepare the register value to be sent */
  I2C_SendData(I2Cx, RegisterValue);

  /*!< Wait till all data have been physically transferred on the bus */
  CODECTimeout = LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF)) {
    if((CODECTimeout--) == 0) { data.status=-5; return TIMEOUT_UserCallback(data);}
  }

  /* End the configuration sequence */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return data;

}


/**
  * @brief  sensor management of the timeout situation.
  * @param  None.
  * @retval None.
  */
data_struct TIMEOUT_UserCallback(data_struct data)
{
  GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
	
	// Stop und Reset
  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_SoftwareResetCmd(I2C1, ENABLE);
  I2C_SoftwareResetCmd(I2C1, DISABLE);

  // I2C deinit
  I2C_DeInit(I2C1);
  // I2C restart
  Start_I2C1();
    
  return data;
}
   
