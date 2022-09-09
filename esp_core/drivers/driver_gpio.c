#include <stdint.h>
#include "driver/gpio.h"
#include "descriptor_gpio.h"

typedef void (*driver_gpio_ioctl_t) (const void *object, void *args);

typedef struct 
{
    bool status;
    uint8_t size;
    driver_gpio_ioctl_t *ioctl_list;
} driver_gpio_context_t;

static void driver_gpio_set_state (const void *object, void *args);

static driver_gpio_ioctl_t entry [] = 
{
    driver_gpio_set_state
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
    descriptor_gpio_t *gpio = (descriptor_gpio_t *)object;

    gpio_config_t io_conf;
    io_conf.mode = gpio->mode == gpio_output ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << gpio->pin);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config (&io_conf);
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
    descriptor_gpio_t *gpio = (descriptor_gpio_t *) object;
    bool *state = (bool *)args;

    gpio_set_level (gpio->pin, *state);
}