#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

char* concat(const char *s1, const char *s2){

    char *result = malloc(strlen(s1) + strlen(s2) + 2); // +2 para o espaço e para o fim
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

void replacer(char* c, char* num, int index){

    int tamanho = 0;

    int fPtr  = open("ARTIGOS.txt", O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY,0666); 

    if(fPtr == -1 || fTemp == -1) {write(1,"ERRO\n",5); exit(1);}

    int jaSubstituiu = 0;
    char** info;
    while(1){
        tamanho = 0;
        char buffer[BUFFSIZE];
        char* newline = malloc(BUFFSIZE);
        size_t n = readln(fPtr,buffer,sizeof buffer);
        if(n <= 0) break;
        info = tokenizeArtigoDyn(buffer,&tamanho);
        if(!strcmp(info[0],c) && jaSubstituiu == 0){
            info[index] = strdup(num);
            newline = concat(info[0],concat(info[1],info[2]));
            write(fTemp,newline,strlen(newline));
            jaSubstituiu = 1;
        }
        else write(fTemp,buffer,n);
    }

    remove("ARTIGOS.txt");
    rename("replace.tmp", "ARTIGOS.txt");
}

// ------------------------------------------------------------------------------------------------

int main(){

    int fd1 = open("ARTIGOS.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666); 
    int fd2 = open("STRINGS.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int line = 1;
    if(fd1 == -1 || fd2 == -1) {write(1,"ERRO\n",5); exit(1);}

    while(1){
        char *buf = malloc(BUFFSIZE);
        int pre = sprintf(buf,"%d ",line);
        ssize_t n = readln(0,buf+pre,BUFFSIZE-pre); // Código numérico + linha lida
        if (n <= 0) break;

        int tamanho = 0;
        char** info = tokenizeArtigoDyn(buf,&tamanho);
        /*
        * info[0] = código
        * info[1] = letra
        * info[2] = 1º argumento
        * info[3] = 2º argumento
        */
        if(tamanho == 4){
            switch(buf[pre]){
                case 'i':{
                    // Parte da escrita nos ficheiros
                    char* num_1stArg = concat(info[0],info[2]); // Referência - Nome
                    write(fd2,concat(num_1stArg,"\n"),strlen(concat(num_1stArg,"\n")));
                    
                    char* num_2ndArg = concat(info[0],concat(info[0],info[3])); // Código - Referência - Preço
                    write(fd1,num_2ndArg,strlen(num_2ndArg));
                    
                    // Parte do código numérico
                    char charLinha[BUFFSIZE];
                    sprintf(charLinha,"O número é: %d\n",line); 
                    write(1,charLinha,strlen(charLinha));
                    break;
                }
                case 'n':{
                    char* a1 = concat(info[0],info[3]);
                    write(fd2,a1,strlen(a1));
                    replacer(info[2],info[0],1); // 1 pois será a referência a ser alterada
                    break;
                }
                case 'p':{
                    replacer(info[2],info[3],2); // 2 pois será o preço a ser alterado
                    break;
                }
                default: write(1,"Formato errado\n",16);
            }
            line++;
        }
        else write(1,"Formato errado\n",16);
    }
    return 0;
}