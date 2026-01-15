#include "wifi_config.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "web_config.h"

static const char *TAG = "CONFIG_WIFI";

// provision_type_t provision_type = PROVISION_SMARTCONFIG; // smart config ok rồi
provision_type_t provision_type = PROVISION_ACCESSPOINT;

#define WIFI_SSID "DUNGX_ESP32" // tên wifi của router
#define WIFI_PASSWORD "12345678"

static EventGroupHandle_t s_wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int HTTP_CONFIG_DONE = BIT2;

static int s_retry_count = 0;
#define MAX_RETRY_COUNT 10

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_count < MAX_RETRY_COUNT)
        {
            s_retry_count++;
            ESP_LOGI(TAG, "Retrying to connect to the AP... (%d/%d)", s_retry_count, MAX_RETRY_COUNT);
            esp_wifi_connect();
        }
        else
        {
            ESP_LOGW(TAG, "Exceeded max retry count. Re-entering provisioning mode...");

            wifi_config_t wifiConfig = {0};
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));

            // Tắt Wi-Fi trước khi reset lại
            esp_wifi_stop();
            esp_wifi_deinit();
            // Gọi lại cấu hình từ đầu
            wifi_config();
        }

        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station %02x:%02x:%02x:%02x:%02x:%02x join, AID=%u",
                 event->mac[0], event->mac[1], event->mac[2],
                 event->mac[3], event->mac[4], event->mac[5],
                 event->aid);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station %02x:%02x:%02x:%02x:%02x:%02x leave, AID=%lu",
                 event->mac[0], event->mac[1], event->mac[2],
                 event->mac[3], event->mac[4], event->mac[5],
                 (unsigned long)event->aid);
    }

    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG, "Scan done");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(TAG, "Found channel");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
        {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", (char *)ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", (char *)password);

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static esp_netif_t *wifi_sta_netif = NULL;
static esp_netif_t *wifi_ap_netif = NULL;
bool is_provisioned(void)
{
    bool provisioned = false;
    if (wifi_sta_netif == NULL)
    {
        wifi_sta_netif = esp_netif_create_default_wifi_sta();
    }

    if (wifi_ap_netif == NULL)
    {
        wifi_ap_netif = esp_netif_create_default_wifi_ap();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_STA, &wifi_config); // get wifi

    if (wifi_config.sta.ssid[0] != 0x00) // kiểm tra xem có wifi hay k
    {
        ESP_LOGI(TAG, "SSID: %s", wifi_config.sta.ssid);
        ESP_LOGI(TAG, "Password: %s", wifi_config.sta.password);
        provisioned = true;
    }

    return provisioned;
}
static void ap_start(void)
{
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 1,
            .password = WIFI_PASSWORD,
            .max_connection = 4, // được 4 đứa kết nối vào
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (wifi_config.ap.password[0] == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
char ssid[33] = {0};
char password[65] = {0};
void http_post_data_callback(char *buf, int len)
{
    // ssid/pass
    printf("%s\n", buf);
    char *pt = strtok(buf, "/");
    strcpy(ssid, pt);
    printf("ssid: %s\n", pt);
    pt = strtok(NULL, "/");
    strcpy(password, pt);
    printf("pass: %s\n", pt);
    xEventGroupSetBits(s_wifi_event_group, HTTP_CONFIG_DONE);
}

static bool is_event_registered = false;
void wifi_config(void)
{
    if (!is_event_registered)
    {
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        is_event_registered = true;
    }

    s_wifi_event_group = xEventGroupCreate();
    bool provisioned = is_provisioned();
    ESP_LOGI(TAG, "Provisioned: %s", provisioned ? "true" : "false");
    if (!provisioned)
    {
        if (provision_type == PROVISION_SMARTCONFIG)
        {
            ESP_ERROR_CHECK(esp_wifi_start());
            ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
            smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
            xEventGroupWaitBits(s_wifi_event_group, ESPTOUCH_DONE_BIT, false, true, portMAX_DELAY);
            esp_smartconfig_stop();
        }
        else if (provision_type == PROVISION_ACCESSPOINT)
        {
            ap_start();
            start_webserver();
            http_post_set_callback(http_post_data_callback);
            xEventGroupWaitBits(s_wifi_event_group, HTTP_CONFIG_DONE, false, true, portMAX_DELAY);

            // convert station mode and connect router chuyển
            stop_webserver();
            ESP_ERROR_CHECK(esp_wifi_stop());   // Tắt Wi-Fi AP mode
            ESP_ERROR_CHECK(esp_wifi_deinit()); // Gỡ cấu hình cũ

            vTaskDelay(pdMS_TO_TICKS(500)); // Delay nhỏ cho chắc

            wifi_config_t wifi_config;
            bzero(&wifi_config, sizeof(wifi_config_t));
            strncpy((char *)wifi_config.sta.ssid, "iphone13", sizeof(wifi_config.sta.ssid));
            strncpy((char *)wifi_config.sta.password, "68686866", sizeof(wifi_config.sta.password));

            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            ESP_ERROR_CHECK(esp_wifi_start());
        }
    }
    else
    {
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "wifi connected");
}
