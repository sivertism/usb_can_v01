/* Include---- ------------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "core_cm4.h"

/* Global variables -------------------------------------------------------------------*/
#include "extern_decl_global_vars.h"
#include "SysTick_metoder.h"


/* Function declarations ---------------------------------------------------------------*/
void SysTick_init(void);
void SysTick_Handler(void);

uint8_t USART_getRxMessage(void);
uint8_t USART_getNewBytes(void);
void USART_transmit(uint8_t data);
uint8_t CAN_getRxMessages(void);
uint8_t CAN_getByteFromMessage(uint8_t filter_number, uint8_t byte_number);
void CAN_transmitByte(uint16_t StdId, uint8_t data);
void USART_datalog_transmit(uint8_t header, uint16_t data);
void USART_timestamp_transmit(uint8_t timestamp);

/* Function definitions ----------------------------------------------------------------*/



/**
 * @brief  Configures the SysTick timer for 100 Hz interrupt frequency.
 * @param  None
 * @retval None
 */
void SysTick_init(void) {
	NVIC_SetPriority(SysTick_IRQn, 1);
	SysTick->CTRL = 0; /* Disable SysTick */
	SysTick->LOAD = 72000000/100;  // 10 msek avbruddsintervall.
	SysTick->VAL = 0;
	SysTick->CTRL = (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk
			| SysTick_CTRL_CLKSOURCE_Msk);
} // end Systick_init()


/**
 * @brief  Configures the CAN-Controller peripheral for 500 kbps communication.
 * 		   Also configures Rx filters according to ID's specified in "can_metoder.h"
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {


	/* Check for new message on CAN and send through USB */
	if(CAN_getRxMessages()>0){
		//USART_transmit(CAN_getByteFromMessage(0,0));
		USART_timestamp_transmit(timestamp);
		message_to_pc = (uint16_t) ( (CAN_getByteFromMessage(2,1) << 8) | CAN_getByteFromMessage(2,0) );
		USART_datalog_transmit('G', message_to_pc);
		message_to_pc = (uint16_t) ( (CAN_getByteFromMessage(2,3) << 8) | CAN_getByteFromMessage(2,2) );
		USART_datalog_transmit('H', message_to_pc);
		message_to_pc = (uint16_t) ( (CAN_getByteFromMessage(2,5) << 8) | CAN_getByteFromMessage(2,4) );
		USART_datalog_transmit('I', message_to_pc);
		message_to_pc = (uint16_t) ( (CAN_getByteFromMessage(2,7) << 8) | CAN_getByteFromMessage(2,6) );
		USART_datalog_transmit('J', message_to_pc);

		if (timestamp >= 255) timestamp = 0;
		timestamp++;
	}

	/* Check for new Rx bytes from USB and send through CAN-BUS */
	if(USART_getNewBytes()>0){
		uint8_t receivedMsg = USART_getRxMessage();
		if (47 < receivedMsg && receivedMsg < 56){
			receivedMsg -= 48;
		} else {
			receivedMsg = 0;
		}
		CAN_transmitByte(0x301,receivedMsg);
	}


	/* Status LED */
	if (status_LED_cnt>50){
		GPIOE->ODR ^= SYSTICK_LED << 8;
		status_LED_cnt = 0;
	}
	status_LED_cnt++;

} // end Systick_Handler()
