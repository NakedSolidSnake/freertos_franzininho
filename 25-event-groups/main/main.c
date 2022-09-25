#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "driver_operation.h"
#include "gpio_options.h"

#define LED_BLUE_BIT    (1UL << 0UL)
#define LED_YELLOW_BIT    (1UL << 1UL)

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

static LED_t blue;
static LED_t yellow;

EventGroupHandle_t event_group;
TimerHandle_t auto_reload;

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

void auto_reload_callback (TimerHandle_t timer)
{
    xEventGroupSetBits (event_group, LED_BLUE_BIT);
}

void handler (void *args)
{
    BaseType_t high_priority;
    xEventGroupSetBitsFromISR (event_group, LED_YELLOW_BIT, &high_priority);
}

void task_led (void *args)
{
    LED_t *led = NULL;
    EventBits_t bits_to_wait = (LED_BLUE_BIT | LED_YELLOW_BIT);
    EventBits_t bits_arrived;

    while (true)
    {
        bits_arrived = xEventGroupWaitBits (event_group,
                                            bits_to_wait,
                                            pdTRUE,
                                            pdFALSE,
                                            portMAX_DELAY);

        if (bits_arrived & LED_BLUE_BIT)
            led = &blue;
        else if (bits_arrived & LED_YELLOW_BIT)
            led = &yellow;

        driver_ioctl (led->handle, gpio_set_state, &led->state);
        led->state = !led->state;
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

    auto_reload = xTimerCreate ("Auto Reload", pdMS_TO_TICKS (500), pdTRUE, 0, auto_reload_callback);

    if (event_group != NULL && auto_reload != NULL)
    {
        xTimerStart (auto_reload, 0);
        xTaskCreate (task_led, "blue", 2500, NULL, 2, NULL);        
    }
    else 
    {
        while (true)
        {
            printf ("Error: Event Group or Timer not created\n");
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}
