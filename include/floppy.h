#ifndef _FLOPPY_H
#define _FLOPPY_H

#include "stdint.h"
#include "stdbool.h"

#define DISK_PARAMETER_ADDRESS 0x000fefc7
#define FLOPPY_DRIVE1 0x0
#define FLOPPY_DRIVE2 0x1

typedef struct{
  uchar_8 steprate_headunload;
  uchar_8 headload_ndma;
  uchar_8 motor_delay_off; /*specified in clock ticks*/
  uchar_8 bytes_per_sector;
  uchar_8 sectors_per_track;
  uchar_8 gap_length;
  uchar_8 data_length; /*used only when bytes per sector == 0*/
  uchar_8 format_gap_length;
  uchar_8 filler;
  uchar_8 head_settle_time; /*specified in milliseconds*/
  uchar_8 motor_start_time; /*specified in 1/8 seconds*/
}__attribute__ ((packed)) floppy_parameters;

enum FloppyRegisters
{
    STATUS_REGISTER_A                = 0x3F0, // read-only
    STATUS_REGISTER_B                = 0x3F1, // read-only
    DIGITAL_OUTPUT_REGISTER          = 0x3F2,
    TAPE_DRIVE_REGISTER              = 0x3F3,
    MAIN_STATUS_REGISTER             = 0x3F4, // read-only
    DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
    DATA_FIFO                        = 0x3F5,
    DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
    CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum FloppyCommands
{
   READ_TRACK =                 2,	// generates IRQ6
   SPECIFY =                    3,      // * set drive parameters
   CHECK_DRIVE_STATUS =         4,
   WRITE_DATA =                 5,      // * write to the disk
   READ_DATA =                  6,      // * read from the disk
   RECALIBRATE =                7,      // * seek to cylinder 0
   CHECK_INTERRUPT =            8,      // * ack IRQ6, get status of last command
   WRITE_DELETED_DATA =         9,
   READ_ID =                    10,	// generates IRQ6
   READ_DELETED_DATA =          12,
   FORMAT_TRACK =               13,     // *
   DUMPREG =                    14,
   SEEK =                       15,     // * seek both heads to cylinder X
   VERSION =                    16,	// * used during initialization, once
   SCAN_EQUAL =                 17,
   PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
   CONFIGURE =                  19,     // * set controller parameters
   LOCK =                       20,     // * protect controller params from a reset
   VERIFY =                     22,
   SCAN_LOW_OR_EQUAL =          25,
   SCAN_HIGH_OR_EQUAL =         29
};

typedef enum
{
    DMA_WRITE = 0b01011000,
    DMA_READ =  0b01010100
} DMA_mode;

typedef struct 
{
  char drive;
  floppy_parameters params;
  uchar_8 st0, st1, st2, c, h, s, ss;
  uchar_8 motor_status;

} floppy_d_data;


void init_floppy(char drive);
// void reset_floppy_controller(char drive);
// void floppy_read(int memaddr, uchar_8 drive, uchar_8 cy, uchar_8 hd, uchar_8 sc, int size);
// void floppy_write(int memaddr, uchar_8 drive, uchar_8 cy, uchar_8 hd, uchar_8 sc, int size);
// void lba_2_chs(int lba, uchar_8* cy, uchar_8* hd, uchar_8* sc);

#endif