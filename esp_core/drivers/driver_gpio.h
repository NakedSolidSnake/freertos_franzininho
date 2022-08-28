#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_

#include "driver_base.h"

bool driver_gpio_init (const void *object);
int8_t driver_gpio_open (const void *object);
int32_t driver_gpio_write (const void *object, const void *buffer, uint32_t size);
int32_t driver_gpio_read (const void *object, void *buffer, uint32_t size);
bool driver_gpio_ioctl (const void *object, uint8_t command, void *args);
bool driver_gpio_close (const void *object);

const driver_base_t driver_gpio = 
{
    .init = driver_gpio_init,
    .open = driver_gpio_open,
    .write = driver_gpio_write,
    .read = driver_gpio_read,
    .ioctl = driver_gpio_ioctl,
    .close = driver_gpio_close
};

#endif /* DRIVER_GPIO_H_ */
