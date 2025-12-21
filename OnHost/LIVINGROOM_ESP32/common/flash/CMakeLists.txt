set(app_src storage_nvs.c)
set(pri_req
    "log"
    "esp_common"
    "nvs_flash"
)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
)