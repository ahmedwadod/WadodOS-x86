#include "../include/floppy.h"
#include "../include/isr.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/ports.h"
#include "../include/device.h"

void motor_turn_on(floppy_d_data* f_data);
void configure_drive(floppy_parameters params);
void calibrate_drive(floppy_d_data* f_data);
void check_interupt_status(uchar_8* st0, uchar_8* cy);
void wait_until_floppy_ready();
void write_floppy_command(uchar_8 command);
uchar_8 read_floppy_data();
void floppy_seek(floppy_d_data* f_data, uchar_8 hd, uchar_8 cy);
void floppy_read_result(uchar_8* ost0, uchar_8* ost1, uchar_8* ost2, uchar_8* oc, uchar_8* oh, uchar_8* osn, uchar_8* oss);
void init_dma(uchar_8 mode, uint_32 addr, uint_32 bytescount);

void floppy_std_read(char* buffer, uint_32 lba, uint_32 byteCount, floppy_d_data* d_data);
void floppy_std_write(char* buffer, uint_32 lba, uint_32 byteCount, floppy_d_data* d_data);

/* Start */



static bool FloppyIRQ_Recieved = false;


void Floppy_Reset_IRQ_Handler(registers_t t)
{
    FloppyIRQ_Recieved = true;
}

/** 
 * Configure the Floppy Disk Controller for the first time.
*/
void init_floppy(char drive)
{
    floppy_parameters paras;
    memcpy((uchar_8*)DISK_PARAMETER_ADDRESS, &paras, 11);
    register_interrupt_handler(IRQ6, Floppy_Reset_IRQ_Handler);

    floppy_d_data fdata;
    fdata.drive = drive;
    fdata.params = paras;
    fdata.motor_status = 0;
    fdata.c = fdata.h = fdata.s = fdata.ss = fdata.st0 = fdata.st1 = fdata.st2 = 0;
    device_t floppy;
    floppy.ID = 2 + drive;
    char *name = "FD";
    char *no;
    itoa(drive, no, 10);
    strcat(name, no);
    strcpy(floppy.name, name);
    memcpy(&fdata, &floppy.data, sizeof(floppy_d_data));
    floppy.d_read = floppy_std_read;
    floppy.d_write = floppy_std_write;

    reset_floppy_controller(&floppy.data);

    add_sys_device(floppy);
    return;
}

/**
 * Reset the floppy drive, also configure it and calibrate it.
 * 
 * drive: Floppy drive number (FLOPPY_DRIVE1 or FLOPPY_DRIVE2)
*/
void reset_floppy_controller(floppy_d_data* f_data)
{
    FloppyIRQ_Recieved = false;

    // Enter and exit the reset
    port_byte_out(DIGITAL_OUTPUT_REGISTER, 0x00);
    port_byte_out(DIGITAL_OUTPUT_REGISTER, 0b1100 | f_data->drive);

    while (!FloppyIRQ_Recieved);
    
    check_interupt_status(&f_data->st0, &f_data->c);    

    port_byte_out(CONFIGURATION_CONTROL_REGISTER, 0x0); // Set at 500kbps
   
    configure_drive(f_data->params);
    calibrate_drive(f_data);
    
    return;
}

void configure_drive(floppy_parameters params)
{
    write_floppy_command(SPECIFY);
    write_floppy_command(params.steprate_headunload);
    write_floppy_command(params.headload_ndma);
    return;
}

void calibrate_drive(floppy_d_data* f_data)
{
    FloppyIRQ_Recieved = false; // Must be at top

    motor_turn_on(f_data);
    write_floppy_command(RECALIBRATE);
    write_floppy_command(f_data->drive);

    while (!FloppyIRQ_Recieved);

    check_interupt_status(&f_data->st0, &f_data->c);

    if(f_data->st0 & 0xc0)
    {
        kpanic("Floppy Calibrate Error", f_data->st0);
    }
    if(f_data->c == 0)
    {
        return;
    }
    else
    {
        calibrate_drive(f_data);
    }   

}

void motor_turn_on(floppy_d_data* f_data)
{
    if(f_data->motor_status == 1)
        return;
    int sleeptime = (int)f_data->params.motor_start_time * 125;
    if(f_data->drive == FLOPPY_DRIVE1)
        port_byte_out(DIGITAL_OUTPUT_REGISTER, 0b00011100 | f_data->drive);
    if(f_data->drive == FLOPPY_DRIVE2)
        port_byte_out(DIGITAL_OUTPUT_REGISTER, 0b00101100 | f_data->drive);
    sleep(sleeptime);
    f_data->motor_status = 1;
}

void check_interupt_status(uchar_8* st0, uchar_8* cy)
{
    write_floppy_command(CHECK_INTERRUPT);
    *st0 = read_floppy_data();
    *cy = read_floppy_data();

    return;
}

