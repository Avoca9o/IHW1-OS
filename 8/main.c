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

int main(int argc, char* argv[]) {
    if (argc != 3) {        // проверка корректности числа введенных аргументов
        fprintf(stderr, "Wrong number of parameters. You should enter name of input file and output file\n");
        return 0;
    }
    int in = open(argv[1], O_RDONLY);
    int bytes_read, bytes_read2;
    int fd;
    char* name = "/tmp/fifo";
    mkfifo(name, 0666);         // привязка именованного канала к программе
    if ((fd = open(name, O_RDWR | O_CREAT, 0666)) == -1) {
        fprintf(stderr, "Can't open fifo\n");   // проверка корректности открытия
        return 0;
    }

    if (in == -1) {         // проверка корректности открытия входного файла
        fprintf(stderr, "Cannot open input file.\n");
        write(fd, "exit", strlen("exit"));
        unlink(name);
        close(fd);
        return 0;
    }

    if ((bytes_read = read(in, buf, SIZE)) == -1) { // попытка чтения данных из входного файла
        fprintf(stderr, "Cannot read from input file.\n");
        return 0;
    }


    if (write(fd, buf, bytes_read) == -1) {         // попытка передачи данных через именованный канал
        fprintf(stderr, "Can't write to fifo\n");
        return 0;
    }

    sleep(2);

    int out = open(argv[2], O_WRONLY | O_CREAT, 0666);  // попытка открытая выходного файла
    if (out == -1) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }

    if ((bytes_read2 = read(fd, buf2, SIZE)) == -1) {
        fprintf(stderr, "Can't read from fifo\n");  // попытка считывания данных из именованного канала
        return 0;
    }

    if (write(out, buf2, bytes_read2) == -1) {      // запись полученных данных в выходной файл
        fprintf(stderr, "Can't write to output file\n");
        return 0;
    }

    close(out);
    close(fd);
    return 0;
}