#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t readln(int fildes, void *buf, size_t nbyte){

    char *b = buf;
    size_t i = 0;

    while(i < nbyte){
        ssize_t n = read(fildes,&b[i],1);
        if (n <= 0) break;
        if(b[i] == '\n') return (i+1);
        i++;
    }
    return i;
}

int main(){

    int BUFFSIZE = 10000;
    int fd = open("ARTIGOS.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
    char *buf = malloc(BUFFSIZE);
    int line = 1;
    if(fd == -1){write(1,"ERRO\n",5); exit(1);}

    while(1){
        int pre = sprintf(buf,"%d ",line);
        ssize_t n = readln(0,buf+pre,BUFFSIZE-pre); // Código numérico + linha lida
        if (n <= 0) break;
        if(buf[pre] == 'i'){
            write(fd,buf,n+pre);
            line++;
        }
    }
    return 0;
}