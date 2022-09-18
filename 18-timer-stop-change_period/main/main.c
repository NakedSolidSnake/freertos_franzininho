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

TimerHandle_t timer_1;
TimerHandle_t timer_2;

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

void auto_reload_callback( TimerHandle_t timer )
{
    uint32_t count = (uint32_t) pvTimerGetTimerID (timer);

    if (timer == timer_1)
    {
        if (count == 10)
            xTimerChangePeriod( timer, pdMS_TO_TICKS (200), 0);
        
        else if (count == 50)
        {
            xTimerChangePeriod( timer, pdMS_TO_TICKS (500), 0);
            count = 0;
        }

        driver_ioctl (blue.handle, gpio_set_state, &blue.state);
        blue.state = !blue.state; 
        count ++;   
    }

    else if (timer == timer_2)
    {

        if (count > 20)
            xTimerStop (timer, 0);

        driver_ioctl (yellow.handle, gpio_set_state, &yellow.state);
        yellow.state = !yellow.state; 
        count ++;
    }

    

    vTimerSetTimerID( timer, (void *)count);
}

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    yellow.handle = driver_open ("led_yellow");
    handle_error (yellow.handle, "LED Yellow Error");

    timer_1 =  xTimerCreate
                  ("Auto Reload",
                   pdMS_TO_TICKS (500),
                   pdTRUE,
                   0,
                   auto_reload_callback );

    timer_2 =  xTimerCreate
                  ("Auto Reload",
                   pdMS_TO_TICKS (500),
                   pdTRUE,
                   0,
                   auto_reload_callback );

    if (timer_1 != NULL && timer_2) 
    {
        BaseType_t status = xTimerStart (timer_1, 0);
        handle_timer_error (status, "Timer not started");

        status = xTimerStart (timer_2, 0);
        handle_timer_error (status, "Timer not started");
    }

}
