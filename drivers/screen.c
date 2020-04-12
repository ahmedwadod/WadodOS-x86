#include "../include/screen.h"
#include "../include/stdlib.h"

/* Declaration of private functions */
int get_cursor_offset();
void set_cursor_offset(int offset);
int kprint_char(char c, int col, int row, int attr);
int handle_scroll(int offset, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

/**********************************************************
 * Public functions                            *
 **********************************************************/

/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 */
void kprint_at(char* message, int col, int row) {
    char* temp = message;
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    while (*temp != (char)0) {
        offset = kprint_char(*temp, col, row, CURRENT_DISPLAY_STYLE);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
        temp++;
    }
}

/**
 * Print a message on the specified location and attribute
 * If col, row, are negative, we will use the current offset
 */
void kprint_at_with_attr(char* message, int col, int row, int attr)
{
    char* temp = message;
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    if(attr == 0)
        attr = CURRENT_DISPLAY_STYLE;

    while (*temp != (char)0) {
        offset = kprint_char(*temp, col, row, attr);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
        temp++;
    }
}

/** 
 * Print a message at the current cursor location
*/
void kprint(char *message) {
    kprint_at(message, -1, -1);
}

/** 
 * Clear the screen in 32bit mode
*/
void kclear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i = 0;
    uchar_8* screen = VIDEO_ADDRESS;

    while (i < screen_size)
    {
        *screen++ = ' ';
        *screen++ = CURRENT_DISPLAY_STYLE;
        i++;
    }
    
    set_cursor_offset(get_offset(0, 0));
}

/** 
 * Clear the screen with color in 32bit mode
*/
void kclear_screen_with_attr(int attr) {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i = 0;
    uchar_8* screen = VIDEO_ADDRESS;

    while (i < screen_size)
    {
        *screen++ = ' ';
        *screen++ = attr;
        i++;
    }
    
    set_cursor_offset(get_offset(0, 0));
}


/**
 * Innermost print function for our kernel, directly accesses the video memory 
 *
 * If 'col' and 'row' are negative, we will print at current cursor location
 * If 'attr' is zero it will use 'white on black' as default
 * Returns the offset of the next character
 * Sets the video cursor to the returned offset
 */
int kprint_char(char c, int col, int row, int attr) {
    uchar_8* vidmem = VIDEO_ADDRESS;
    if (!attr) attr = CURRENT_DISPLAY_STYLE;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    }
    else if(c == '\r')
    {
        // Do nothing
    }
    else if(c == '\b')
    {
        offset -= 2;
    }
    else if(c == '\t')
    {
        for (int t = 0; t < 4; t++)
        {
            vidmem += offset;
            *vidmem++ = ' ';
            *vidmem++ = attr;
            offset += 2;
        }
        
    }
    else {
        vidmem += offset;
        *vidmem++ = c;
        *vidmem++ = attr;
        offset += 2;
    }

    //Handling the scroll
    offset = handle_scroll(offset, attr);
    
    set_cursor_offset(offset);
    return offset;
}

/**
 * Setup the display style
*/
void kset_display(uchar_8 attr)
{
    CURRENT_DISPLAY_STYLE = attr;
    kclear_screen_with_attr(CURRENT_DISPLAY_STYLE);
}


/**********************************************************
 * Private kernel functions                               *
 **********************************************************/

int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (uchar_8)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (uchar_8)(offset & 0xff));
}

int handle_scroll(int offset, char attr)
{
    /* Check if the offset is over screen size and scroll */
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        int i;
        for (i = 1; i < MAX_ROWS; i++) 
            memcpy(get_offset(0, i) + VIDEO_ADDRESS,
                        get_offset(0, i-1) + VIDEO_ADDRESS,
                        MAX_COLS * 2);

        /* Blank last line */
        char *last_line = get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS;
        for (i = 0; i < MAX_COLS * 2; i+=2) 
        {
            last_line[i] = ' ';
            last_line[i + 1] = attr;
        }

        offset -= 2 * MAX_COLS;
    }

    return offset;
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }
