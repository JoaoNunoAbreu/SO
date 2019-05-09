#include "Auxiliares.h"

char** tokenizeArtigoDyn(char* artigo, int* tamanho, int quantos) {
    
    char** artigos = (char**) malloc(quantos * sizeof(char*));
    char* temp = strdup(artigo);
    char* token = strtok(temp," ");
    while(token != NULL){
        artigos[*tamanho] = strdup(token);
        token = strtok(NULL," ");
        *tamanho = *tamanho + 1;
    }
    return artigos;
}

int main(int argc, char* argv[]){ 

    int tamanho;
    char** info;
    int contador = 0;
    int fd = 0;

    while(1){
        tamanho = 0;
        char buf[BUFFSIZE];
        ssize_t n = readln(fd,buf,BUFFSIZE);
        if (n <= 0) break;
        contador++;
        info = tokenizeArtigoDyn(buf,&tamanho,2);
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
                free(info[0]);

                if(n1 == 0) write(1,"O número que inseriu não é um código de um artigo.\n",55);
                else if(n2 == 0) write(1,"O código de artigo inserido não tem um stock associado.\n",58);
                else{
                    int tamanho1 = 0;
                    int tamanho2 = 0;

                    buffer1[n1] = '\0';
                    buffer2[n2] = '\0';

                    char** info1 = tokenizeArtigoDyn(buffer1,&tamanho1,2);
                    char** info2 = tokenizeArtigoDyn(buffer2,&tamanho2,2);

                    char* preco = malloc(BUFFSIZE); strcat(strcat(preco,"O preço é: "),info1[2]);
                    char* stock = malloc(BUFFSIZE); strcat(strcat(stock,"O stock é: "),info2[1]);
                    free(info1); free(info2);

                    write(1,preco,strlen(preco));
                    write(1,stock,strlen(stock));
                    free(preco); free(stock);
                }
                close(fdArtigos);
                close(fdStocks);
                break;
            }
            case 2:{
                int cv_sv = open("cv_sv", O_WRONLY);
                write(cv_sv,buf,n);
                close(cv_sv);

                int sv_cv = open("sv_cv", O_RDONLY);
                char buffer[BUFFSIZE];
                int n = read(sv_cv, buffer, sizeof buffer);
                write(1,buffer,n);
                close(sv_cv);
                break;
            }
            default: {write(1,"Erro no tamanho do cv\n",22);}
        }
    }
    return 0; 
}