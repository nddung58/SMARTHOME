#include "ndd_std_types.h"
#include <buzzer.h>
#include "manual_mode.h"
#include "convert.h"
#include "queue.h"

#include "../../BSW/Inc/globals.h"
#include "../../BSW/Inc/Output/led_rgb.h"
#include "../../BSW/Inc/Output/motor.h"

void Manual_Process(void)
{
    while (!empty(&g_uartQueue) && (sys.mode == MANUAL_MODE))
    {
    	GPIOC->ODR |= (1 << 13);
        message_t *message = front(&g_uartQueue);
        if (message == NULL)
        {
            return;
        }
        if (message->header[0] == COMMAND)
        {
            switch (message->header[1])
            {
            case LED:
                LED_RGB_SetState(message->payload[0]);
                break;
            case FAN:
                Motor_SetLevel(message->payload[0]);

                break;
            case BUZZER:

                if (message->payload[0] > 0)
                {
                    Buzzer_On();
                }
                else
                {
                    Buzzer_Off();
                }
                break;
            case MODE:
                if (message->payload[0] == 0)
                {
                    sys.mode = (uint8_t)AUTO_MODE;
                    uint8_t m = AUTO_MODE;
                    uint8_t data[20];
                    uint8_t length = Create_Message(NOTIFY, MODE, 1, &m, data);
                    USART1_Send_Data(data, length);
                    Delay_ms(10);
                }
                break;
            }
        }
        pop(&g_uartQueue);
    }
}
