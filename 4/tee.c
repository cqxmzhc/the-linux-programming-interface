#include <fcntl.h>
#include <ctype.h>
#include "/home/zhangbin/Downloads/tlpi-dist/lib/tlpi_hdr.h"

#define MAX_READ 1024

int readFrom (int fd, char *buf, size_t len) {
    while (1) {
        ssize_t numRead, numWrite;
        numRead = read(fd, buf, len);
        if (numRead == -1) {
            printf("%s\n", "read failed");
            return 0;
        }
        size_t i;
        for (i = 0; i < (size_t)numRead; ++i) {
            numWrite = write(STDOUT_FILENO, buf, len);
            if (numWrite == -1) {
                printf("%s\n", "write failed");
                return 0;
            }
        }
    }
}

int main (int argc, char *argv[])
{
    int fd;
    ssize_t  numRead, numWrite;
    int openFlags = O_CREAT | O_WRONLY | O_APPEND;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                      S_IROTH |S_IWOTH;
    char *buf;
    buf = malloc(MAX_READ);
    if (buf == NULL) {
        printf("%s\n", "mallocate failed");
        return 0;
    }
    if (argc == 1) {
       readFrom(fd, buf, MAX_READ);
    }
    else if (argc == 2) {
       fd = open(argv[1], openFlags, filePerms);
       if (fd == -1) {
           printf("%s\n", "open failed");
           return 0;
       }
       while (1) {
            numRead = read(STDIN_FILENO, buf, MAX_READ);
            if (numRead == -1) {
                printf("%s\n", "read failed");
                return 0;
            }
            numWrite = write(fd, buf, numRead);
            if ( numWrite == -1) {
                printf("%s\n", "open failed");
                return 0;
            }
       }
    }
    return 0;
}
