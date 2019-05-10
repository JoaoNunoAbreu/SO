#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#define BUFFSIZE 1000

ssize_t readln(int fildes, void *buf, size_t nbyte);
char* concat(const char *s1, const char *s2);
int contaLinhas(char* file);
char* removeEnter(char* str);