set(app_src web_config.c
            wifi_config.c)
set(pri_req
    "log"
    "freertos"
    "nvs_flash"
    "esp_wifi"
    "esp_event"
    "esp_hw_support"
    "esp_http_server"
    "wpa_supplicant"
)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
                    EMBED_TXTFILES "../../html/webserver.html"
)