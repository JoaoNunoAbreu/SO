#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFSIZE 10000

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

char** tokenizeArtigoDyn(char* artigo, int* tamanho) {
    
    char** artigos = (char**) malloc(3 * sizeof(char*));
    char* temp = strdup(artigo);
    char* token = strtok(temp," ");
    while(token != NULL){
        artigos[*tamanho] = strdup(token);
        token = strtok(NULL," ");
        *tamanho = *tamanho + 1;
    }
    return artigos;
}

int main(){ 

    int tamanho = 0;
    char** info;
    int fdFifo = open("FIFO", O_WRONLY);

    while(1){
        tamanho = 0;
        char *buf = malloc(BUFFSIZE);
        ssize_t n = readln(0,buf,BUFFSIZE);
        if (n <= 0) break;
        info = tokenizeArtigoDyn(buf,&tamanho);
        switch(tamanho){
            case 1:{
                int fdArtigos = open("ARTIGOS.txt",O_RDONLY, 0666);
                int fdStocks  = open("STOCKS.txt" ,O_RDONLY, 0666);

                char buffer1[BUFFSIZE];
                char buffer2[BUFFSIZE];

                size_t n1 = 0;
                size_t n2 = 0;

                for(int i = 0; i < atoi(info[0]); i++){
                    n1 = readln(fdArtigos,buffer1,sizeof buffer1);
                    n2 = readln(fdStocks ,buffer2,sizeof buffer2);
                    if(n1 <= 0) break;
                    if(n2 <= 0) break;
                }

                if(n1 == 0 || n2 == 0) {write(1,"ERRO\n",5);exit(1);}

                int tamanho1 = 0;
                int tamanho2 = 0;

                buffer1[n1] = '\0';
                buffer2[n2] = '\0';

                char** info1 = tokenizeArtigoDyn(buffer1,&tamanho1);
                char** info2 = tokenizeArtigoDyn(buffer2,&tamanho2);

                char* preco = malloc(BUFFSIZE); strcat(strcat(preco,"O preço é: "),info1[2]);
                char* stock = malloc(BUFFSIZE); strcat(strcat(stock,"O stock é: "),info2[1]);

                write(1,preco,strlen(preco));
                write(1,stock,strlen(stock));

                close(fdArtigos);
                close(fdStocks);
                break;
            }
            case 2:{
                write(fdFifo,buf,n);
                
                break;
            }
            default: {write(1,"ERRO\n",5);}
        }
    }
    close(fdFifo);  
    
    return 0; 
} 