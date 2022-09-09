#include "driver_operation.h"
#include "driver_gpio.h"
#include "descriptor_gpio.h"

const descriptor_gpio_t led_blue = 
{
    .pin = 21,
    .status = false,
    .mode = gpio_output
} ;

const descriptor_gpio_t led_yellow = 
{
    .pin = 33,
    .status = false,
    .mode = gpio_output
} ;

static driver_operation_t _drivers [] = 
{
    {.name = "led_blue", .is_open = false, .descriptor = &led_blue, .driver = &driver_gpio},
    {.name = "led_yellow", .is_open = false, .descriptor = &led_yellow, .driver = &driver_gpio},
};

uint8_t drivers_amount = sizeof (_drivers) / sizeof (_drivers [0]);
driver_operation_t *drivers = _drivers;