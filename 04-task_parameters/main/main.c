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

void task_message (void *args) 
{
    char *msg = (char *)args;
    while (1)
    {        
        printf ("Message: %s.\n", msg);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } 
}

void task_led (void *args)
{
    bool state = false;

    int8_t led_blue = *(int8_t *)args;

    while (1)
    {
        
        printf ("LED blue State %s.\n", state ? "ON" : "OFF");
        driver_ioctl (led_blue, gpio_set_state, &state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        state = !state;
    }
}

static int8_t handle;
static char *message = "Hello World";

void app_main(void)
{
    handle = driver_open ("led_blue");
    handle_error (handle, "LED blue error.");

    xTaskCreate (task_led, "task led", 2500, &handle, 1 , NULL);
    xTaskCreate (task_message, "task message", 2500, (void *)message, 1 , NULL);
}
