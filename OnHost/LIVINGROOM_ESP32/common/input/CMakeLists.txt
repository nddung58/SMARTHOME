set(app_src input.c)
set(pri_req
    "driver"
    "log"
    "esp_common"
)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
)