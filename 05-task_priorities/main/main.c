#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_message (void *args) 
{
    char *msg = (char *)args;
    while (1)
    {        
        printf ("Message: %s.\n", msg);
    } 
}

static char *message1 = "Message 1";
static char *message2 = "Message 2";

void app_main(void)
{
    xTaskCreate (task_message, "task message 1", 2500, (void *)message1, 2 , NULL);
    xTaskCreate (task_message, "task message 2", 2500, (void *)message2, 1 , NULL);
}
