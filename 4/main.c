#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const size_t SIZE = 5000;   // размер буфера
char buf[SIZE + 1];         // буфера
char buf2[SIZE + 1];
char buf3[SIZE + 1];

// функция для проверки симовла на то, является ли он буквой
int is_good(char ch) {
    if (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A') {
        return 1;
    } else {
        return 0;
    }
}

// функция переворота одного слова в тексте
void reverse(int a, int b)
{
    while (a < b) {
        char temp = buf2[a];
        buf2[a] = buf2[b];
        buf2[b] = temp;
        ++a; --b;
    }
}

// функция обработки текста, переворачивающая каждое слово
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
    int in = open(argv[1], O_RDONLY);                       // открываем входной файл
    int bytes_read, bytes_read2, bytes_read3;

    if (in == -1) {                                         // проверка корректности открытия
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }


    int fd1[2];                                             // файловые дескрипторы для каналов
    int fd2[2];
    if (pipe(fd1) != 0) {                                   // проверка корректности создания канала
        fprintf(stderr, "Cannot create pipe1.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);                  // первый дочерний поток - делает свою работу
        if (write(fd1[1], buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        close(fd1[1]);
        close(fd1[0]);
        exit(0);
    } else {
        close(fd1[1]);                                       // родительский поток - ждет дочерний
        wait(NULL);
    }

    close(in);                                               // закрываем входной файл, он больше не нужен

    if (pipe(fd2) != 0) {                                   // проверка корректности создания второго канала
        fprintf(stderr, "Cannot create pipe2.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read2 = read(fd1[0], &buf2, SIZE);            // второй дочерний поток - делает свою работу
        close(fd1[0]);
        reversewords(bytes_read2);
        if (write(fd2[1], buf2, bytes_read2) == -1) {
            fprintf(stderr, "Cannot write in pipe2.\n");
        }
        close(fd2[1]);
        close(fd2[0]);
        exit(0);
    } else {
        close(fd1[0]);                                      // родительский поток ждет
        close(fd2[1]);
        wait(NULL);
    }

    int out = open(argv[2], O_RDWR | O_CREAT, 0666);        // открываем выходной файл
    if (out == -1) {                                        // проверка на корректность открытия
        fprintf(stderr, "Cannot open output file.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read3 = read(fd2[0], &buf3, SIZE);            // третий дочерний поток со своими функциями
        write(out, buf3, bytes_read3);
        close(fd2[0]);
        exit(0);
    } else {
        close(fd2[0]);
        wait(NULL);
    }

    close(out);
    return 0;
}