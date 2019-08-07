#include <fcntl.h>
#include <stdlib.h>

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
    print("Directory is created? #s\n\n", exists(dir_name) ? "yes" : "no");

    // Are contents reversed?

    // permissions
    print("Permissions\n");

    perm file_perm = get_perm(dest),
         dir_perm = get_perm(argv[2]);

    print("File:\n");
    print_perm(file_perm);

    print("Dir:\n");
    print_perm(dir_perm);

    free(dest_with_slash);
    free(filename);
    free(dest);
}
