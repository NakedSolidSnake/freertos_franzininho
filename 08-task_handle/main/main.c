#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t task2_handle = NULL;

void task_1 (void *args)
{
    UBaseType_t priority = uxTaskPriorityGet (NULL);

    while (1)
    {
        printf ("Task 1 running\n");
        vTaskPrioritySet (task2_handle, priority + 1);
    }
}

void task_2 (void *args)
{
    UBaseType_t priority = uxTaskPriorityGet (NULL);

    while (1)
    {
        printf ("Task 2 running\n");
        vTaskPrioritySet (NULL, priority - 2);
    }
}

void app_main(void)
{
    xTaskCreate (task_1, "task 1", 2500, NULL, 2 , NULL);
    xTaskCreate (task_2, "task 2", 2500, NULL, 2 , &task2_handle);
}

