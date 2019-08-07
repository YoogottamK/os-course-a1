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

    print("\nDone\n");
    free(block);
}
