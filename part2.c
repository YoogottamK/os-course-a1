#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// Utilities I wrote to help with this task
// Basically wrappers over syscalls
#include "utils.h"

// Some wrappers for file related syscalls
#include "file.h"

int main(int argc, char ** argv) {
    // Check if the user provided path or not
    if(argc != 4) {
        print("Usage: #s 'path_to_rev_file' 'path_to_orig_file' 'path_to_dir' \n", argv[0]);

        return 1;
    }

    char * rev_path = argv[1],
        *orig_path = argv[2],
        *dir_name = argv[3];

    // Try to open the file
    int in_fd = open(orig_path, O_RDONLY);

    if(in_fd == -1) {
        print("Some error occured while opening '#s': ", orig_path);
        perror("¯\\_(ツ)_/¯.");

        return 2;
    }

    if(!is_file(rev_path)) {
        print("The 'file' #s doesn't look like one\n", rev_path);
        return 2;
    }

    if(!is_file(orig_path)) {
        print("The 'file' #s doesn't look like one\n", rev_path);
        return 2;
    }

    if(!is_dir(dir_name)) {
        print("The 'dir' #s doesn't look like one\n", dir_name);
        return 3;
    }

    // Does the directory exist?
    print("Was the directory created? #s\n", exists(dir_name) ? "yes" : "no");

    // Are contents reversed?
    print("Checking file content...");

    char * tmpfile = uniq_file();
    int rev_fd = open(rev_path, O_RDONLY);

    if(rev_fd == -1) {
        print("Some error occured while opening '#s': ", rev_path);
        perror("¯\\_(ツ)_/¯.");

        return 2;
    }

    print("\rWere the contents of the file reversed? #s\n", check_rev(in_fd, rev_fd) ? "yes" : "no");

    // permissions
    print("\nPermissions\n");

    perm rev_perm = get_perm(rev_path),
         orig_perm = get_perm(orig_path),
         dir_perm = get_perm(dir_name);

    print("Original File:\n");
    print_perm(orig_perm);

    print("Reversed File:\n");
    print_perm(rev_perm);

    print("Dir:\n");
    print_perm(dir_perm);

    // Cleanup
    unlink(tmpfile);

    close(in_fd);
    close(rev_fd);

    free(tmpfile);
}
