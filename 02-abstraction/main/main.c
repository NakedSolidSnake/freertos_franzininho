#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver_operation.h"
#include "gpio_options.h"


void app_main(void)
{
    bool state = false;

    int8_t led_blue = driver_open ("led_blue");
    if (led_blue < 0)
    {
        while (true)
        {
            printf ("LED blue error.\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);    
        }
    }

    printf ("LED blue initialized.\n");

    while (1) {
        
        printf ("LED blue State %s.\n", state ? "ON" : "OFF");
        driver_ioctl (led_blue, gpio_set_state, &state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        state = !state;
    }
}