void wait_until_floppy_ready()
{
    uchar_8 d = port_byte_in(MAIN_STATUS_REGISTER);
    while ((d&0x80) != 0x80)
    {
        d = port_byte_in(MAIN_STATUS_REGISTER);
    }

    return;
}

void write_floppy_command(uchar_8 command)
{
    wait_until_floppy_ready();
    port_byte_out(DATA_FIFO, command);
    return;
}

uchar_8 read_floppy_data()
{
    wait_until_floppy_ready();
    return port_byte_in(DATA_FIFO);
}

void floppy_seek(floppy_d_data* f_data, uchar_8 hd, uchar_8 cy)
{
    FloppyIRQ_Recieved = false;
    motor_turn_on(f_data);

    write_floppy_command(SEEK);
    write_floppy_command((hd << 2) | f_data->drive);
    write_floppy_command(cy);

    while (!FloppyIRQ_Recieved);

    check_interupt_status(&f_data->st0, &f_data->c);
    
    if(((f_data->st0>>5) & 0x01) && cy == f_data->c)
    {
        return;
    }
    else
    {
        kpanic("Floppy Seek Error.", f_data->st0);
    }
}

/**
 * Convert Logical Block Addressing to Cylinder-Head-Sector Addressing
 * This function gives you the desired CHS of a certain sector in LBA.
 * It returns sectors+1
 * 
 * lba: LBA value
 * cy: Pointer to cylinder variable
 * hd: Pointer to head variable
 * sc: Pointer to sector variable
*/
void lba_2_chs(int lba, uchar_8* cy, uchar_8* hd, uchar_8* sc)
{
    int trips = (lba-1) / 18;
    *cy = trips / 2;
    *hd = trips & 0x01; // AND 0001b
    *sc = lba - (trips * 18) + 1;
}

/**
 * Read Floppy to Memory Function.
 * It start reading from the floppy disk and loading the data to memory using DMA.
 * This Function reads the floppy in Multitracking mode.
 * 
 * memaddr: Address to load the data.
 * drive: Floppy drive number (FLOPPY_DRIVE1, FLOPPY_DRIVE2)
 * cy: Starting cylinder
 * hd: Starting head
 * sc: Starting sector
 * size: The length of data to load from floppy to memory in bytes
*/
void floppy_read(int memaddr, uchar_8 drive, uchar_8 cy, uchar_8 hd, uchar_8 sc, int size, floppy_d_data* f_data)
{
    FloppyIRQ_Recieved = false;

    floppy_seek(f_data, 0, cy);
    floppy_seek(f_data, 1, cy);

    motor_turn_on(f_data);
    init_dma(DMA_READ, memaddr, size);
    sleep(f_data->params.head_settle_time);

    write_floppy_command(READ_DATA | 0x40 | 0x80);
    write_floppy_command((hd << 2)|drive);
    write_floppy_command(cy);
    write_floppy_command(hd);
    write_floppy_command(sc);
    write_floppy_command(f_data->params.bytes_per_sector);
    write_floppy_command(f_data->params.sectors_per_track);
    write_floppy_command(f_data->params.gap_length);
    write_floppy_command(f_data->params.data_length);

    while(!FloppyIRQ_Recieved){}

    floppy_read_result(&f_data->st0, &f_data->st1, &f_data->st2, &f_data->c, &f_data->h, &f_data->s, &f_data->ss);

    return;
    
}

/** Write Floppy from Memory Function 
 * This function starts writing data from a given memory address into the floppy disk using DMA.
 * It uses Multitracking.
 * 
 * memaddr: Memory address of content to write
 * drive: Floppy drive number (FLOPPY_DRIVE1, FLOPPY_DRIVE2)
 * cy: Starting cylinder
 * hd: Starting head
 * sc: Starting sector
 * size: Length of data to write in bytes
*/
void floppy_write(int memaddr, uchar_8 drive, uchar_8 cy, uchar_8 hd, uchar_8 sc, int size, floppy_d_data* f_data)
{
    FloppyIRQ_Recieved = false;

    floppy_seek(f_data, 0, cy);
    floppy_seek(f_data, 1, cy);

    motor_turn_on(f_data);
    init_dma(DMA_WRITE, memaddr, size);
    sleep(f_data->params.head_settle_time);

    write_floppy_command(WRITE_DATA | 0x40);
    write_floppy_command((hd << 2)|drive);
    write_floppy_command(cy);
    write_floppy_command(hd);
    write_floppy_command(sc);
    write_floppy_command(f_data->params.bytes_per_sector);
    write_floppy_command(f_data->params.sectors_per_track);
    write_floppy_command(f_data->params.gap_length);
    write_floppy_command(f_data->params.data_length);

    while(!FloppyIRQ_Recieved){}

    floppy_read_result(&f_data->st0, &f_data->st1, &f_data->st2, &f_data->c, &f_data->h, &f_data->s, &f_data->ss);

    return;
}


