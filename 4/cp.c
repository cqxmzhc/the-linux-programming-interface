#include "../tlpi-dist/lib/tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_READ 1024

int main (int argc, char *argv[])
{
    int rFd, wFd;
    ssize_t numRead, numWrite;
    char *buf;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s srcFile destFile", argv[0]);
    }

    rFd = open(argv[1], O_RDWR);
    if (rFd == -1) {
        errExit("open");
    }

    buf = malloc(MAX_READ);
    if (buf == NULL) {
        errExit("malloc");
    }

    wFd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (wFd == -1) {
        errExit("open");
    }

    while ((numRead = read(rFd, buf, MAX_READ)) > 0) {
        ssize_t i;
        for (i = 0; i < numRead; ++i) {
            if (buf[i] == '\0') {
                if (lseek(wFd, 1, SEEK_CUR) == -1) {
                    errExit("lseek");
                }
            }
            else {
                numWrite = write(wFd, &buf[i], 1);
                if (numWrite == -1) {
                    errExit("write");
                }
            }
        }
    }
    free(buf);
}

