#ifndef GPIO_OPTIONS_H_
#define GPIO_OPTIONS_H_

typedef enum 
{
    gpio_set_state,     /* bool -> true = on, false = off*/
    gpio_get_state,     /* bool -> true = not pressed, false = pressed*/
    gpio_set_interrupt_handler /* void (*handler) (void *args) */
} gpio_options_t;

#endif /* GPIO_OPTIONS_H_ */
