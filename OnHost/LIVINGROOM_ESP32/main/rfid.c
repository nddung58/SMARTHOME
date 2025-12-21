#include "rfid.h"
#include <esp_log.h>
#include "rc522.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"
#include <string.h>

static const char *TAG = "RFID";

#define RC522_SPI_MISO 25
#define RC522_SPI_MOSI 23
#define RC522_SPI_SCLK 19
#define RC522_SPI_CS 22
#define RC522_RST -1

#define MAX_CARD 20

uint8_t uid_default[4] = {0x23, 0x9B, 0x85, 0x2D};

rfid_card_list_t valid_cards = {0};

static bool add_mode = false;

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

rfid_handle_t rfid_handle;

static bool add_card(rc522_picc_uid_t uid)
{
    if (valid_cards.count >= MAX_CARD)
    {
        ESP_LOGW(TAG, "Card list full!");
        return false;
    }

    valid_cards.cards[valid_cards.count++] = uid;
    ESP_LOGI(TAG, "Added new card");
    return true;
}

static bool rfid_uid_equal(const rc522_picc_uid_t *a, const rc522_picc_uid_t *b)
{
    if (a->length != b->length)
        return false;

    return memcmp(a->value, b->value, (size_t)a->length) == 0;
}

static bool rfid_is_valid(const rc522_picc_uid_t *uid)
{
    if (uid->length == 4 && memcmp(uid->value, uid_default, (size_t)uid->length) == 0)
    {
        return true;
    }
    for (int i = 0; i < valid_cards.count; i++)
    {
        if (rfid_uid_equal(uid, &valid_cards.cards[i]))
        {
            return true;
        }
    }

    return false;
}

static void on_picc_state_changed(
    void *arg,
    esp_event_base_t base,
    int32_t event_id,
    void *data)
{
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE)
    {
        rc522_picc_print(picc);

        rc522_picc_uid_t uid = picc->uid;

        if (add_mode)
        {
            ESP_LOGI(TAG, "Learning mode: Adding card...");
            add_card(uid);
            add_mode = false;
            ESP_LOGI(TAG, "Learning mode OFF");
            return;
        }

        if (rfid_is_valid(&uid))
        {
            ESP_LOGI(TAG, "ACCESS GRANTED!");
            // TODO: mở cửa, bật relay, gửi MQTT...
            rfid_handle(ACCESS_GRANTED);
        }
        else
        {
            ESP_LOGW(TAG, "ACCESS DENIED!");
            rfid_handle(ACCESS_DENIED);
        }
    }
    else if (picc->state == RC522_PICC_STATE_IDLE &&
             event->old_state >= RC522_PICC_STATE_ACTIVE)
    {
        ESP_LOGI(TAG, "Card removed");
        rfid_handle(CARD_REMOVED);
    }
}

void rfid_init(void)
{
    ESP_LOGI(TAG, "Initializing RC522...");

    rc522_spi_config_t cfg = {
        .host_id = SPI3_HOST,
        .bus_config = &(spi_bus_config_t){
            .mosi_io_num = RC522_SPI_MOSI,
            .miso_io_num = RC522_SPI_MISO,
            .sclk_io_num = RC522_SPI_SCLK,
        },
        .dev_config = {
            .spics_io_num = RC522_SPI_CS,
        },
        .rst_io_num = RC522_RST,
    };

    rc522_spi_create(&cfg, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_cfg = {
        .driver = driver,
    };

    rc522_create(&scanner_cfg, &scanner);

    rc522_register_events(scanner,
                          RC522_EVENT_PICC_STATE_CHANGED,
                          on_picc_state_changed,
                          NULL);

    rc522_start(scanner);

    ESP_LOGI(TAG, "RC522 READY.");
}

void rfid_set_callback(void *cb)
{
    if (cb)
    {
        rfid_handle = cb;
    }
}