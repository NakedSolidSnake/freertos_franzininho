#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver_operation.h"
#include "gpio_options.h"

SemaphoreHandle_t semaphore;

typedef struct 
{
    char *message;
    int delay_ms;
} message_t;

void print (const char *message)
{
    xSemaphoreTake (semaphore, portMAX_DELAY);
    size_t size = strlen (message);

    for (size_t i = 0; i < size; i++)
        printf ("%c", message[i]);
    xSemaphoreGive (semaphore);
}

void task_print (void *args)
{
    message_t *message = (message_t *)args;

    while (true)
    {
        print (message->message);
        vTaskDelay (message->delay_ms/ portTICK_PERIOD_MS);
    }
}

void app_main(void)
{

    semaphore = xSemaphoreCreateMutex ();

    static message_t m1 =
    {
        .delay_ms = 10,
        .message = "Task 1: Não perca as novidades no site www.embarcados.com.br, pois podemos encontrar diversas informações e novidade sobre tecnologia e oportunidades de emprego\n"
    };

    static message_t m2 = 
    {
        .delay_ms = 0,
        .message = "Task 2: Não esqueçam de seguir no Instagram e se inscreverem no canal do Embarcados. No canal do youtube podemos assistir diversos conteudos sobre tecnologia e não devemos esquecer de participar do café que acontece as quartas feiras\n"
    };

    if (semaphore != NULL)
    {
        xTaskCreate (task_print, "print", 2500, &m1, 2, NULL);
        xTaskCreate (task_print, "print", 2500, &m2, 1, NULL);
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
