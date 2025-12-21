#ifndef MAIN_RFID_H__
#define MAIN_RFID_H__

#include <stdint.h>
#include "esp_err.h"
#include "rc522_picc.h"

typedef enum
{
    ACCESS_GRANTED,
    ACCESS_DENIED,
    CARD_REMOVED
} rfid_state_t;

#define MAX_CARD 20

typedef struct
{
    rc522_picc_uid_t cards[MAX_CARD];
    int count;
} rfid_card_list_t;

typedef void (*rfid_handle_t)(uint8_t event);

void rfid_init(void);
void rfid_addcard_start(void);
void rfid_set_callback(void *cb);

#endif // MAIN_RFID_H_
