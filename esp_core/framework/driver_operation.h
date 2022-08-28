#ifndef DRIVER_OPERATION_H_
#define DRIVER_OPERATION_H_

#include "driver_base.h"

typedef struct 
{
    const char *name;
    bool is_open;
    const void *descriptor;
    const driver_base_t *driver;
} driver_operation_t;

int8_t driver_open (const char *name);
int32_t driver_write (int8_t handle, const void *buffer, uint32_t size);
int32_t driver_read (int8_t handle, void *buffer, uint32_t size);
bool driver_ioctl (int8_t handle, uint8_t command, void *args);
bool driver_close (int8_t handle);

#endif /* DRIVER_OPERATION_H_ */
