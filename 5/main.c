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

    char* name1 = "/tmp/fifo1";
    char* name2 = "/tmp/fifo2";
    mkfifo("/tmp/fifo1", 0666);
    mkfifo("/tmp/fifo2", 0666);
    int fd1 = open(name1, O_RDWR | O_CREAT);
    if (fd1 < 0) {
        fprintf(stderr, "Cannot open FIFO1.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);
        if (bytes_read == 0) {
            close(fd1);
            exit(0);
        }
        if (write(fd1, buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        close(fd1);
        exit(0);
    } else {
        wait(NULL);
    }

    if (bytes_read == 0) {
        close(fd1);
        unlink(name1);
        unlink(name2);
        exit(0);
    }

    close(in);
    int fd2 = open(name2, O_RDWR | O_CREAT);
    if (fd2 < 0) {
        fprintf(stderr, "Cannot open FIFO2.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read2 = read(fd1, &buf2, SIZE);
        reversewords(bytes_read2);
        if (write(fd2, buf2, bytes_read2) == -1) {
            fprintf(stderr, "Cannot write in pipe2.\n");
        }
        close(fd2);
        close(fd1);
        exit(0);
    } else {
        wait(NULL);
    }

    int out = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (out == -1) {
        fprintf(stderr, "Cannot open output file.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read3 = read(fd2, &buf3, SIZE);
        write(out, buf3, bytes_read3);
        close(fd1);
        close(fd2);
        exit(0);
    } else {
        wait(NULL);
    }
    close(fd1);
    close(fd2);

    unlink(name1);
    unlink(name2);
    close(out);
    return 0;
}