#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_LED_21 21

#define GPIO_LED_21_PIN_SEL (1UL << GPIO_LED_21)

void init_gpio (void)
{
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_LED_21_PIN_SEL;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config (&io_conf);
}

void app_main(void)
{
    bool state = false;

    init_gpio ();

    while (1) {
        printf ("Hello World.\n");
        gpio_set_level (GPIO_LED_21, state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        state = !state;
    }
}
