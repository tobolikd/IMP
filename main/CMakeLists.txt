set(project_srcs "IMP.c" "i2c-utils.c" "ssd1306.c" "utils.c")

idf_component_register(SRCS "${project_srcs}"
                    PRIV_REQUIRES driver
                    INCLUDE_DIRS ".")
