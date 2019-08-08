#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/ioctl.h>

#include <stdio.h>

#include "utils.h"

unsigned int len_str(const char * str) {
    unsigned int l;
    for(l=0; str[l]; l++);

    return l;
}

unsigned int len_num(long long n) {
    if(!n) return 1;
    if(n < 0) n = -n;

    unsigned int l;
    for(l=0; n; n/=10, l++);

    return l;
}

char * num2str(long long n) {
    unsigned int l = len_num(n);
    int neg = (n < 0);
    l += neg;

    if(neg)
        n = -n;

    char * str = (char *) malloc(l + 1);
    str[l] = 0;

    for(; l; n /= 10) str[--l] = '0' + (n % 10);

    if(neg) str[0] = '-';

    return str;
}

void str_set(char * str, int begin, int end, const char * rep) {
    for(int i = begin; i < end; i++)
        str[i] = rep[i - begin];
}

void print_num(long long n) {
    char * str_n = num2str(n);

    write(1, str_n, len_str(str_n));
}

char * append(const char * a, const char * b) {
    int l_a = len_str(a),
        l_b = len_str(b);

    char * ret = (char *) malloc(l_a + l_b + 1);
    int i = 0;

    for(; i < l_a; i++)
        ret[i] = a[i];

    for(; i < l_a + l_b; i++)
        ret[i] = b[i - l_a];

    ret[i] = 0;

    return ret;
}

void reverse(char * str) {
    int r = len_str(str) - 1, l = 0;
    char c;

    while(l < r) {
        c = str[l];
        str[l] = str[r];
        str[r] = c;

        l++; r--;
    }
}

bool str_eq(const char * a, const char * b) {
    for(int i = 0; a[i] && b[i]; i++)
        if(a[i] != b[i])
            return false;

    return true;
}

void print(char * fmt, ...) {
    char * to_print = (char *) malloc(len_str(fmt) + 1), *ch = fmt;
    int l = 0;

    int fmt_i;
    long fmt_l;
    long long fmt_L;
    char * fmt_s;

    va_list arg;
    va_start(arg, fmt);

    while(*ch) {
        while(*ch != '#') {
            to_print[l++] = *ch;
            ch++;

            if(*ch == 0) {
                to_print[l++] = 0;
                write(1, to_print, l);

                free(to_print);
                va_end(arg);
                return;
            }
        }

        write(1, to_print, l);
        l = 0;
        ch++;

        switch(*ch) {
            case 'c':
                fmt_i = va_arg(arg, int);
                write(1, &fmt_i, 1);
                break;
            case 'i':
                fmt_i = va_arg(arg, int);
                print_num(fmt_i);
                break;
            case 'l':
                fmt_l = va_arg(arg, long);
                print_num(fmt_l);
                break;
            case 'L':
                fmt_L = va_arg(arg, long long);
                print_num(fmt_L);
                break;
            case 's':
                fmt_s = va_arg(arg, char *);
                write(1, fmt_s, len_str(fmt_s));
                break;
            case '#':
                write(1, "#", 1);
                break;
            case 0:
                free(to_print);
                va_end(arg);
                return;
            default:
                write(1, ch, 1);
                break;
        }

        ch++;
    }

    free(to_print);
    va_end(arg);
}

char * progress_bar(long long finished, long long total) {
    struct winsize w;
    ioctl(2, TIOCGWINSZ, &w);
    long long width = w.ws_col, percentage;

    percentage = (finished * 100) / total;
    if (percentage > 100)
        percentage = 100;

    char * pre = "Progress: [",
         *percent_str = num2str(percentage),
         *percentext = append(percent_str, "%"),
         *post = append("] ", percentext),
         *bar;

    width -= (len_str(pre) + len_str(post));
    long long len_col = (width * percentage) / 100,
         len_nocolor = width - len_col;

    bar = (char*) malloc(len_col + len_nocolor + 9);

    bar[len_col + len_nocolor + 8] = 0;

    str_set(bar, 0, 4, "\033[7m");
    str_set(bar, len_col + 4, len_col + 8, "\033[0m");

    for(int i = 4; i < len_col + 4; i++)
        bar[i] = ' ';
    for(int i = len_col + 8; i < len_col + len_nocolor + 8; i++)
        bar[i] = ' ';

    char *bar_with_progress  = append(pre, bar),
         *ret = append(bar_with_progress, post);

    free(percent_str);
    free(percentext);
    free(post);
    free(bar);
    free(bar_with_progress);

    return ret;
}
