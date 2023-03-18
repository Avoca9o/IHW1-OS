#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const size_t SIZE = 5000;
char buf[SIZE + 1];

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
        char temp = buf[a];
        buf[a] = buf[b];
        buf[b] = temp;
        ++a; --b;
    }
}

void reversewords(int length)
{
    int left = 0;
    while (left < length) {
        if (is_good(buf[left] == 0)) {
            ++left;
            continue;
        }
        int right = left + 1;
        while (right < length && is_good(buf[right]) == 1) {
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
    int bytes_read;

    if (in == -1) {
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }


    int fd1[2];
    int fd2[2];
    if (pipe(fd1) != 0) {
        fprintf(stderr, "Cannot create pipe1.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);
        if (write(fd1[1], buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        close(fd1[1]);
        close(fd1[0]);
        exit(0);
    } else {
        close(fd1[1]);
        wait(NULL);
    }

    close(in);

    if (pipe(fd2) != 0) {
        fprintf(stderr, "Cannot create pipe2.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(fd1[0], &buf, bytes_read);
        close(fd1[0]);
        reversewords(bytes_read);
        if (write(fd2[1], buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe2.\n");
        }
        close(fd2[1]);
        close(fd2[0]);
        exit(0);
    } else {
        close(fd1[0]);
        close(fd2[1]);
        wait(NULL);
    }

    int out = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (out == -1) {
        fprintf(stderr, "Cannot open output file.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(fd2[0], &buf, bytes_read);
        write(out, buf, bytes_read);
        close(fd2[0]);
        exit(0);
    } else {
        close(fd2[0]);
        wait(NULL);
    }

    close(out);
    return 0;
}