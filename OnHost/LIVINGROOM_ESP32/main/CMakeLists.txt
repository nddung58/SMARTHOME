idf_component_register(
    SRCS "app_main.c" 
    "led.c" "ldr.c" "buzzer.c" "mq2.c" "dht11.c" "rain.c" "door.c" "sg90.c" "rfid.c" "app_com_uart.c"
    "fsm_message.c" "message.c" "queue.c" "living_handle.c" "adc_shared.c"
    INCLUDE_DIRS "."
)


