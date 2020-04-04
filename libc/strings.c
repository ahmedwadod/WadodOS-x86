#include "strings.h"

/**
 * Convert a number from type (int) to string format
 * Like toString() functions in most languages.
 * 
 * No return, it puts string value in str
*/
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}


/**
 * Reverse the order of charachters in a string
 * No returns, it remaps the given argument
*/
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/**
 * Gets length of a string.
 * 
 * Returns number of charachters
 */
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

/** 
 * Remove last letter from a string.
 * 
 * No return, it edits the given string.
*/
void droplastchar(char* str)
{
    int len = strlen(str);
    str += len - 1;
    *str++ = '\0';
    str -= len;
}

/** 
 * Adds a char(letter) to the end of string(str).
 * 
 * No return, it edits the given string.
*/
void appendchar(char* str, char letter)
{
    int len = strlen(str);
    str += len;
    *str++ = letter;
    *str = '\0';
    str -= (len + 1);
}

char strcmp_len(char *a, char *b, int count)
{
    char *tempA = a;
    char *tempB = b;
    for (int i = 0; i < count; i++)
    {
        if (*tempA++ != *tempB++)
        {
            return 0;
        }
    }
    
    return 1;
}

/**
 * Appends string (addition) to (origin).
 * 
 * No return
*/
void combine(char* origin, char* addition)
{
    int endOfOrigin = (int)origin + strlen(origin);
    memory_copy(addition, (char*)endOfOrigin, strlen(addition));
}