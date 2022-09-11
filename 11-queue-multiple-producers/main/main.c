#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver_operation.h"
#include "gpio_options.h"

QueueHandle_t queue;

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

void task_producer (void *args);
void task_consumer (void *args);

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

void task_producer (void *args)
{
    LED_t *led = (LED_t *)args;

    while (true)
    {
        xQueueSendToBack (queue, led, 0);
        vTaskDelay (1000 / portTICK_PERIOD_MS);
        led->state = !led->state;
    }
}

void task_consumer (void *args)
{
    LED_t led;
    BaseType_t status;

    const TickType_t timeout = pdMS_TO_TICKS (100);
    
    while (true)
    {
        status = xQueueReceive (queue, &led, timeout);
        if (status == pdPASS)
            driver_ioctl (led.handle, gpio_set_state, &led.state);    
    }
}

void app_main(void)
{
    static LED_t blue;
    static LED_t yellow;

    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");

    queue = xQueueCreate (2, sizeof (LED_t));
    if (queue != NULL)
    {
        xTaskCreate (task_producer, "task producer", 2500, &blue, 2 , NULL);
        xTaskCreate (task_producer, "task producer", 2500, &yellow, 2 , NULL);
        xTaskCreate (task_consumer, "task consumer", 2500, NULL, 2 , NULL);
    }
    else 
    {
        while (true)
        {
            printf ("Error: Queue not initialized.\n");
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}

