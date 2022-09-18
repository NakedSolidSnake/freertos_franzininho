#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver_operation.h"
#include "gpio_options.h"

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

bool status = false;
static LED_t blue;

void handle_error (int8_t handle, const char *error)
{
    if (handle < 0)
    {
        while (true)
        {
            printf ("%s\n", error);
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}

void handler (void *args)
{
    status = true;
}

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    int8_t button = driver_open ("button");
    handle_error (button, "Button Error");

    driver_ioctl (button, gpio_set_interrupt_handler, &handler);

    while (true)
    {
        // driver_ioctl (button, gpio_get_state, &status);

        if (status) 
        {            
            blue.state = true; 
            driver_ioctl (blue.handle, gpio_set_state, &blue.state);
            status = false;
            vTaskDelay (500/ portTICK_PERIOD_MS);
        }

        driver_ioctl (blue.handle, gpio_set_state, &status);
        
    }
}
