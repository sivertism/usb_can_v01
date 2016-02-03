/*
 * File: init.c
 *
 * Brief: Contains function(s) for calling initialization functions
 */

/* Funtion Prototypes --------------------------------------------------------*/
void init(void);
void CAN_Config(void);
void GPIO_init(void);
void SysTick_init(void);
void USART_init();

/* Funtion Definitions -------------------------------------------------------*/

void init(void){
	GPIO_init();
	USART_init();
	CAN_Config();
	SysTick_init();
}
