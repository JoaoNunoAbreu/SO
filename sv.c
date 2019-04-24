#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFSIZE 10000

int main(){
    mkfifo("FIFO",0666);
    int fdStock = open("STOCKS.txt", O_CREAT | O_TRUNC | O_WRONLY , 0666);

    while(1){
        int fd = open("FIFO", O_RDONLY); // Abre o FIFO para ler
        char buf[BUFFSIZE];
        while(1){
            int n = read(fd,buf,sizeof buf); // Lê do FIFO e guarda no array 
            if(n <= 0) break;
            write(fdStock,buf,n);
        }
        close(fd);
    }
    return 0;
}