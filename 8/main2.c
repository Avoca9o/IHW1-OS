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

int main() {
    char* name = "/tmp/fifo";
    sleep(1);
    int bytes_read;
    mkfifo(name, 0666);
    int fd = open(name, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        fprintf(stderr, "Can't open  fifo\n");
        return 0;
    }

    if ((bytes_read = read(fd, &buf, SIZE)) == -1 || strcmp(buf, "exit") == 1) {
        fprintf(stderr, "Can't read fifo\n");
        close(fd);
        unlink(name);
        return 0;
    }
    reversewords(bytes_read);
    if (write(fd, buf, bytes_read) == -1) {
        fprintf(stderr, "Can't write fifo\n");
        return 0;
    }
    close(fd);
    unlink(name);
    return 0;
}