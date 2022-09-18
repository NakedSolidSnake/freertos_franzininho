#include <stdint.h>
#include "driver/gpio.h"
#include "descriptor_gpio.h"

#define ESP_INTR_FLAG_DEFAULT   0

typedef void (*driver_gpio_ioctl_t) (const void *object, void *args);
typedef void (*gpio_isr_handler_t) (void *args);

typedef struct 
{
    bool status;
    uint8_t size;
    driver_gpio_ioctl_t *ioctl_list;
} driver_gpio_context_t;

static void driver_gpio_set_state (const void *object, void *args);
static void driver_gpio_get_state (const void *object, void *args);
static void driver_gpio_set_interrupt_handler (const void *object, void *args);

static driver_gpio_ioctl_t entry [] = 
{
    driver_gpio_set_state,
    driver_gpio_get_state,
    driver_gpio_set_interrupt_handler
};

static driver_gpio_context_t context = 
{
    .status = false,
    .size = sizeof (entry) / sizeof (entry[0]),
    .ioctl_list = entry
};

bool driver_gpio_init (const void *object)
{
    (void)object;
    return false;
}

int8_t driver_gpio_open (const void *object)
{
    gpio_descriptor_t *gpio = (gpio_descriptor_t *)object;

    gpio_config_t io_conf;
    io_conf.mode = gpio->direction == gpio_output ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << gpio->pin);
    io_conf.intr_type = gpio->interrupt;
    io_conf.pull_down_en = (gpio->pull == gpio_pull_down);
    io_conf.pull_up_en = (gpio->pull == gpio_pull_up);
    gpio_config (&io_conf);

    if (gpio->interrupt)
    {
        gpio_set_intr_type (gpio->pin, gpio->interrupt);
        gpio_install_isr_service (ESP_INTR_FLAG_DEFAULT);
    }

    return true;
}

int32_t driver_gpio_write (const void *object, const void *buffer, uint32_t size)
{
    (void) object;
    (void) buffer;
    (void) size;
    return -1;
}

int32_t driver_gpio_read (const void *object, void *buffer, uint32_t size)
{
    (void) object;
    (void) buffer;
    (void) size;
    return -1;
}

bool driver_gpio_ioctl (const void *object, uint8_t command, void *args)
{
    bool status = false;

    if (command < context.size)
    {
        (*entry[command]) (object, args);
        status = true;
    }

    return status;
}

bool driver_gpio_close (const void *object)
{
    return false;
}

static void driver_gpio_set_state (const void *object, void *args)
{
    gpio_descriptor_t *gpio = (gpio_descriptor_t *) object;
    bool *state = (bool *)args;

    gpio_set_level (gpio->pin, *state);
}

static void driver_gpio_get_state (const void *object, void *args)
{
    gpio_descriptor_t *gpio = (gpio_descriptor_t *) object;
    bool *state = (bool *)args;

    *state = gpio_get_level (gpio->pin);
}

static void driver_gpio_set_interrupt_handler (const void *object, void *args)
{
    gpio_descriptor_t *gpio = (gpio_descriptor_t *) object;

    gpio_isr_handler_t handler = (gpio_isr_handler_t)(args);

    gpio_isr_handler_add (gpio->pin, handler, (void *)&gpio->pin);
}