#ifndef DESCRIPTOR_GPIO_H_
#define DESCRIPTOR_GPIO_H_

#include <stdbool.h>

typedef enum 
{
    gpio_output,
    gpio_input
} descriptor_gpio_mode_t;

typedef struct 
{
    uint8_t pin;
    bool status;
    descriptor_gpio_mode_t mode;
} descriptor_gpio_t;

#endif /* DESCRIPTOR_GPIO_H_ */
