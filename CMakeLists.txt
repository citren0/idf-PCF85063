
idf_component_register(
    SRCS "pcf85063.c"
    REQUIRES driver
    PRIV_REQUIRES i2c-lock
    INCLUDE_DIRS "include"
)