#ifndef DRIVER_BASE_H_
#define DRIVER_BASE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    bool (*init) (const void *object);
    int8_t (*open) (const void *object);
    int32_t (*write) (const void *object, const void *buffer, uint32_t size);
    int32_t (*read) (const void *object, void *buffer, uint32_t size);
    bool (*ioctl) (const void *object, uint8_t command, void *args);
    bool (*close) (const void *object);
} driver_base_t;

#endif /* DRIVER_BASE_H_ */
