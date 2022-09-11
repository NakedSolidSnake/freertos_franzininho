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

TaskHandle_t task2_handle = NULL;

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
    int8_t led_blue = driver_open ("led_blue");
    handle_error (led_blue, "LED Blue Error");

    int8_t led_yellow = driver_open ("led_yellow");
    handle_error (led_yellow, "LED Yellow Error");

    LED_t leds [2] = 
    {
        {.handle = led_blue, .state = false},
        {.handle = led_yellow, .state = false},
    };

    uint8_t index = 0;

    while (true)
    {
        xQueueSendToBack (queue, &leds[index], 0);
        vTaskDelay (1000 / portTICK_PERIOD_MS);
        leds [index].state = !leds[index].state;
        index ^= 0x01;
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
    queue = xQueueCreate (1, sizeof (LED_t));
    if (queue != NULL)
    {
        xTaskCreate (task_producer, "task producer", 2500, NULL, 2 , NULL);
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

