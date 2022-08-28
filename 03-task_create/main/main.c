#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver_operation.h"
#include "gpio_options.h"

void handle_error (int8_t handle, const char *error)
{
    if (handle < 0)
    {
        while (true)
        {
            printf ("%s\n", error);
            vTaskDelay(1000 / portTICK_PERIOD_MS);    
        }
    }
}

void task_count (void *args) 
{
    int count = 0;
    while (1)
    {        
        printf ("Count: %d.\n", count++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } 
}

void task_led (void *args)
{
    bool state = false;

    int8_t led_blue = driver_open ("led_blue");
    handle_error (led_blue, "LED blue error.");

    while (1)
    {
        
        printf ("LED blue State %s.\n", state ? "ON" : "OFF");
        driver_ioctl (led_blue, gpio_set_state, &state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        state = !state;
    }
}

void app_main(void)
{
    xTaskCreate (task_led, "task led", 2500, NULL, 1 , NULL);
    xTaskCreate (task_count, "task count", 2500, NULL, 1 , NULL);
}
