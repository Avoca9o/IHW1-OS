#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const size_t SIZE = 5000; // размер буфера
char buf[SIZE + 1];       // буфера
char buf2[SIZE + 1];
char buf3[SIZE + 1];

// функция проверки символа на то, является ли он буквой
int is_good(char ch) {
    if (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A') {
        return 1;
    } else {
        return 0;
    }
}

// функция, переворачивающая одно слова в тексте по заданным границам
void reverse(int a, int b)
{
    while (a < b) {
        char temp = buf2[a];
        buf2[a] = buf2[b];
        buf2[b] = temp;
        ++a; --b;
    }
}

// функция, переворачивающая все слова в тексте
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
    if (argc != 3) {        // проверка корректности числа введенных аргументов
        fprintf(stderr, "Wrong number of parameters. You should enter name of input file and output file\n");
        return 0;
    }
    int in = open(argv[1], O_RDONLY);           // открываем входной файл для чтения
    int bytes_read, bytes_read2, bytes_read3;

    if (in == -1) {                             // проверка корректности второго файла
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }


    int fd1[2];                 // файловый дескриптор для канала
    if (pipe(fd1) != 0) {       // попытка создать неименованный канал
        fprintf(stderr, "Cannot create pipe1.\n");
        return 0;
    }

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);                  // сначала первый дочерний процес считывает данные
        if (write(fd1[1], buf, bytes_read) == -1) {
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        sleep(1);
        int out = open(argv[2], O_RDWR | O_CREAT, 0666);    // затем получает информацию от второго и 
        if (out == -1) {                                    // записывает ее в выходной файл
            fprintf(stderr, "Cannot open output file.\n");  // проверка корректности открытия файла
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
        bytes_read2 = read(fd1[0], &buf2, SIZE);            // второй дочерний процесс считывает данные из  
        reversewords(bytes_read2);                          // неименованного канала и обрабатывает их
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