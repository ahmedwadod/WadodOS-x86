#include "files.h"

void _convertFatNameToFileName(char fatName[8], char ext[3], char *filename);
void _convertFileNameToFatName(char filename[12], char *fatName, char *ext);
ushort_16 _getClusterValue(ushort_16 cluster);

static char DEFAULT_FLOPPY;
static FAT_DirectoryEntry root_directory[224];
union
{
    uchar_8 b[2];
    ushort_16 s;
} FAT_TABLE1[2304], FAT_TABLE2[2304];

void init_fat12(char drive)
{
    DEFAULT_FLOPPY = drive;
    setup_floppy();
    reset_floppy_controller(drive);

    /* Read tables */
    int tbPtr = 0xd000;
    memory_set((char*)tbPtr, '\0', 4608);
    char c,h,s;
    lba_2_chs(1, &c, &h, &s);
    floppy_read(tbPtr, DEFAULT_FLOPPY, c, h, s, 4608);
    char *tbStr = (char*)tbPtr;
    for (int i = 0; i < 4608; i += 2)
    {
        FAT_TABLE1[i/2].b[0] = tbStr[i];
        FAT_TABLE1[i/2].b[1] = tbStr[i+1];
        // Redundant copy shit
        FAT_TABLE2[i/2].b[0] = tbStr[i];
        FAT_TABLE2[i/2].b[1] = tbStr[i+1];
    }

    /* Read root directory */
    int rdPtr = 0xb000;
    memory_set((char*) rdPtr, '\0', 7680);
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
            int emptyCluster = 1;
            for(emptySlotInRD; emptySlotInRD < 34; emptySlotInRD++)
            {
                if(root_directory[emptySlotInRD].name[0] == '\0')
                {
                    break;
                }
            }
            for (emptyCluster; emptyCluster < 2304; emptyCluster++)
            {
                if(_getClusterValue(emptyCluster) == 0)
                {
                    break;
                }
            }
            

            memory_copy(fatName, f.name, 8);
            memory_copy(ext, f.extension, 3);
            memory_copy(filename, f.filename, 12);

            root_directory[emptySlotInRD].firstLogicalCluster = emptyCluster;
            root_directory[emptySlotInRD].longName[0] = 'A';
            memory_copy(fatName, root_directory[emptySlotInRD].name, 8);
            memory_copy(ext, root_directory[emptySlotInRD].extension, 3);
            f.directoryEntry = &root_directory[emptySlotInRD];
            break;
        }
        else
        {
            f.name[0] = '\0';
            break;
        }
    }
    
    return f;
}

char isEOF(ushort_16 clusval)
{
    return (clusval == 0x00) || (clusval >= 0xFF0 || clusval <= 0xFFF);
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