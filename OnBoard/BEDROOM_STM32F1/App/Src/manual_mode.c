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
        message_t *message = front(&g_uartQueue);
        if (message == NULL)
        {
            return;
        }
        if (message->header[0] == COMMAND && message->header[2] == 4)
        {
            switch (message->header[1])
            {
            case LED:
                float led_state = Convert_Bytes_To_Float(message->payload[0], message->payload[1], message->payload[2], message->payload[3]);
                LED_RGB_SetState(led_state);
                break;
            case FAN:
                float motor_level = Convert_Bytes_To_Float(message->payload[0], message->payload[1], message->payload[2], message->payload[3]);
                Motor_SetLevel(motor_level);

                break;
            case BUZZER:
                float buzzer_on = Convert_Bytes_To_Float(message->payload[0], message->payload[1], message->payload[2], message->payload[3]);
                if (buzzer_on > 0)
                {
                    Buzzer_On();
                }
                else
                {
                    Buzzer_Off();
                }
                break;
            case MODE:
                float mode = Convert_Bytes_To_Float(message->payload[0], message->payload[1], message->payload[2], message->payload[3]);
                if (mode == 0.0f)
                {
                    sys.mode = (uint8_t)AUTO_MODE;
                    uint8_t m = AUTO_MODE;
                    uint8_t data[20];
                    uint8_t length = Create_Message(NOTIFY, MODE, 1, &m, data);
                    USART1_Send_Data(data, length);
                }
                break;
            }
        }
        pop(&g_uartQueue);
    }
}
