#include "Auxiliares.h"

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

int main(){

    int codArtigos[BUFFSIZE] = {};
    int tamanho1,tamanho2;
    int sumStock, sumMontante;

    while(1){
        tamanho1 = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(0,buffer,sizeof buffer);
        if(n <= 0) break;
        char** info = tokenizeArtigoDyn(buffer,&tamanho1,3);
        addSemRep(codArtigos,atoi(info[0]));
        free(info);
    }

    for(int i = 0; codArtigos[i]; i++){
        lseek(0,0,SEEK_SET); 
        sumStock = sumMontante = 0;
        while(1){
            tamanho2 = 0;
            char buffer[BUFFSIZE];
            size_t n = readln(0,buffer,sizeof buffer);
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
        write(1,concat(concat(codigoStr,stockStr),montanteStr),strlen(concat(concat(codigoStr,stockStr),montanteStr)));
    }
    return 0;
}