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
    print("\tUser:\t");
    print("\tr: #c\t", '0' + !!(p.u & 4));
    print("\tw: #c\t", '0' + !!(p.u & 2));
    print("\tx: #c\n", '0' + !!(p.u & 1));

    print("\tGroup:\t");
    print("\tr: #c\t", '0' + !!(p.g & 4));
    print("\tw: #c\t", '0' + !!(p.g & 2));
    print("\tx: #c\n", '0' + !!(p.g & 1));

    print("\tOther:\t");
    print("\tr: #c\t", '0' + !!(p.o & 4));
    print("\tw: #c\t", '0' + !!(p.o & 2));
    print("\tx: #c\n", '0' + !!(p.o & 1));
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

void rev_copy(int fd_a, int fd_b, int bs, bool progres) {
    off_t sz = lseek(fd_a, 0, SEEK_END);
    long long total_bytes = sz, bytes_copied = 0;

    char * block = (char*) malloc(bs + 1);

    // if whatever's left is less than block size
    if(sz < bs) bs = sz;

    sz = lseek(fd_a, -bs, SEEK_END);

    while(sz >= 0) {
        read(fd_a, block, bs);
        block[bs] = 0;

        bytes_copied += bs;

        reverse(block);
        write(fd_b, block, bs);

        if(progres) {
            char * bar = progress_bar(bytes_copied, total_bytes);
            print("#s\r", bar);
            free(bar);
        }

        if(!sz)
            break;
        else if(sz < bs) {
            bs = sz;
            sz = lseek(fd_a, 0, SEEK_SET);
        } else
            sz = lseek(fd_a, -2 * bs, SEEK_CUR);
    }

    if(progres)
        print("\nDone\n");

    free(block);
}

bool diff_file(int fd_a, int fd_b) {
    struct stat st_a, st_b;
    fstat(fd_a, &st_a);
    fstat(fd_b, &st_b);

    if(st_a.st_size != st_b.st_size)
        return true;


    off_t bs = 1e6, size = st_a.st_size, left = size;
    char *file_a = (char*) malloc(bs + 1),
         *file_b = (char*) malloc(bs + 1);

    while(left) {
        if(bs > left)
            bs = left;

        read(fd_a, file_a, bs);
        read(fd_b, file_b, bs);

        file_a[bs] = 0;
        file_b[bs] = 0;

        left -= bs;

        if(!str_eq(file_a, file_b))
            return true;
    }

    free(file_a);
    free(file_b);

    return false;
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
