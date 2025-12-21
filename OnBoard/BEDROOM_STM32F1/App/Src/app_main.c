#include "app_main.h"
#include "stm32f1xx.h"
#include "stm32_rcc.h"
#include "message.h"
#include "queue.h"
#include "auto_mode.h"
#include "manual_mode.h"
#include "system_manager.h"
#include "timer_base.h"

#include "../../BSW/Inc/Com/uart.h"
#include "../../BSW/Inc/Input/dht11.h"
#include "../../BSW/Inc/Input/mq2.h"
#include "../../BSW/Inc/Input/pir.h"
#include "../../BSW/Inc/Output/Door.h"

UART_HandleTypeDef huart1;
FrameQueue g_uartQueue;
uint8_t uart_rx_buffer[FRAME_MAX_SIZE];

SystemState sys;

void App_Init(void)
{
	SystemClock_Config();
	USART1_Init(115200);
	Timer_Init();
	Queue_init(&g_uartQueue);
	device_init();

	DUNGX_UART_Receive_IT(&huart1, uart_rx_buffer, 2);

	pir_init();
	pir_test_init();
}

void App_Loop(void)
{
//	static uint32_t lastDeviceUpdateTick = 0;
//	static uint32_t lastProcessTick = 0;
//
//	uint32_t currentTick = GetTick();
//
//	// Task 1: Update device 200ms
//	if ((currentTick - lastDeviceUpdateTick) >= 200)
//	{
//		DeviceManager_UpdateData();
//		lastDeviceUpdateTick = currentTick;
//	}
//
//	// Task 2: Handle 50ms
//	if ((currentTick - lastProcessTick) >= 50)
//	{
//		if (sys.mode == AUTO_MODE)
//		{
//			Auto_Process();
//		}
//		else if (sys.mode == MANUAL_MODE)
//		{
//			Manual_Process();
//		}
//
//		lastProcessTick = currentTick;
//	}

	pir_process();
}
