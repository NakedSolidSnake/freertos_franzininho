#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver_operation.h"
#include "gpio_options.h"

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

TaskHandle_t task2_handle = NULL;

void task_1 (void *args);
void task_2 (void *args);

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

void task_1 (void *args)
{
    bool state = false;
    uint8_t count = 0;
    int8_t led_blue = driver_open ("led_blue");
    handle_error (led_blue, "LED Blue Error");

    int8_t led_yellow = driver_open ("led_yellow");
    handle_error (led_yellow, "LED Yellow Error");

    LED_t led = 
    {
        .handle = led_yellow,
        .state = true
    };

    while (true)
    {
        driver_ioctl (led_blue, gpio_set_state, &state);
        vTaskDelay (1000/ portTICK_PERIOD_MS);
        state = !state;

        if (count ++ > 10)
        {
            count = 0;
            xTaskCreate (task_2, "task 2", 2500, &led, 3 , &task2_handle);
            led.state = !led.state;
        }
    }
}

void task_2 (void *args)
{
    LED_t *led = (LED_t *)args;
    driver_ioctl (led->handle, gpio_set_state, &led->state);
    vTaskDelete (task2_handle);
}

void app_main(void)
{
    xTaskCreate (task_1, "task 1", 2500, NULL, 2 , NULL);
}

