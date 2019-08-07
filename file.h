#include <stdbool.h>
#include <sys/types.h>

#ifndef __FILE_H
#define __FILE_H

typedef struct {
    bool d;
    char u, g, o;
} perm;

/*
 * exists: checks if given file / dir exists
 */
bool exists(const char * path);

/*
 * get_size: returns size of the file at 'path'
 *
 * returns -1 if file doesn't exist
 */
off_t get_size(const char * path);

/*
 * perm: returns the permissions on the file / dir at 'path'
 *       as a struct
 *
 *       .u, .g, .o: perms for user, group and others
 *       .d: isDir?
 */
perm get_perm(const char * path);

void print_perm(perm p);

/*
 * get_filename: returns the name of file
 *  [useful when path was specified instead of the name of the file]
 */
char * get_filename(const char * path);

/*
 * rev_copy: copy contents of file a to file b, in reverse
 *  fd_{a, b}: file descriptor for file {a, b}
 *  bs: block size chunks in which the file is copied
 *  progress: whether to show the progress bar or not
 */
void rev_copy(int fd_a, int fd_b, int bs, bool progress);

#endif
