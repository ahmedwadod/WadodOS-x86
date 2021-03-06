#ifndef _FAT12_H
#define _FAT12_H

#include "stdint.h"
#include "stdbool.h"

typedef struct {
    char longName[16];
    char longExt[16];
    char name[8];
    char extension[3];
    char attribute;
    char reserved[2];
    char creationTime[2];
    char creationDate[2];
    char lastAcessDate[2];
    char ignoreInFAT12[2];
    char lastWriteTime[2];
    char lastWriteDate[2];
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

static char* FATT_MEM_BUFFER = 0;
static char* RD_MEM_BUFFER = 0;


void init_fat12();
FILE fopen(char filename[12], bool create);
void fread(FILE f, char* buffer);
void fwrite(FILE f, char* data, int length);
FILE* getAllFiles();

#endif