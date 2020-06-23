//
// Created by hhhxiao on 5/28/20.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char *argv[]) {
    if (argc != 4 && argc != 3) {
        printf("usage [fileName] [size(KB)]\n or [fileName] [size(KB)] [seed]");
        return 0;
    }
    long seed = argc == 4 ? atoi(argv[3]) : time(NULL);
    srand(seed);
    FILE *fp = fopen(argv[1], "wb");
    long size = 1024;
    size *= atoi(argv[2]);
    while (size-- > 0)
        fputc(rand() % (128 - '0') + '0', fp);
    fclose(fp);
}

