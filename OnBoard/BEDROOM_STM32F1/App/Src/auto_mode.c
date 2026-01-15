#include "ndd_std_types.h"

#include <buzzer.h>
#include "auto_mode.h"

#include "convert.h"
#include "queue.h"

#include "message.h"
#include "../../BSW/Inc/Com/uart.h"
#include "../../BSW/Inc/globals.h"
#include "../../BSW/Inc/Output/led_rgb.h"
#include "../../BSW/Inc/Output/motor.h"

void Auto_Process(void)
{
    if (sys.mode == AUTO_MODE)
    {
        if (sys.temperature > 35.0f || sys.humidity < 35.0f || sys.gas_level > 500.0f)
        {
            Buzzer_On();

            LED_RGB_SetState(LED_RED);

            Motor_SetLevel(3);
        }
        else
        {
            Buzzer_Off();
//            if (sys.lux < 300.0f)
//            {
//                LED_RGB_SetState(LED_WHITE_100);
//            }
//            else if (sys.lux >= 300.0f && sys.lux < 600.0f)
//            {
//                LED_RGB_SetState(LED_WHITE_50);
//            }
//            else if (sys.lux >= 1200.0f)
//            {
//                LED_RGB_SetState(LED_OFF);
//            }
//            else
//            {
//
//                LED_RGB_Off();
//            }
            LED_RGB_SetState(LED_WHITE_100);

            if (sys.temperature > 30.0f)
            {
                Motor_SetLevel(3);
            }
            else if (sys.temperature <= 30.0f && sys.temperature > 25.0f)
            {
                Motor_SetLevel(2);
            }
            else if (sys.temperature <= 25.0f && sys.temperature > 20.0f)
            {
                Motor_SetLevel(1);
            }
            else
            {
                Motor_SetLevel(0);
            }
        }

        while (!empty(&g_uartQueue))
        {

            message_t *message = front(&g_uartQueue);

            if (message == NULL)
            {
                return;
            }



            if (message->header[0] == COMMAND && message->header[2] == 1 && message->header[1] == MODE)
            {

                uint8_t mode = message->payload[0];
                if (mode == 1)
                {

                    sys.mode = MANUAL_MODE;
                    uint8_t m = (uint8_t)sys.mode;
                    uint8_t data[20];
                    uint8_t length = Create_Message(NOTIFY, MODE, 1, &m, data);
                    USART1_Send_Data(data, length);
                    Delay_ms(10);

                }
            }

            pop(&g_uartQueue);
        }
    }
}
