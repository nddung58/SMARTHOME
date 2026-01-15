#include "system_manager.h"
#include "message.h"

#include "../../BSW/Inc/Com/uart.h"
#include "../../BSW/Inc/globals.h"
#include "../../BSW/Inc/Input/cds.h"
#include "../../BSW/Inc/Input/dht11.h"
#include "../../BSW/Inc/Input/mq2.h"
#include "../../BSW/Inc/Input/pir.h"
#include "../../BSW/Inc/Input/rain.h"

#include "../../BSW/Inc/Output/led_rgb.h"
#include "../../BSW/Inc/Output/motor.h"
#include "../../BSW/Inc/Output/buzzer.h"
#include "../../BSW/Inc/Output/door.h"

void device_init(void)
{

    DHT11_Init();
    CDS_Init();
    MQ2_Init();
    pir_init();
    rain_init();

    LED_RGB_Init();
    Buzzer_Init();


    door_init();
    Motor_Init();

    sys.temperature = 25.0f;
    sys.humidity = 50.0f;
    sys.gas_level = 200.0f;
    sys.lux = 500.0f;
    sys.rainpercent = 0;
    sys.led_state = 1; // 0: off, 1: WHITE, 2: SOFT WHITE, 3: GREEN, 4: RED
    sys.fan = 0;       // 0: off, 1: low, 2: medium, 3: high
    sys.buzzer_on = 0; // 0: off, 1: on
    sys.awnings = 0;
    sys.door = 0;
    sys.mode = 0; // 0: auto, 1: manual

    uint8_t data[20];
    uint8_t m = (uint8_t)AUTO_MODE;
    uint8_t length = Create_Message(NOTIFY, MODE, 1, &m, data);
    USART1_Send_Data(data, length);
}

void DeviceManager_UpdateData(void)
{
    uint8_t data[20];
    uint8_t length;



    // ======= DHT11 =======
    uint8_t h, h_d, t, t_d;
    if (DHT11_Read(&h, &h_d, &t, &t_d))
    {

        float humidity = h + h_d / 10.0f;
        float temperature = t + t_d / 10.0f;

        if (humidity != sys.humidity)
        {
            sys.humidity = humidity;
            uint8_t *value = Convert_Float_To_Bytes(sys.humidity);
            length = Create_Message(NOTIFY, HUMI, 4, value, data);
            USART1_Send_Data(data, length);
        }

        if (temperature != sys.temperature)
        {
            sys.temperature = temperature;
            uint8_t *value = Convert_Float_To_Bytes(sys.temperature);
            length = Create_Message(NOTIFY, TEMP, 4, value, data);
            USART1_Send_Data(data, length);
        }
    }

    // ======= LDR =======
    float lux = CDS_ReadLux();
    if (lux != sys.lux)
    {
        sys.lux = lux;
        uint8_t *value = Convert_Float_To_Bytes(sys.lux);
        length = Create_Message(NOTIFY, LDR, 4, value, data);
        USART1_Send_Data(data, length);
    }

    // ======= MQ2 =======
    float gas = MQ2_ReadLevel();
    if (gas != sys.gas_level)
    {
        sys.gas_level = gas;
        uint8_t *value = Convert_Float_To_Bytes(sys.gas_level);
        length = Create_Message(NOTIFY, MQ2, 4, value, data);
        USART1_Send_Data(data, length);
    }

    uint8_t rain = rain_getpercent();
    if (gas != sys.gas_level)
    {
        sys.rainpercent = rain;
        length = Create_Message(NOTIFY, RAIN, 1, &sys.rainpercent, data);
        USART1_Send_Data(data, length);
    }

    // ======= actuator =======
    uint8_t led_state = LED_RGB_GetState();
    if (led_state != sys.led_state)
    {
        sys.led_state = led_state;

        length = Create_Message(NOTIFY, LED, 1, &sys.led_state, data);
        USART1_Send_Data(data, length);
    }

    uint8_t motor_level = Motor_GetLevel();
    if (motor_level != sys.fan)
    {
        sys.fan = motor_level;
        length = Create_Message(NOTIFY, FAN, 1, &sys.fan, data);
        USART1_Send_Data(data, length);
    }

    uint8_t buzzer_state = Buzzer_GetState();
    if (buzzer_state != sys.buzzer_on)
    {
        sys.buzzer_on = buzzer_state;
        length = Create_Message(NOTIFY, BUZZER, 1, &sys.buzzer_on, data);
        USART1_Send_Data(data, length);
    }

    uint8_t door = door_getstate();
    if (door != sys.door)
    {
        sys.door = door;
        length = Create_Message(NOTIFY, DOOR, 1, &sys.door, data);
        USART1_Send_Data(data, length);
    }

//    uint8_t awnings = awnings_getstate();
//    if (awnings != sys.awnings)
//    {
//        sys.awnings = awnings;
//        length = Create_Message(NOTIFY, AWNINGS, 1, &sys.awnings, data);
//        USART1_Send_Data(data, length);
//    }

    process_servo_logic();
}
