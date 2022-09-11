#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver_operation.h"
#include "gpio_options.h"

QueueHandle_t mailbox;

typedef struct 
{
    int8_t handle;
    bool state;
    TickType_t timestamp;
} LED_t;

void task_producer (void *args);
void task_consumer (void *args);

void update_mailbox (LED_t *led);
bool read_mailbox (LED_t *led);

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
    LED_t blue;
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    while (true)
    {
        update_mailbox (&blue);
        vTaskDelay (500 / portTICK_PERIOD_MS);
        blue.state = !blue.state;
    }
}

void task_consumer (void *args)
{
    LED_t led;
    
    while (true)
    {
        if (read_mailbox (&led) == true)
            driver_ioctl (led.handle, gpio_set_state, &led.state);    
    }
}

void app_main(void)
{
    mailbox = xQueueCreate (1, sizeof (LED_t));
    if (mailbox != NULL)
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

void update_mailbox (LED_t *led)
{
    led->timestamp = xTaskGetTickCount ();
    xQueueOverwrite (mailbox, led);
}

bool read_mailbox (LED_t *led)
{
    bool status = false;

    TickType_t last = led->timestamp;

    xQueuePeek (mailbox, led, portMAX_DELAY);

    if (led->timestamp > last)
        status = true;

    return status;
}