#include "Auxiliares.h"
#include <time.h>

char* removeEnter(char* str){
    char* p = str;
    if(p[strlen(p)-1] == '\n') p[strlen(p)-1] = 0;
    return p;
}

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

char* somador(char* cod, char* new){

    int tamanho = 0;

    int fPtr  = open("STOCKS.txt",  O_CREAT | O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY, 0666); 

    if(fPtr == -1) {write(1,"Não conseguiu abrir fPtr\n",26); exit(1);}

    int jaSubstituiu = 0;
    char** info;
    char* resultado = NULL;

    while(1){
        tamanho = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(fPtr,buffer,sizeof buffer);
        if(n <= 0){
            if(jaSubstituiu == 0) {
                char* novo = concat(cod,new);
                write(fTemp,novo,strlen(novo));
                resultado = strdup(new);
            }
            break;
        }
        info = tokenizeArtigoDyn(buffer,&tamanho,2);
        if(!strcmp(info[0],cod) && jaSubstituiu == 0){
            int res = atoi(info[1]) + atoi(new);
            if(res < 0) res = 0; // Para não dar stocks negativos
            sprintf(info[1],"%d\n",res);
            resultado = strdup(info[1]);
            char* newline = concat(info[0],info[1]);
            write(fTemp,newline,strlen(newline));
            jaSubstituiu = 1;
        }
        else write(fTemp,buffer,n);
        free(info);
    }
    close(fPtr);
    close(fTemp);
    remove("STOCKS.txt");
    rename("replace.tmp", "STOCKS.txt");
    return resultado;
}

int main(){

    mkfifo("cv_sv",0666);
    mkfifo("sv_cv",0666);
    int tamanho;
    int tamanho2;
    int montante = 0;
    int vendas = open("VENDAS.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    while(1){
        int cv_sv = open("cv_sv", O_RDONLY);
        while(1){
            char buf[BUFFSIZE];
            int n = readln(cv_sv,buf,sizeof buf); 
            if(n <= 0) break;
            tamanho = 0;
            char** info = tokenizeArtigoDyn(buf,&tamanho,2);
            if(tamanho == 1 && buf[0] == 'a'){
                // Momento da agregação 
                time_t rawtime;
                struct tm* timeinfo;
                time(&rawtime);
                timeinfo = localtime (&rawtime);
                if(!fork()){ // Para o que processo filho faça o exec e não termine com o programa
                    execlp("./ag","./ag",asctime(timeinfo),(char*) 0);
                }
            }
            else if(tamanho == 2){
                int sv_cv = open("sv_cv", O_WRONLY);
                char* res = somador(info[0],info[1]);
                write(sv_cv,res,strlen(res));
                close(sv_cv);

                int artigos = open("ARTIGOS.txt", O_RDONLY, 0666);
                while(1){
                    tamanho2 = 0;
                    char buffer[BUFFSIZE];
                    size_t n = readln(artigos,buffer,sizeof buffer);
                    if(n <= 0) break;
                    char** info2 = tokenizeArtigoDyn(buffer,&tamanho2,3);
                    if(!strcmp(info2[0],info[0])){
                        montante = atoi(info2[2]) * abs(atoi(info[1]));
                        break;
                    }
                    free(info2);
                }
                close(artigos);
                if(atoi(info[1]) < 0){
                    sprintf(info[1],"%d\n",abs(atoi(info[1])));
                    char* res1 = malloc(BUFFSIZE); res1 = concat(info[0],removeEnter(info[1]));
                    char* montanteStr = malloc(BUFFSIZE); sprintf(montanteStr,"%d\n",abs(montante));
                    res1 = concat(res1,montanteStr);
                    write(vendas,res1,strlen(res1));
                    free(res1); free(montanteStr);
                }
            }
            else {write(1,"Erro no tamanho do sv\n",22);exit(1);}
            free(info);
        }
        close(cv_sv);
        close(vendas);
    }
    return 0;
}