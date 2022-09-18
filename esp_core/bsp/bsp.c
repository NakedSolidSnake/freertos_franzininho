#include "driver_operation.h"
#include "driver_gpio.h"
#include "descriptor_gpio.h"

const gpio_descriptor_t led_blue = 
{
    .pin = 21,
    .status = false,
    .direction = gpio_output,
    .pull = gpio_pull_off,
    .interrupt = gpio_interrupt_disable
} ;

const gpio_descriptor_t led_yellow = 
{
    .pin = 33,
    .status = false,
    .direction = gpio_output,
    .pull = gpio_pull_off,
    .interrupt = gpio_interrupt_disable
} ;

// const gpio_descriptor_t button = 
// {
//     .pin = 0,
//     .status = false,
//     .direction = gpio_input,
//     .pull = gpio_pull_up,
//     .interrupt = gpio_interrupt_disable
// };

const gpio_descriptor_t button = 
{
    .pin = 0,
    .status = false,
    .direction = gpio_input,
    .pull = gpio_pull_up,
    .interrupt = gpio_interrupt_rising_edge
};

static driver_operation_t _drivers [] = 
{
    {.name = "led_blue", .is_open = false, .descriptor = &led_blue, .driver = &driver_gpio},
    {.name = "led_yellow", .is_open = false, .descriptor = &led_yellow, .driver = &driver_gpio},
    {.name = "button", .is_open = false, .descriptor = &button, .driver = &driver_gpio},
};

uint8_t drivers_amount = sizeof (_drivers) / sizeof (_drivers [0]);
driver_operation_t *drivers = _drivers;