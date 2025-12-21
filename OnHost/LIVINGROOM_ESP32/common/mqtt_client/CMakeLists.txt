set(app_src nddmqtt_client.c)
set(pri_req
    "esp_wifi"
    "log"
    "nvs_flash"
    "esp_event"
    "esp_system"
    "mqtt"
)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
                    # EMBED_TXTFILES
                    #     "../../certs/ca.crt"
                    #     "../../certs/client.crt"
                    #     "../../certs/client.key"
)