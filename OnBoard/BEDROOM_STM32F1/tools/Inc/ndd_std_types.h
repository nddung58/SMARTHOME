#ifndef NDD_STD_TYPES_H_
#define NDD_STD_TYPES_H_

typedef enum
{
    ON_HOST_LIVING_ROOM,

    // on board
    ON_BOARD_BED_ROOM,

} nodes_t;

typedef enum
{
    // Sensor
    LDR = 0x01,
    PIR = 0x02,
    MQ2 = 0x03,
    HUMI = 0x04,
    TEMP = 0x05,
    RAIN = 0x06,

    // Actuator
    LED = 0x11,
    FAN = 0x12,
    BUZZER = 0x13,
    AWNINGS = 0x14,
    DOOR = 0x15,

    // system
    MODE = 0x20,
    UNKNOWN = 0xFF,

} id_end_device_t;

typedef enum
{
    AUTO_MODE = 0,
    MANUAL_MODE = 1
} mode_t;

#endif