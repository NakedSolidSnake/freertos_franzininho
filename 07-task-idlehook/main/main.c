#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vApplicationIdleHook (void);

static int count = 0;

void task_periodic (void *args)
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS (500);
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount ();

    while (1)
    {
        printf ("Count: %d\n", count);
        vTaskDelayUntil (&xLastWakeTime, xDelay500ms);
    }
}

void app_main(void)
{
    xTaskCreate (task_periodic, "task periodic", 2500, NULL, 2 , NULL);
}

void vApplicationIdleHook (void)
{
    count++;
}
