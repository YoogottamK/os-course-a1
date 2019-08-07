#ifndef __UTILS_H
#define __UTILS_H

//--------------+
// For printing |
//--------------+

/*
 * print: My own version of printf
 *
 * Specify a format string and the values just like in printf
 *
 * Usage:
 *  #c: char
 *  #i: integer
 *  #l: long
 *  #L: long long
 *  #s: string
 *  ##: '#'
 */
void print(char * fmt, ...);

/*
 * print_num: print numbers
 *
 * This will print a number 'n' on the screen
 */
void print_num(long long n);


//---------------------------------+
// Helpers for the print functions |
//---------------------------------+

/*
 * len_str: Returns the length of string 'str'
 */
unsigned int len_str(const char * str);

/*
 * len_num: returns the number of digits in 'n'
 *
 * '-' sign is not counted as a digit
 */
unsigned int len_num(long long n);

/*
 * num2str: Converts a number to string
 */
char * num2str(long long n);

/*
 * str_set: replace str[begin..end] with characters from rep
 * end is not included
 */
void str_set(char * str, int begin, int end, const char * rep);

/*
 * append: append string b to a
 */
char * append(const char * a, const char * b);

/*
 * reverse: reverses a string
 */
void reverse(char *str);

/*
 * progress_bar: display a progress bar
 */
char * progress_bar(long long finished, long long total);

#endif
