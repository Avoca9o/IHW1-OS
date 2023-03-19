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

// функция проверки символа на то является он буквой или нет
int is_good(char ch) {
    if (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A') {
        return 1;
    } else {
        return 0;
    }
}

// функция переворота одного слова по заданным границам
void reverse(int a, int b)
{
    while (a < b) {
        char temp = buf2[a];
        buf2[a] = buf2[b];
        buf2[b] = temp;
        ++a; --b;
    }
}

// функция переворота всех слов в тексте
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
    if (argc != 3) { // проверка корректности числа введенных аргументов
        fprintf(stderr, "Wrong number of parameters. You should enter name of input file and output file\n");
        return 0;
    }
    int in = open(argv[1], O_RDONLY);   // открываем входной файл
    int bytes_read, bytes_read2, bytes_read3;

    if (in == -1) {         // проверка корректности открытия входного файла
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }

    char* name1 = "/tmp/fifo1";     // имя именованного канала
    mkfifo("/tmp/fifo1", 0666);     // привязка именованного канала к программе

    int fd1 = open(name1, O_RDWR | O_CREAT, 0666);  // открытие именованного канала

    if (fork() == 0) {
        bytes_read = read(in, &buf, SIZE);          // первый дочерний поток - сначала считывает данные
        if (write(fd1, buf, bytes_read) == -1) {    // и передает их второму через именованный канал
            fprintf(stderr, "Cannot write in pipe1.\n");
        }
        sleep(1);
        int out = open(argv[2], O_RDWR | O_CREAT, 0666);    // затем считывает полученные от второго 
        if (out == -1) {                    // потока данные и записывает их в файл
            fprintf(stderr, "Cannot open output file.\n");
            exit(0);
        }
        bytes_read3 = read(fd1, &buf3, SIZE);
        write(out, buf3, bytes_read3);
        close(fd1);
        close(out);
        exit(0);
    }

    close(in);

    if (fork() == 0) {
        bytes_read2 = read(fd1, &buf2, SIZE);   // второй дочерний поток - получает через именованный канал 
        reversewords(bytes_read2);      // данные от первого, обрабатывает их и возвращает через тот же 
        if (write(fd1, buf2, bytes_read2) == -1) {      // именованный канал
            fprintf(stderr, "Cannot write in pipe2.\n");
        }
        close(fd1);
        exit(0);
    } else {
        wait(NULL);
    }   
    unlink(name1);      // отвязываем именованный канал от программы
    wait(NULL);
    return 0;
}