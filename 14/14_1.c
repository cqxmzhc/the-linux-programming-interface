#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

//定义生成随机数的范围
#define MAX 900000
#define LEN 8
#define BUFFSIZE 1

//生成6位随机数
int get_rand() {
    int r;
    //生成种子
    r = 100000 + rand() % MAX; //获得6位数的种子
    return r;
}

char *get_filename() {
    char *filename;
    filename = malloc(LEN);
    filename[0] = 'x';
    filename[LEN - 1] = '\0';
    sprintf(filename+1, "%d", get_rand());
    return filename;
}

int main (int argc, char *argv[]) {
    time_t start, end;
    int fd;
    char *buff;
    buff = malloc(1);
    *buff = 't';

    //创建文件,开始计时
    srand(time(NULL));
    start = clock();
    int i;
    for (i = 0; i < atoi(argv[1]); i++) {
        fd = open(get_filename(), O_CREAT | O_WRONLY);
        if (fd > 0) {
            write(fd, buff, BUFFSIZE);
        }
    }
    end = clock();
    printf("start is %ld\n", start);
    printf("end is %ld\n", end);
    printf("cost %lfs to create %s files\n", (double)(end - start)/CLOCKS_PER_SEC, argv[1]);
    return 0;
}


