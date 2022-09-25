#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver_operation.h"
#include "gpio_options.h"

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;


static LED_t blue;
QueueHandle_t queue;

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
    bool status = true;
    BaseType_t high_priority;
    xQueueSendToFrontFromISR (queue, &status, &high_priority);
}

void app_main(void)
{
    bool status;
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    int8_t button = driver_open ("button");
    handle_error (button, "Button Error");

    driver_ioctl (button, gpio_set_interrupt_handler, &handler);

    queue = xQueueCreate (1, sizeof (bool));

    if (queue == NULL)
    {
        while (true)
        {
            vTaskDelay (1000 / portTICK_PERIOD_MS);
        }
    }

    while (true)
    {
        // driver_ioctl (button, gpio_get_state, &status);

        if (xQueueReceive (queue, &status, portMAX_DELAY) == pdPASS) 
        {            
            driver_ioctl (blue.handle, gpio_set_state, &blue.state);
            blue.state = !blue.state;
        }
    }
}
