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

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    auto_reload =  xTimerCreate
                  ("Auto Reload",
                   pdMS_TO_TICKS (500),
                   pdTRUE,
                   0,
                   auto_reload_callback );

    if (auto_reload != NULL) 
    {
        BaseType_t status = xTimerStart (auto_reload, 0);
        handle_timer_error (status, "Timer not started");
    }

}
