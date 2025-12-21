set(app_src convert.c)
set(pri_req
)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${pri_req}
                    REQUIRES ${pri_req}
)