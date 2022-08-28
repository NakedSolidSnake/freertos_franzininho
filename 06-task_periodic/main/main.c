#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_periodic (void *args)
{
    const char *message = "Task Periodic";

    const TickType_t xDelay500ms = pdMS_TO_TICKS (500);
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount ();

    while (1)
    {
        printf ("%s\n", message);
        vTaskDelayUntil (&xLastWakeTime, xDelay500ms);
    }
}

void task_message (void *args) 
{
    char *msg = (char *)args;
    while (1)
    {        
        printf ("Message: %s.\n", msg);
        vTaskDelay (1000 / portTICK_PERIOD_MS);
    } 
}

static char *message1 = "Message 1";
static char *message2 = "Message 2";

void app_main(void)
{
    xTaskCreate (task_message, "task message 1", 2500, (void *)message1, 1 , NULL);
    xTaskCreate (task_message, "task message 2", 2500, (void *)message2, 1 , NULL);
    xTaskCreate (task_periodic, "task periodic", 2500, NULL, 2 , NULL);
}
