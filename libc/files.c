#include "files.h"

void _convertFatNameToFileName(char fatName[8], char ext[3], char *filename);
void _convertFileNameToFatName(char filename[12], char *fatName, char *ext);
ushort_16 _getClusterValue(ushort_16 cluster);
ushort_16 _getNextEmptyCluster();
void _writeRootDirectory();
void _writeFatTables();

static char DEFAULT_FLOPPY;
static FAT_DirectoryEntry root_directory[224];
static FAT_Table *FAT_TABLE1 = (FAT_Table*) FAT_TABLE1_MEMADDRESS;
static FAT_Table *FAT_TABLE2 = (FAT_Table*) FAT_TABLE2_MEMADDRESS;

void init_fat12(char drive)
{
    DEFAULT_FLOPPY = drive;
    setup_floppy();
    reset_floppy_controller(drive);

    /* Read tables */
    kmalloc(FAT_TABLE1_MEMADDRESS, 4608);
    kmalloc(FAT_TABLE2_MEMADDRESS, 4608);
    char c,h,s;
    lba_2_chs(1, &c, &h, &s);
    floppy_read(FAT_TABLE1_MEMADDRESS, DEFAULT_FLOPPY, c, h, s, 4608);
    memory_copy(FAT_TABLE1_MEMADDRESS, FAT_TABLE2_MEMADDRESS, 4608);

    /* Read root directory */
    int rdPtr = 0xb000;
    kmalloc((char*) rdPtr, 7680);
    lba_2_chs(19, &c, &h, &s);
    floppy_read(rdPtr, DEFAULT_FLOPPY, c, h, s, 7680);
    union
    {
        char raw[7680];
        FAT_DirectoryEntry dic[224];
    } ent;
    memory_copy((char*)rdPtr, ent.raw, 7680);

    // Only 34 files maximum
    for(int i = 0; i < 34; i++)
    {
        root_directory[i] = ent.dic[i];
    }
}

FILE fopen(char filename[12], bool create)
{
    char fatName[9];
    char ext[4];

    FILE f;

    _convertFileNameToFatName(filename, &fatName, &ext);
    for (int i = 0; i < 34; i++)
    {
        char nc = strcmp_len(root_directory[i].name, fatName, 8);
        char ec = strcmp_len(root_directory[i].extension, ext, 3);
        if(nc && ec)
        {
            memory_copy(fatName, f.name, 8);
            memory_copy(ext, f.extension, 3);
            memory_copy(filename, f.filename, 12);
            f.directoryEntry = &root_directory[i];
            break;
        }
        else if(create)
        {
            int emptySlotInRD = 0;
            for(emptySlotInRD; emptySlotInRD < 34; emptySlotInRD++)
            {
                if(root_directory[emptySlotInRD].name[0] == '\0')
                {
                    break;
                }
            }
            ushort_16 emptyCluster = _getNextEmptyCluster();
            
            FAT_TABLE1[emptyCluster].s = 0xFF0;

            memory_copy(fatName, f.name, 8);
            memory_copy(ext, f.extension, 3);
            memory_copy(filename, f.filename, 12);

            root_directory[emptySlotInRD].firstLogicalCluster = emptyCluster;
            root_directory[emptySlotInRD].longName[0] = 'A';
            memory_copy(fatName, root_directory[emptySlotInRD].name, 8);
            memory_copy(ext, root_directory[emptySlotInRD].extension, 3);
            f.directoryEntry = &root_directory[emptySlotInRD];

            _writeFatTables();
            _writeRootDirectory();
            break;
        }
        else
        {
            f.name[0] = '\0';
        }
    }
    
    return f;
}

bool isEOF(ushort_16 clusval)
{
    return (clusval >= 0xFF0 || clusval <= 0xFFF);
}
void fread(FILE f, char* buffer)
{
    ushort_16 clusterValue = f.directoryEntry->firstLogicalCluster;
    int byteCounter = 0;
    char c, h, s;
    do
    {
        int ptr = byteCounter + (int)buffer;
        lba_2_chs(31+clusterValue, &c, &h, &s);
        floppy_read(ptr, DEFAULT_FLOPPY, c, h, s, 512);
        byteCounter += 512;
        clusterValue = _getClusterValue(clusterValue);
    } while(!isEOF(clusterValue));
}

