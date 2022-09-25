#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver_operation.h"
#include "gpio_options.h"

typedef struct 
{
    int8_t handle;
    bool state;
} LED_t;

static LED_t blue;
SemaphoreHandle_t semaphore;

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
    xSemaphoreGiveFromISR (semaphore, &high_priority);
}

void task_led (void *args)
{
    LED_t *led = (LED_t *)args;

    while (true)
    {
        xSemaphoreTake (semaphore, portMAX_DELAY);
        driver_ioctl (led->handle, gpio_set_state, &led->state);
        led->state = !led->state;
        vTaskDelay (100/ portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    blue.handle = driver_open ("led_blue");
    handle_error (blue.handle, "LED Blue Error");

    int8_t button = driver_open ("button");
    handle_error (button, "Button Error");

    driver_ioctl (button, gpio_set_interrupt_handler, &handler);

    semaphore = xSemaphoreCreateBinary ();

    if (semaphore != NULL)
    {
        xTaskCreate (task_led, "blue", 2500, &blue, 2, NULL);
    }
    else 
    {
        while (true)
        {
            printf ("Error: Semaphore not created\n");
            vTaskDelay (1000/ portTICK_PERIOD_MS);
        }
    }
}