void init_dma(uchar_8 mode, uint_32 addr, uint_32 bytescount)
{
    union
    {
        uchar_8 b[4];
        unsigned long l;
    } address, count;

    address.l = (unsigned) addr;
    count.l = (unsigned) bytescount - 1;

    if((address.l >> 24) || (count.l >> 16) || (((address.l&0xffff)+count.l)>>16)) {
        kpanic("init_dma: static buffer problem\n", 0);
    }

    mode |= 0x02;

    __asm__ volatile("cli"); // Disable interrupts

    // Mask
    port_byte_out(0x0A, 0x06);

    port_byte_out(0x0C, 0xFF); // Reset the flip-flop

    // Set address
    port_byte_out(0x04, address.b[0]); // Low byte
    port_byte_out(0x04, address.b[1]); // High byte
    port_byte_out(0x81, address.b[2]); // Page

    port_byte_out(0x0C, 0xFF); // Reset the flip-flop

    // Set count
    port_byte_out(0x05, count.b[0]); // Low byte
    port_byte_out(0x05, count.b[1]); // High byte

    // Set mode
    port_byte_out(0x0B, mode);

    // Unmask
    port_byte_out(0x0A, 0x02);

    __asm__ volatile("sti"); // Enable interrupts
}

void floppy_read_result(uchar_8* ost0, uchar_8* ost1, uchar_8* ost2, uchar_8* oc, uchar_8* oh, uchar_8* osn, uchar_8* oss)
{
    *ost0 = read_floppy_data();
    *ost1 = read_floppy_data();
    *ost2 = read_floppy_data();
    *oc = read_floppy_data();
    *oh = read_floppy_data();
    *osn = read_floppy_data();
    *oss = read_floppy_data();

    int error = 0;

    if(*ost0 & 0xC0) {
        static const uchar_8 * status[] =
        { 0, "error", "invalid command", "drive not ready" };
        kpanic("Floppy Error: General error in st0.", *ost0);
        error = 1;
    }
    if(*ost1 & 0x80) {
        kpanic("Floppy Error: end of cylinder.", *ost0);
        error = 1;
    }
    if(*ost0 & 0x08) {
        kpanic("Floppy Error: drive not ready.", *ost0);
        error = 1;
    }
    if(*ost1 & 0x20) {
        kpanic("Floppy Error: CRC error.", *ost1);
        error = 1;
    }
    if(*ost1 & 0x10) {
        kpanic("Floppy Error: controller timeout.", *ost1);
        error = 1;
    }
    if(*ost1 & 0x04) {
        kpanic("Floppy Error: no data found.", *ost1);
        error = 1;
    }
    if((*ost1|*ost2) & 0x01) {
        kpanic("Floppy Error: no address mark found.", *ost1);
        error = 1;
    }
    if(*ost2 & 0x40) {
        kpanic("Floppy Error: deleted address mark.", *ost2);
        error = 1;
    }
    if(*ost2 & 0x20) {
        kpanic("Floppy Error: CRC error in data.", *ost2);
        error = 1;
    }
    if(*ost2 & 0x10) {
        kpanic("Floppy Error: wrong cylinder.", *ost2);
        error = 1;
    }
    if(*ost2 & 0x04) {
        kpanic("Floppy Error: uPD765 sector not found.", *ost2);
        error = 1;
    }
    if(*ost2 & 0x02) {
        kpanic("Floppy Error: bad cylinder.", *ost2);
        error = 1;
    }
    if(*oss != 0x2) {
        kpanic("Floppy Error: cannot read bytes.", *oss);
        error = 1;
    }
    if(*ost1 & 0x02) {
        kpanic("Floppy Error: not writable.",  *ost1);
        error = 2;
    }

    if(!error) {
        return;
    }
    if(error > 1) {
        kpanic("Floppy Error: Too many errors.", error);
        return ;
    }
    
    return;
}

void floppy_std_read(char* buffer, uint_32 lba, uint_32 byteCount, floppy_d_data* d_data)
{
    char c, h, s;
    lba_2_chs(lba, &c, &h, &s);
    floppy_read((int)buffer, d_data->drive, c, h, s, byteCount, d_data);
}

void floppy_std_write(char* buffer, uint_32 lba, uint_32 byteCount, floppy_d_data* d_data)
{
    char c, h, s;
    lba_2_chs(lba, &c, &h, &s);
    floppy_write((int)buffer, d_data->drive, c, h, s, byteCount, d_data);
}