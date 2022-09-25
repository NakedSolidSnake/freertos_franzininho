#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver_operation.h"
#include "gpio_options.h"

#define LED_BLUE_BIT        (1UL << 0UL)
#define LED_YELLOW_BIT      (1UL << 1UL)
#define PRINT_BIT           (1UL << 2UL)

#define ALL_BITS_SYNC       (LED_BLUE_BIT | LED_YELLOW_BIT| PRINT_BIT)

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

static LED_t blue;
static LED_t yellow;

EventGroupHandle_t event_group;

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
    BaseType_t high_priority;
    xEventGroupSetBitsFromISR (event_group, ALL_BITS_SYNC ,&high_priority);    
}

void task_blue (void *args)
{
    while (true)
    {
        xEventGroupWaitBits (event_group,
                             LED_BLUE_BIT,
                             pdTRUE,
                             pdTRUE,
                             portMAX_DELAY);

        driver_ioctl (blue.handle, gpio_set_state, &blue.state);
        blue.state = !blue.state;
        vTaskDelay (500/ portTICK_PERIOD_MS);
    }
}

void task_yellow (void *args)
{

    while (true)
    {
        xEventGroupWaitBits (event_group,
                             LED_YELLOW_BIT,
                             pdTRUE,
                             pdTRUE,
                             portMAX_DELAY);

        driver_ioctl (yellow.handle, gpio_set_state, &yellow.state);
        yellow.state = !yellow.state;
        vTaskDelay (500/ portTICK_PERIOD_MS);
    }
}

void task_print (void *args)
{
    char *message = (char *)args;

    while (true)
    {
        xEventGroupWaitBits (event_group,
                             PRINT_BIT,
                             pdTRUE,
                             pdTRUE,
                             portMAX_DELAY);

        printf ("Received a token Yeah: %s\n", message);
        vTaskDelay (100/ portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");

    int8_t button = driver_open ("button");
    handle_error (button, "Button Error");

    driver_ioctl (button, gpio_set_interrupt_handler, &handler);

    event_group = xEventGroupCreate ();

    if (event_group != NULL)
    {
        xTaskCreate (task_blue, "blue", 2500, NULL, 2, NULL);
        xTaskCreate (task_yellow, "yellow", 2500, NULL, 2, NULL);
        xTaskCreate (task_print, "print", 2500, "Hello", 2, NULL);
    }
    else 
    {
        while (true)
        {
            printf ("Error: Event Group not created\n");
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}
