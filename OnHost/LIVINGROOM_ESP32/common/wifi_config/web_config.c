/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <esp_http_server.h>

#include "web_config.h"

extern const uint8_t webserver_html_start[] asm("_binary_webserver_html_start");
extern const uint8_t webserver_html_end[] asm("_binary_webserver_html_end");

static const char *TAG = "WEB CONFIG";
static httpd_handle_t server = NULL;
static http_post_handle_t http_post_cb = NULL;


static esp_err_t get_handler(httpd_req_t *req)
{
    const char *html_page = (const char *)webserver_html_start;
    size_t html_len = webserver_html_end - webserver_html_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, html_len);

    ESP_LOGI(TAG, "Sent config HTML page to client.");
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t *req)
{
    char buf[100];                                     // Bộ đệm chứa dữ liệu nhận được
    int data_len = req->content_len;              // Lấy độ dài dữ liệu từ request

    // Đọc dữ liệu từ client gửi đến
    httpd_req_recv(req, buf, data_len);

    ESP_LOGI(TAG, "Data recv: %.*s", data_len, buf);

    http_post_cb(buf, data_len);

    // Kết thúc phản hồi (gửi response rỗng)
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}



static const httpd_uri_t http_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t http_post = {
    .uri = "/post",
    .method = HTTP_POST,
    .handler = post_handler,
    .user_ctx = NULL,
};

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_start(&server, &config);

    httpd_register_uri_handler(server, &http_get);
    httpd_register_uri_handler(server, &http_post);
}

void stop_webserver(void)
{
    httpd_stop(server);
}

void http_post_set_callback(void *cb)
{
    if(cb){
        http_post_cb = cb;
    }
}