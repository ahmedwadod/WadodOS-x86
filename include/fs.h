#ifndef _FS_H
#define _FS_H

#include "stdint.h"
#include "stdbool.h"
#include "stdtime.h"
#include "device.h"

typedef struct {
    char longName[16];
    char longExt[16];
    char name[8];
    char extension[3];
    char attribute;
    char reserved[2];
    ushort_16 creationTime;
    ushort_16 creationDate;
    ushort_16 lastAcessDate;
    ushort_16 high16bitsofclusterFAT32;
    ushort_16 lastWriteTime;
    ushort_16 lastWriteDate;
    ushort_16 firstLogicalCluster;
    uint_32 fileSize;
} FAT_DirectoryEntry; // 64 Bytes

typedef struct
{
    char name[9]; // Last char for null
    char extension[4]; // Last char for null
    char filename[13]; // Last char for null
    FAT_DirectoryEntry *directoryEntry;
} FILE;

typedef enum
{
    FAT12 = 0,
    FAT16 = 1,
    FAT32 = 2
} FS_TYPE;

typedef char* fs_data_t;

typedef struct
{
    FS_TYPE type;
    device_t* device;
    FILE (*f_open)(char* filename, device_t* dev, fs_data_t* data);
    FILE* (*f_list)(device_t* dev, fs_data_t* data);
    bool (*f_exist)(char* filename, device_t* dev, fs_data_t* data);
    void (*f_touch)(char* filename, device_t* dev, fs_data_t* data);
    void (*f_read)(FILE f, char* buffer, device_t* dev, fs_data_t* data);
    void (*f_write)(FILE f, char* buffer, int count, device_t* dev, fs_data_t* data);
    void (*f_delete)(char* filename, device_t* dev, fs_data_t* data);
    fs_data_t data;
} filesystem_t;

typedef struct
{
    uchar_8 		bootjmp[3];
	uchar_8 		oem_name[8];
	ushort_16 	    bytes_per_sector;
	uchar_8		    sectors_per_cluster;
	ushort_16		reserved_sector_count;
	uchar_8		    table_count;
	ushort_16		root_entry_count;
	ushort_16		total_sectors_16;
	uchar_8		    media_type;
	ushort_16		table_size_16;
	ushort_16		sectors_per_track;
	ushort_16		head_side_count;
	uint_32 		hidden_sector_count;
	uint_32 		total_sectors_32;
}BOOTSECTOR;

static filesystem_t* filesystems = 0;
static int lastFS = 0;
static filesystem_t* current_fs = 0;

void init_fs();
void select_drive(char* drive);

void datetime_to_fatdatetime(datetime_t* dt, ushort_16* fattime, ushort_16* fatdate);
void fatdatetime_to_datetime(ushort_16* fattime, ushort_16* fatdate, datetime_t* dt);

#define fopen(filename) current_fs->f_open(filename, current_fs->device, current_fs->data)

#endif