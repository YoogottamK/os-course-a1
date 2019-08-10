#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file.h"
#include "utils.h"

bool exists(const char * path) {
    struct stat st;
    return stat(path, &st) != -1;
}

off_t get_size(const char * path) {
    struct stat st;
    int ret = stat(path, &st);

    if(ret == -1)
        return -1;

    return st.st_size;
}

perm get_perm(const char * path) {
    perm perms = { .u = 0, .g = 0, .o = 0, .d = 0 };
    struct stat st;

    if(stat(path, &st) == -1) {
        perms.u = perms.g = perms.o = -1;
        return perms;
    }

    perms.u = 4 * !!(st.st_mode & S_IRUSR) +
              2 * !!(st.st_mode & S_IWUSR) +
              !!(st.st_mode & S_IXUSR);
    perms.g = 4 * !!(st.st_mode & S_IRGRP) +
              2 * !!(st.st_mode & S_IWGRP) +
              !!(st.st_mode & S_IXGRP);
    perms.o = 4 * !!(st.st_mode & S_IROTH) +
              2 * !!(st.st_mode & S_IWOTH) +
              !!(st.st_mode & S_IXOTH);

    perms.d = S_ISDIR(st.st_mode);

    return perms;
}

void print_perm(perm p) {
    char *on_fg = "\033[32m",
         *off_fg = "\033[31m",
         *col_r = !!(p.u & 4) ? on_fg : off_fg,
         *col_w = !!(p.u & 2) ? on_fg : off_fg,
         *col_x = !!(p.u & 1) ? on_fg : off_fg;

    print("\tUser:\t");
    print("\t#sr\033[0m#sw\033[0m#sx\033[0m\n", col_r, col_w, col_x);

    col_r = !!(p.g & 4) ? on_fg : off_fg;
    col_w = !!(p.g & 2) ? on_fg : off_fg;
    col_x = !!(p.g & 1) ? on_fg : off_fg;

    print("\tGroup:\t");
    print("\t#sr\033[0m#sw\033[0m#sx\033[0m\n", col_r, col_w, col_x);

    col_r = !!(p.o & 4) ? on_fg : off_fg;
    col_w = !!(p.o & 2) ? on_fg : off_fg;
    col_x = !!(p.o & 1) ? on_fg : off_fg;

    print("\tOther:\t");
    print("\t#sr\033[0m#sw\033[0m#sx\033[0m\n", col_r, col_w, col_x);
}

char * get_filename(const char * path) {
    int l = len_str(path), i = l - 1, j;
    char * name = (char *) malloc(l + 1);

    while(i >= 0 && path[i] != '/')
        i--;

    i++;

    for(j = 0; path[j + i]; j++)
        name[j] = path[j + i];

    name[j] = 0;

    return name;
}

void rev_copy(int fd_a, int fd_b, int bs) {
    off_t sz = lseek(fd_a, 0, SEEK_END);
    long long total_bytes = sz, bytes_copied = 0;

    char * block = (char*) malloc(bs + 1),
         *bar;

    // if whatever's left is less than block size
    if(sz < bs) bs = sz;

    sz = lseek(fd_a, -bs, SEEK_END);

    while(sz >= 0) {
        read(fd_a, block, bs);
        block[bs] = 0;

        bytes_copied += bs;

        reverse(block);

        write(fd_b, block, bs);

        bar = progress_bar(bytes_copied, total_bytes);
        print("#s\r", bar);

        if(!sz)
            break;
        else if(sz < bs) {
            bs = sz;
            sz = lseek(fd_a, 0, SEEK_SET);
        } else
            sz = lseek(fd_a, -2 * bs, SEEK_CUR);
    }

    print("\nDone\n");

    free(bar);
    free(block);
}

bool check_rev(int fd_a, int fd_b) {
    struct stat st_a, st_b;
    fstat(fd_a, &st_a);
    fstat(fd_b, &st_b);

    if(st_a.st_size != st_b.st_size)
        return false;

    off_t bs = st_a.st_size / 100;
    if(!bs) bs = 1;
    if(bs > 1e7) bs = 1e7;

    char *buf_a = (char*) malloc(bs + 1),
         *buf_b = (char*)  malloc(bs + 1);

    if(bs > st_a.st_size)
        bs = st_a.st_size;

    off_t rev_offset = lseek(fd_b, -bs, SEEK_END);

    while(rev_offset >= 0) {
        read(fd_a, buf_a, bs);
        read(fd_b, buf_b, bs);

        buf_a[bs] = 0;
        buf_b[bs] = 0;

        reverse(buf_b);

        if(!str_eq(buf_a, buf_b)) {
            free(buf_a);
            free(buf_b);

            return false;
        }

        if(!rev_offset)
            break;
        else if(rev_offset < bs) {
            bs = rev_offset;
            rev_offset = lseek(fd_b, 0, SEEK_SET);
        } else
            rev_offset = lseek(fd_b, -2 * bs, SEEK_CUR);
    }

    free(buf_a);
    free(buf_b);

    return true;
}

char * uniq_file() {
    int num = 0, len_name = 10;
    char * name = (char*) malloc(len_name + 1);
    name[len_name] = 0;

    str_set(name, 0, len_name, "tmp_______");

    while(exists(name)) {
        num++;
        char * num_str = num2str(num);
        int len = len_str(num_str);

        str_set(name, len_name - len, len_name, num_str);
        free(num_str);
    }

    return name;
}
