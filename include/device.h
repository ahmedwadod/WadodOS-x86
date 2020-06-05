#ifndef _DEVICE_H
#define _DEVICE_H

#include "stdint.h"
#include "stdlib.h"

typedef struct
{
    char raw[255]; 
} d_data_t;

typedef struct
{
    uint_32 ID;
    char name[4];
    void (*d_read) (char* buffer, uint_32 lba, uint_32 byteCount, d_data_t* d_data);
    void (*d_write)(char* buffer, uint_32 lba, uint_32 byteCount, d_data_t* d_data);
    d_data_t data; 
} device_t;

static device_t sys_devices[32];
static uint_32 lastDev = 0;

void init_devices();
void add_sys_device(device_t dev);
device_t* get_sys_device(uint_32 id);

#endif