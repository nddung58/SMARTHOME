set(app_src output.c)
set(pri_req
    "driver"
    "log"
)
    
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
)