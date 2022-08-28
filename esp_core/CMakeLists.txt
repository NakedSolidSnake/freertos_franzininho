set (srcs "bsp/bsp.c" "drivers/driver_gpio.c" "framework/driver_operation.c")

idf_component_register (SRCS "${srcs}"
                        INCLUDE_DIRS "descriptors" "drivers" "framework"
                        REQUIRES "driver")