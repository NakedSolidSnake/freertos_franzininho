#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver_operation.h"
#include "gpio_options.h"

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

TimerHandle_t auto_reload;
TimerHandle_t one_shot;

static LED_t blue;
static LED_t yellow;

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

void handle_timer_error (BaseType_t status, const char *error)
{
    if (status != pdPASS)
    {
        while (true)
        {
            printf ("%s\n", error);
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}

void auto_reload_callback( TimerHandle_t xTimer )
{
    driver_ioctl (blue.handle, gpio_set_state, &blue.state);
    blue.state = !blue.state;
}

void one_shot_callback( TimerHandle_t xTimer )
{
    yellow.state = true;
    driver_ioctl (yellow.handle, gpio_set_state, &yellow.state);
}

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");

    auto_reload =  xTimerCreate
                  ("Auto Reload",
                   pdMS_TO_TICKS (500),
                   pdTRUE,
                   0,
                   auto_reload_callback );

    one_shot =  xTimerCreate
                  ("One Shot",
                   pdMS_TO_TICKS (5000),
                   pdFALSE,
                   0,
                   one_shot_callback );

    if (auto_reload != NULL && one_shot != NULL) 
    {
        BaseType_t status = xTimerStart (auto_reload, 0);
        handle_timer_error (status, "Timer not started");

        status = xTimerStart (one_shot, 0);
        handle_timer_error (status, "Timer not started");
    }

}
