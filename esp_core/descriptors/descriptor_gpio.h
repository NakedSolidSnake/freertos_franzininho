#ifndef DESCRIPTOR_GPIO_H_
#define DESCRIPTOR_GPIO_H_

#include <stdbool.h>

typedef void (*gpio_interrupt_handler) (void *args);

typedef enum 
{
    gpio_output,
    gpio_input
} gpio_direction_t;

typedef enum 
{
    gpio_pull_off,
    gpio_pull_up,
    gpio_pull_down
} gpio_pull_type_t;

typedef enum 
{
    gpio_interrupt_disable,
    gpio_interrupt_rising_edge,
    gpio_interrupt_falling_edge,
    gpio_interrupt_both_edge,
    gpio_interrupt_low_level,
    gpio_interrupt_high_level
} gpio_interrupt_type_t;

typedef struct 
{
    uint8_t pin;
    bool status;
    gpio_direction_t direction;
    gpio_pull_type_t pull;
    gpio_interrupt_type_t interrupt;
} gpio_descriptor_t;

#endif /* DESCRIPTOR_GPIO_H_ */
