#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFFSIZE 10000

long strToInt(char* str){

    char *ptr;
    long ret;
    ret = strtol(str, &ptr, 10);

    return ret;
}

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

char* concat(const char *s1, const char *s2){

    char *result = malloc(strlen(s1) + strlen(s2) + 2); // +2 para o espaço e para o fim
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

char* somador(char* cod, char* new){

    int tamanho = 0;

    int fPtr  = open("STOCKS.txt",  O_CREAT | O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY, 0666); 

    if(fPtr == -1 || fTemp == -1) {write(1,"Não conseguiu abrir\n",19); exit(1);}

    int jaSubstituiu = 0;
    char** info;
    char* resultado = NULL;

    while(1){
        tamanho = 0;
        char buffer[BUFFSIZE];
        char* newline = malloc(BUFFSIZE);
        size_t n = readln(fPtr,buffer,sizeof buffer);
        if(n <= 0){
            if(jaSubstituiu == 0) {
                char* novo = concat(cod,new);
                write(fTemp,novo,strlen(novo));
                resultado = strdup(new);
            }
            break;
        }
        info = tokenizeArtigoDyn(buffer,&tamanho);
        if(!strcmp(info[0],cod) && jaSubstituiu == 0){
            long res = strToInt(info[1]) + strToInt(new);
            sprintf(info[1],"%lu\n",res);
            resultado = strdup(info[1]);
            newline = concat(info[0],info[1]);
            write(fTemp,newline,strlen(newline));
            jaSubstituiu = 1;
        }
        else write(fTemp,buffer,n);
    }
    remove("STOCKS.txt");
    rename("replace.tmp", "STOCKS.txt");
    return resultado;
}

int main(){

    mkfifo("cv_sv",0666);
    mkfifo("sv_cv",0666);
    int tamanho;

    while(1){
        int cv_sv = open("cv_sv", O_RDONLY);
        int sv_cv = open("sv_cv", O_WRONLY);
        char buf[BUFFSIZE];
        while(1){
            int n = read(cv_sv,buf,sizeof buf); 
            if(n <= 0) break;
            tamanho = 0;
            char** info = tokenizeArtigoDyn(buf,&tamanho);
            if(tamanho == 2){
                char* res = somador(info[0],info[1]);
                write(sv_cv,res,strlen(res));
            }
            else write(1,"ERRO\n",5);
        }
        close(cv_sv);
        close(sv_cv);
    }
    return 0;
}