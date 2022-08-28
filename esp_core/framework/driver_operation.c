#include "driver_operation.h"
#include <string.h>

extern uint8_t drivers_amount;
extern driver_operation_t *drivers;

int8_t driver_open (const char *name)
{
    int8_t i;
    int8_t handle = -1;

    if (name != NULL)
    {
        for (i = 0; i < drivers_amount; i++)
        {
            driver_operation_t *driver = &drivers[i];

            if (strcmp (name, driver->name) == 0)
            {
                if (driver->driver == NULL)
                    break;

                if (driver->is_open || driver->driver->open (driver->descriptor))
                {
                    driver->is_open = true;
                    handle = i;
                    break;
                }
            }
        }
    }

    return handle;
}

int32_t driver_write (int8_t handle, const void *buffer, uint32_t size)
{
    int32_t count = -1;

    if (handle >= 0 && handle < drivers_amount)
    {
        driver_operation_t *driver = &drivers[handle];
        count = driver->driver->write (driver->descriptor, buffer, size);
    }
    return count;
}

int32_t driver_read (int8_t handle, void *buffer, uint32_t size)
{
    int32_t count = -1;

    if (handle >= 0 && handle < drivers_amount)
    {
        driver_operation_t *driver = &drivers[handle];
        count = driver->driver->read (driver->descriptor, buffer, size);
    }
    return count;
}

bool driver_ioctl (int8_t handle, uint8_t command, void *args)
{
    bool status = false;
    if (handle >= 0 && handle < drivers_amount)
    {
        driver_operation_t *driver = &drivers[handle];
        status = driver->driver->ioctl (driver->descriptor, command, args);
    }

    return status;
}

bool driver_close (int8_t handle)
{
    bool status = false;
    if (handle >= 0 && handle < drivers_amount)
    {
        driver_operation_t *driver = &drivers[handle];
        if (driver->is_open)
        {
            driver->driver->close (driver->descriptor);
            driver->is_open = false;
            status = true;
        }
    }

    return status;
}