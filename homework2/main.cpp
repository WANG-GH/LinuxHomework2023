#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(const char* path) {
    struct stat info;
    if (stat(path, &info) != 0) {
        std::cerr << "Error: unable to stat " << path << '\n';
        return;
    }

    // Print file type
    std::cout << (S_ISDIR(info.st_mode) ? 'd' : '-');

    // Print permissions
    std::cout << ((info.st_mode & S_IRUSR) ? 'r' : '-')
              << ((info.st_mode & S_IWUSR) ? 'w' : '-')
              << ((info.st_mode & S_IXUSR) ? 'x' : '-')
              << ((info.st_mode & S_IRGRP) ? 'r' : '-')
              << ((info.st_mode & S_IWGRP) ? 'w' : '-')
              << ((info.st_mode & S_IXGRP) ? 'x' : '-')
              << ((info.st_mode & S_IROTH) ? 'r' : '-')
              << ((info.st_mode & S_IWOTH) ? 'w' : '-')
              << ((info.st_mode & S_IXOTH) ? 'x' : '-');

    // Print size
    std::cout << ' ' << std::setw(6) << info.st_size;

    // Print name
    std::cout << ' ' << path;

    std::cout << '\n';
}

int main(int argc, char** argv) {
    const char* dir_path = (argc > 1) ? argv[1] : ".";

    DIR* dir = opendir(dir_path);
    if (!dir) {
        std::cerr << "Error: unable to open directory " << dir_path << '\n';
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        print_file_info(entry->d_name);
    }

    closedir(dir);
    
    return 0;
}
