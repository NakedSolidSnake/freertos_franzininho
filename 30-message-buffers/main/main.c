#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"

#define BYTES_AMOUNT    20

MessageBufferHandle_t message_buffer;

void task_sender (void *args)
{
    char *message = "Hello World";

    while (true)
    {
        xMessageBufferSend (message_buffer, (void *)message, strlen (message), 0);
        vTaskDelay (1000/ portTICK_PERIOD_MS);
    }
}

void task_receiver (void *args)
{
    char buffer [BYTES_AMOUNT] = {0};
    size_t bytes_amount;
    while (true)
    {
        bytes_amount = xMessageBufferReceive (message_buffer,
                                             (void *)buffer,
                                             sizeof (buffer),
                                             portMAX_DELAY);

        if (bytes_amount > 0)
        {
            buffer[bytes_amount] = 0;
            printf ("Received: %s\n", buffer);
        }   
    }
}

void app_main(void)
{
    message_buffer = xMessageBufferCreate (BYTES_AMOUNT);

    if (message_buffer != NULL)
    {
        xTaskCreate (task_sender, "sender", 2500, NULL, 2, NULL);
        xTaskCreate (task_receiver, "receiver", 2500, NULL, 2, NULL);
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
