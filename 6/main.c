#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const size_t SIZE = 5000;
char buf[SIZE + 1];
char buf2[SIZE + 1];
char buf3[SIZE + 1];

int is_good(char ch) {
    if (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A') {
        return 1;
    } else {
        return 0;
    }
}

void reverse(int a, int b)
{
    while (a < b) {
        char temp = buf2[a];
        buf2[a] = buf2[b];
        buf2[b] = temp;
        ++a; --b;
    }
}

void reversewords(int length)
{
    int left = 0;
    while (left < length) {
        if (is_good(buf2[left] == 0)) {
            ++left;
            continue;
        }
        int right = left + 1;
        while (right < length && is_good(buf2[right]) == 1) {
            ++right;
        }
        reverse(left, right - 1);
        left = right + 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of parameters. You should enter name of input file and output file\n");
        return 0;
    }
    int in = open(argv[1], O_RDONLY);
    int bytes_read, bytes_read2, bytes_read3;

    if (in == -1) {
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }


    int fd1[2];
    if (pipe(fd1) != 0) {
        fprintf(stderr, "Cannot create pipe1.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);
        if (write(fd1[1], buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        sleep(1);
        int out = open(argv[2], O_RDWR | O_CREAT, 0666);
        if (out == -1) {
            fprintf(stderr, "Cannot open output file.\n");
            exit(0);
        }
        bytes_read3 = read(fd1[0], &buf3, SIZE);
        write(out, buf3, bytes_read3);
        close(fd1[1]);
        close(fd1[0]);
        close(out);
        exit(0);
    }

    close(in);

    if (fork() == 0) {
        bytes_read2 = read(fd1[0], &buf2, SIZE);
        reversewords(bytes_read2);
        if (write(fd1[1], buf2, bytes_read2) == -1) {
            fprintf(stderr, "Cannot write in pipe2.\n");
        }
        close(fd1[1]);
        close(fd1[0]);
        exit(0);
    } else {
        wait(NULL);
    }
    sleep(1);
    return 0;
}