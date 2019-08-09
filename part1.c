#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// Utilities I wrote to help with this task
// Basically wrappers over syscalls
#include "utils.h"

// Some wrappers for file related syscalls
#include "file.h"

int main(int argc, char ** argv) {
    // Check if the user provided path or not
    if(argc != 2) {
        print("Usage: #s 'path_to_file'\n", argv[0]);

        return 1;
    }

    // Try to open the file
    int in_fd = open(argv[1], O_RDONLY);
    if(in_fd == -1) {
        print("Some error occured while opening #s\n", argv[1]);

        return 2;
    }

    char * filename = get_filename(argv[1]),
          *dir_name = "Assignment";

    // Does the directory already exist?
    if(!exists(dir_name)) {
        // no? ok trying to create it
        if(mkdir(dir_name, 0700) == -1) {
            // oops
            print("There was some error in creating directory #s", dir_name);

            return 3;
        }
    }

    // these intermediate variables are used to
    // prevent any memory leaks. Each of them are free'd
    // at the end
    char * dest_with_slash = append(dir_name, "/"),
         *dest = append(dest_with_slash, filename);

    // create the dest file
    int rev_fd = open(dest, O_CREAT | O_TRUNC | O_WRONLY, 0600);

    off_t bs = get_size(argv[1]) / 100;
    if(!bs) bs = 1;
    if(bs > 1e6) bs = 1e6;

    // reverse copy contents of in_fd to rev_fd
    rev_copy(in_fd, rev_fd, bs, 1);

    close(in_fd);
    close(rev_fd);

    free(dest_with_slash);
    free(filename);
    free(dest);

    return 0;
}
