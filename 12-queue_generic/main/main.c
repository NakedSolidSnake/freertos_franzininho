#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver_operation.h"
#include "gpio_options.h"

QueueHandle_t queue;

typedef enum 
{
    type_led, 
    type_string
} type_t;

typedef struct 
{
    type_t type;
    void *data;
} event_t;

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

#define LED_ON  "on"
#define LED_OFF "off"

void task_producer_led_type (void *args);
void task_producer_string_type (void *args);
void task_consumer (void *args);
void process_led_event (event_t *event);
void process_string_event (event_t *event, LED_t *led);

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

void task_producer_led_type (void *args)
{
    LED_t blue;
    event_t event = 
    {
        .type = type_led,
        .data = &blue
    };

    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    while (true)
    {
        xQueueSendToBack (queue, &event, 0);
        vTaskDelay (500 / portTICK_PERIOD_MS);
        blue.state = !blue.state;
    }
}

void task_producer_string_type (void *args)
{
    event_t event = 
    {
        .type = type_string
    };

    const char *commands[] = 
    {
        LED_OFF,
        LED_ON
    };

    uint8_t index = 0;

    while (true)
    {
        event.data = (void *)commands[index];
        xQueueSendToBack (queue, &event, 0);
        vTaskDelay (1000 / portTICK_PERIOD_MS);
        index ^= 0x01;
    }
}

void task_consumer (void *args)
{
    event_t event;
    LED_t yellow;
    BaseType_t status;

    const TickType_t timeout = pdMS_TO_TICKS (100);

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");
    
    while (true)
    {
        status = xQueueReceive (queue, &event, timeout);
        if (status == pdPASS)
        {
            if (event.type == type_led)
                process_led_event (&event);
            else if (event.type == type_string)
                process_string_event (&event, &yellow);
        }
    }
}

void app_main(void)
{
    queue = xQueueCreate (2, sizeof (event_t));
    if (queue != NULL)
    {
        xTaskCreate (task_producer_led_type, "task producer", 2500, NULL, 2 , NULL);
        xTaskCreate (task_producer_string_type, "task producer", 2500, NULL, 2 , NULL);
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

void process_led_event (event_t *event)
{
    LED_t *led = (LED_t *)event->data;
    driver_ioctl (led->handle, gpio_set_state, &led->state);
}

void process_string_event (event_t *event, LED_t *led)
{
    char *command = (char *)event->data;
    bool state;

    if (strcmp (command, LED_OFF) == 0)
    {
        state = false;
        driver_ioctl (led->handle, gpio_set_state, &state);
    }
    else if (strcmp (command, LED_ON) == 0)
    {
        state = true;
        driver_ioctl (led->handle, gpio_set_state, &state);
    }
}

    
