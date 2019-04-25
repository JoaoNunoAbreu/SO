#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

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

char* concat(const char *s1, const char *s2){

    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
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

void addSemRep(int v[], int k){

    int i;
    for(i = 0; v[i]; i++){
        if(v[i] == k) break;
    }
    if(!v[i]) {
        v[i] = k;
        v[i+1] = '\0';
    }
}

int main(){

    int fd1  = open("VENDAS.txt", O_RDONLY, 0666);
    int codArtigos[BUFFSIZE];
    int tamanho1,tamanho2;
    int sumStock = 0; int sumMontante = 0;

    while(1){ // Percorre o ficheiro de vendas para ver quantos códigos de artigo há
        tamanho1 = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(fd1,buffer,sizeof buffer);
        if(n <= 0) break;
        char** info = tokenizeArtigoDyn(buffer,&tamanho1);
        addSemRep(codArtigos,atoi(info[0]));
    }

    int fd2  = open("VENDAS.txt", O_RDONLY, 0666);
    for(int i = 0; codArtigos[i]; i++){ // Volta a percorrer o ficheiro de vendas para agregar os dadoss

        while(1){
            tamanho2 = 0;
            char buffer[BUFFSIZE];
            size_t n = readln(fd2,buffer,sizeof buffer);
            if(n <= 0) break;
            char** info = tokenizeArtigoDyn(buffer,&tamanho2);
            if(atoi(info[0]) == codArtigos[i]){
                sumStock += atoi(info[1]);
                sumMontante += atoi(info[2]);
            }
        }

        char* codigoStr   = malloc(BUFFSIZE); sprintf(codigoStr,"%d",codArtigos[i]);
        char* stockStr    = malloc(BUFFSIZE); sprintf(stockStr,"%d",sumStock);
        char* montanteStr = malloc(BUFFSIZE); sprintf(montanteStr,"%d\n",sumMontante);
        write(1,concat(concat(codigoStr,stockStr),montanteStr),strlen(concat(concat(codigoStr,stockStr),montanteStr)));
    }
        
    return 0;
}