void fwrite(FILE f, char* data, int length)
{
    ushort_16 currentCluster = f.directoryEntry->firstLogicalCluster;
    ushort_16 clusterValue = 0;
    int writeLength = length;
    if(writeLength > 512)
        writeLength = 512;
    char c, h, s;
    for (ushort_16 i = 0; i < length; i+=512)
    {
        int ptr = i + (int)data;
        lba_2_chs(31+currentCluster, &c, &h, &s);
        floppy_write(ptr, DEFAULT_FLOPPY, c, h, s, writeLength);
        clusterValue = _getClusterValue(currentCluster);
        if(isEOF(clusterValue))
        {
            FAT_TABLE1[currentCluster].s = _getNextEmptyCluster();
        }
        else
        {
            currentCluster = clusterValue;
        }
        
    }
    
    _writeFatTables();
    _writeRootDirectory();
}

FILE* getAllFiles()
{
    FILE f;
    static FILE *files;
    for (int i = 0; i < 34; i++)
    {
        if(root_directory[i].name[0] == '\0')
        {
            *(f.filename) = 0;
            *(f.extension) = 0;
            *(f.name) = 0;
            f.directoryEntry = 0;
            files[i] = f;
            break;
        }
        _convertFatNameToFileName(root_directory[i].name, root_directory[i].extension, f.filename);
        memory_copy(root_directory[i].name, f.name, 8);
        memory_copy(root_directory[i].extension, f.extension, 3);
        f.directoryEntry = &root_directory[i];
        files[i] = f;
        i++;
    }
    
    return files;
}



void _convertFatNameToFileName(char fatName[8], char ext[3], char *filename)
{
    char *temp = filename;

    int nameCount = 1;
    for (int i = 7; i >= 0; i--)
    {
        if(fatName[i] != ' ')
        {
            nameCount += i;
            break;
        }
    }
    for (int i = 0; i < nameCount; i++)
    {
        *temp++ = fatName[i];
    }
    *temp++ = '.';
    for (int i = 0; i < 3; i++)
    {
        if(ext[i] != ' ')
            *temp++ = ext[i];
        else
        {
            *temp = '\0';
            break;
        }
    }
    *temp = '\0';    
    
}
void _convertFileNameToFatName(char filename[12], char *fatName, char *ext)
{
    char *tempName = fatName;
    char *tempExt = ext;

    int i = 0;
    for (; i < 8; i++)
    {
        if(filename[i] != '.')
        {
            *tempName++ = filename[i];
        }
        else
        {
            int count = 8 - i;
            for (int j = 0; j < count; j++)
            {
                *tempName++ = ' ';
            }
            break;
        }
    }
    i++;
    for (int k = 0; k < 3; k++)
    {
        if(filename[i] != '\0')
        {
            *tempExt++ = filename[i];
            i++;
        }
        else
        {
            break;
        }
    }
}

// Clusters starting from 1
ushort_16 _getClusterValue(ushort_16 cluster)
{
    cluster--;
    uchar_8 isOdd = cluster % 2;
    if(!isOdd) // Is even
    {
        return ((FAT_TABLE1[cluster].s >> 1) & 0x0FFF);
    }
    else
    {
        ushort_16 firstHalfByte = FAT_TABLE1[cluster-1].s & 0x000F;
        ushort_16 second = (FAT_TABLE1[cluster].s << 3) & 0xFFF0;
        return second | firstHalfByte;
    }
}

ushort_16 _getNextEmptyCluster()
{
    for (ushort_16 i = 2; i < 2304; i++)
    {
        if(_getClusterValue(i) == 0x00)
        {
            return i;
        }
    }
    
    return 0;    
}

void _writeFatTables()
{
    char c,h,s;
    lba_2_chs(1, &c, &h, &s);
    floppy_write(FAT_TABLE1_MEMADDRESS, DEFAULT_FLOPPY, c, h, s, 4608); // TABLE 1
    lba_2_chs(10, &c, &h, &s);
    floppy_write(FAT_TABLE1_MEMADDRESS, DEFAULT_FLOPPY, c, h, s, 4608); // TABLE 2
}

void _writeRootDirectory()
{
    int rdPtr = 0xb000;
    kmalloc((char*) rdPtr, 7680);
    
    union
    {
        char raw[7680];
        FAT_DirectoryEntry dic[224];
    } ent;
    
    // Only 34 files maximum
    for(int i = 0; i < 34; i++)
    {
        ent.dic[i] = root_directory[i];
    }

    memory_copy(ent.raw, (char*)rdPtr, 7680);
    char c, h, s;
    lba_2_chs(19, &c, &h, &s);
    floppy_write(rdPtr, DEFAULT_FLOPPY, c, h, s, 7680);
}