#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

// Utilities I wrote to help with this task
// Basically wrappers over syscalls
#include "utils.h"

// Some wrappers for file related syscalls
#include "file.h"

int main(int argc, char ** argv) {
    // Check if the user provided path or not
    if(argc != 3) {
        print("Usage: #s 'path_to_file' 'dest_dir'\n", argv[0]);

        return 1;
    }

    // Try to open the file
    int in_fd = open(argv[1], O_RDONLY);
    if(in_fd == -1) {
        print("Some error occured while opening #s\n", argv[1]);

        return 2;
    }

    char * filename = get_filename(argv[1]),
          *dir_name = argv[2];

    // these intermediate variables are used to
    // prevent any memory leaks. Each of them are free'd
    // at the end
    char * dest_with_slash = append(dir_name, "/"),
         *dest = append(dest_with_slash, filename);

    // Does the directory exist?
    print("Directory is created? #s\n", exists(dir_name) ? "yes" : "no");

    // Are contents reversed?
    char * tmpfile = uniq_file();
    int chk_fd = open(tmpfile, O_CREAT | O_TRUNC | O_RDWR),
        rev_fd = open(dest, O_RDONLY);

    // reverse the reversed contents again to check
    // if it is same as the original file
    rev_copy(rev_fd, chk_fd, 1e6, 0);

    print("Contents reversed? #s\n", diff_file(in_fd, chk_fd) ? "no" : "yes");

    // permissions
    print("\nPermissions\n");

    perm file_perm = get_perm(dest),
         dir_perm = get_perm(argv[2]);

    print("File:\n");
    print_perm(file_perm);

    print("Dir:\n");
    print_perm(dir_perm);

    unlink(tmpfile);

    free(tmpfile);
    free(dest_with_slash);
    free(filename);
    free(dest);
}
