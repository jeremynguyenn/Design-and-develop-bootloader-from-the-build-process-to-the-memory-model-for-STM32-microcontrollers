#include <stdio.h>
#include "stm32f4xx.h"
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "bsp.h"

/*Modules:
 * FPU
 * UART
 * TIMEBASE
 * GPIO (BSP)
 * ADC
 * */

#define SECTOR0_BASE_ADDRESS		0x08000000 /*Bootloader sector*/
#define SECTOR1_BASE_ADDRESS		0x08004000 /*Default APP sector*/
#define SECTOR2_BASE_ADDRESS		0x08008000 /*APP1 sector*/
#define SECTOR3_BASE_ADDRESS		0x0800C000 /*Factory App sector*/


#define DEFAULT_APP_ADDRESS		SECTOR1_BASE_ADDRESS
#define APP1_ADDRESS			SECTOR2_BASE_ADDRESS
#define FACTORY_APP_ADDRESS		SECTOR3_BASE_ADDRESS


#define  GPIOAEN		(1U<<0)
#define  PIN5			(1U<<5)
#define  LED_PIN		PIN5

bool btn_state;


#define MSP_VERIFY_MASK			    0x2FFE0000
#define EMPTY_MEM					0xFFFFFFFF
typedef void(*func_ptr)(void);

#define MEM_CHECK_V2
volatile char g_ch_key;
volatile uint8_t g_un_key;



typedef enum
{
	APP1 =1,
	FACTORY_APP,
}SYS_APPS;


static void proces_btldr_cmds(SYS_APPS curr_app);

//void jmp_to_default_app(void)
//{
//	uint32_t app_start_address;
//	func_ptr jump_to_app;
//
//	printf("Bootloader Started....\n\r");
//	delay(300);
//
//	/*Version 1*/
//
//#ifdef MEM_CHECK_V1
//	if(((*(uint32_t *)APPLICATION_ADDRESS) & MSP_VERIFY_MASK ) ==  0x20020000)
//#endif
//
//#ifdef MEM_CHECK_V2
//	/*Version 2*/
//	if((*(uint32_t *)APPLICATION_ADDRESS) != EMPTY_MEM)
//#endif
//
//	{
//		printf("Starting application.....\n\r");
//		app_start_address =  *(uint32_t *)(APPLICATION_ADDRESS + 4);
//
//		jump_to_app = (func_ptr) app_start_address;
//
//		/*Initialialize main stack pointer*/
//		__set_MSP(*(uint32_t *)APPLICATION_ADDRESS);
//
//		/*jump*/
//		jump_to_app();
//
//	}
//	else
//	{
//		printf("No application found at location....\n\r");
//	}
//
//
//}

static void jump_to_app(uint32_t addr_value)
{
	uint32_t app_start_address;
	func_ptr jump_to_app;

	/*Version 1*/
#ifdef MEM_CHECK_V1
	if(((*(uint32_t *)addr_value) & MSP_VERIFY_MASK ) ==  0x20020000)
#endif

#ifdef MEM_CHECK_V2
	/*Version 2*/
	if((*(uint32_t *)addr_value) != EMPTY_MEM)
#endif

	{
		printf("Starting application.....\n\r");
		app_start_address =  *(uint32_t *)(addr_value + 4);

		jump_to_app = (func_ptr) app_start_address;

		/*Initialialize main stack pointer*/
		__set_MSP(*(uint32_t *)addr_value);

		/*jump*/
		jump_to_app();

	}
	else
	{
		printf("No application found at location....\n\r");
	}

}

int main()
{
	/*Enable FPU*/
	fpu_enable();

	/*Initialize debug UART*/
	system_uart_init();

	/*Initialize timebase*/
	timebase_init();

	/*Initialize LED*/
	led_init();

	/*Initialize Push button*/

	button_init();

	//jmp_to_default_app();

	if(get_btn_state())
	{
		/*Button pressed*/
		printf("DBG: Button Pressed \n\r");

		 /*Button pressed*/
		 printf("==========================================\n");
		 printf("==========================================\n");
		 printf("==========================================\n");
		 printf("==========================================\n");

		 printf("==========================================\n");
		 printf("\n");
		 printf("Bootloader Menu\n");
		 printf("\n");
		 printf("==========================================\n");
		 printf("==========================================\n");
		 printf("==========================================\n");

		 printf("Available Commands:\n");
		 printf("1       ==> Run App 1\n");
		 printf("F       ==> Factory App 2\n");
		 printf("Any key ==> Run Default App\n");

		while(1)
		{
			proces_btldr_cmds(g_un_key);
		}
	}
	else
	{
		/*Button not pressed*/
		jump_to_app(DEFAULT_APP_ADDRESS);
	}

	while(1)
	{

	}
}


static void proces_btldr_cmds(SYS_APPS curr_app)
{
	switch(curr_app)
	{
	case APP1:
		printf("APP1 Selected....\n\r");
		jump_to_app(APP1_ADDRESS);
		break;
	case FACTORY_APP:
		printf("Factory APP Selected....\n\r");

		jump_to_app(FACTORY_APP_ADDRESS);

		 break;
	default:
		break;
	}
}


static void uart_callback(void)
{
	g_ch_key = USART2->DR;

	if( g_ch_key ==  '1')
	{

		g_un_key = 1;
	}
	else if((g_ch_key ==  'f')  || (g_ch_key ==  'F'))
	{
		g_un_key = 2;

	}
	else
	{

	}
}

void USART2_IRQHandler(void)
{
	/*Check if RXNE is set*/
	if(USART2->SR & SR_RXNE)
	{
		uart_callback();
	}
}
