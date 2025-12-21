#ifndef APP_LED_H_
#define APP_LED_H_

#include <stdint.h>

typedef enum
{
    LED_OFF = 0,
    LED_WHITE_50 = 1,
    LED_WHITE_100 = 2,
    LED_WARM = 3,
    LED_RED = 4,
    LED_UNKNOWN = 5
} LED_State_t;

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB_Color_t;

void led_init(void);

void led_set(LED_State_t state);

LED_State_t led_getstatus(void);

#endif // APP_LED_H_