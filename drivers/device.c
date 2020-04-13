#include "../include/device.h"


void init_devices()
{
    kmalloc(sys_devices, 32 * sizeof(device_t));
}

void add_sys_device(device_t dev)
{
    sys_devices[lastDev] = dev;
    lastDev++;
}

device_t* get_sys_device(uint_32 id)
{
    for (int i = 0; i < 32; i++)
    {
        if(sys_devices[i].ID == id)
        {
            return &sys_devices[i];
        }
    }
    
    return NULL;
}