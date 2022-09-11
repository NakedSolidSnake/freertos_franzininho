#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver_operation.h"
#include "gpio_options.h"

QueueHandle_t queue_led;
QueueHandle_t queue_string;

QueueSetHandle_t queue_set;

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

typedef struct 
{
    char *command;
} command_t;

#define LED_ON  "on"
#define LED_OFF "off"

void task_producer_led_type (void *args);
void task_producer_string_type (void *args);
void task_consumer (void *args);
void process_queue_led (void);
void process_queue_string (LED_t *led);

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

    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    while (true)
    {
        xQueueSendToBack (queue_led, &blue, 0);
        vTaskDelay (500 / portTICK_PERIOD_MS);
        blue.state = !blue.state;
    }
}

void task_producer_string_type (void *args)
{
    command_t command;
    const char *commands[] = 
    {
        LED_OFF,
        LED_ON
    };

    uint8_t index = 0;

    while (true)
    {
        command.command = (char *)commands[index];
        xQueueSendToBack (queue_string, &command, 0);
        vTaskDelay (1000 / portTICK_PERIOD_MS);
        index ^= 0x01;
    }
}

void task_consumer (void *args)
{
    LED_t yellow;
    const TickType_t timeout = pdMS_TO_TICKS (100);
    QueueSetMemberHandle_t queue_handle;

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");
    
    while (true)
    {
        queue_handle = xQueueSelectFromSet (queue_set, timeout);
        
        if (queue_handle == queue_led)
            process_queue_led ();

        else if (queue_handle == queue_string)
            process_queue_string (&yellow);
    }
}

void app_main(void)
{
    queue_led = xQueueCreate (1, sizeof (LED_t));
    queue_string = xQueueCreate (1, sizeof (command_t));

    queue_set = xQueueCreateSet (2);

    if (queue_led != NULL && queue_string != NULL && queue_set)
    {
        xQueueAddToSet (queue_led, queue_set);
        xQueueAddToSet (queue_string, queue_set);

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

void process_queue_led (void)
{
    LED_t led;
    xQueueReceive (queue_led, &led, 0);
    driver_ioctl (led.handle, gpio_set_state, &led.state);
}

void process_queue_string (LED_t *led)
{
    command_t command;
    bool state;

    xQueueReceive (queue_string, &command, 0);

    if (strcmp (command.command, LED_OFF) == 0)
    {
        state = false;
        driver_ioctl (led->handle, gpio_set_state, &state);
    }
    else if (strcmp (command.command, LED_ON) == 0)
    {
        state = true;
        driver_ioctl (led->handle, gpio_set_state, &state);
    }
}

    
