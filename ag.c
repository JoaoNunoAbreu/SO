#include "Auxiliares.h"

char* removeEnter(char* str){
    char* p = str;
    if(p[strlen(p)-1] == '\n') p[strlen(p)-1] = 0;
    return p;
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

char** tokenizeArtigoDyn(char* artigo, int* tamanho, int quantos){
    
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

    int fd1  = open("VENDAS.txt", O_RDONLY, 0666);
    int codArtigos[BUFFSIZE];
    int tamanho1,tamanho2;
    int sumStock, sumMontante;
    int fdDest = 1;
    if(argc == 2) fdDest = open(strcat(removeEnter(argv[1]),".txt"), O_CREAT | O_WRONLY,0666);

    while(1){ // Percorre o ficheiro de vendas para ver quantos códigos de artigo há
        tamanho1 = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(fd1,buffer,sizeof buffer);
        if(n <= 0) break;
        char** info = tokenizeArtigoDyn(buffer,&tamanho1,3);
        addSemRep(codArtigos,atoi(info[0]));
    }
    close(fd1);

    int fd2 = open("VENDAS.txt", O_RDONLY, 0666);
    for(int i = 0; codArtigos[i]; i++){ // Volta a percorrer o ficheiro de vendas para agregar os dados

        lseek(fd2,0,SEEK_SET); // Põe a apontar para o início do ficheiro
        sumStock = sumMontante = 0;
        while(1){
            tamanho2 = 0;
            char buffer[BUFFSIZE];
            size_t n = readln(fd2,buffer,sizeof buffer);
            if(n <= 0) break;
            char** info = tokenizeArtigoDyn(buffer,&tamanho2,3);
            if(atoi(info[0]) == codArtigos[i]){
                sumStock += atoi(info[1]);
                sumMontante += atoi(info[2]);
            }
        }
        char* codigoStr   = malloc(BUFFSIZE); sprintf(codigoStr,"%d",codArtigos[i]);
        char* stockStr    = malloc(BUFFSIZE); sprintf(stockStr,"%d",sumStock);
        char* montanteStr = malloc(BUFFSIZE); sprintf(montanteStr,"%d\n",sumMontante);
        write(fdDest,concat(concat(codigoStr,stockStr),montanteStr),strlen(concat(concat(codigoStr,stockStr),montanteStr)));
    }
    close(fd2);
    return 0;
}