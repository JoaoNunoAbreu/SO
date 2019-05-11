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

void replacer(char* cod, char* new, int index){

    int tamanho = 0;

    int fPtr  = open("ARTIGOS.txt", O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY,0666); 

    if(fPtr == -1 || fTemp == -1) {write(1,"ERRO\n",5); exit(1);}

    int jaSubstituiu = 0;

    while(1){
        tamanho = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(fPtr,buffer,sizeof buffer);
        if(n <= 0) break;
        char** info = tokenizeArtigoDyn(buffer,&tamanho,3);
        if(!strcmp(info[0],cod) && jaSubstituiu == 0){
            info[index] = strdup(new);
            char* newline = concat(info[0],concat(info[1],info[2]));
            newline = strtok(newline,"\n\r");
            strcat(newline,"\n");
            write(fTemp,newline,strlen(newline));
            jaSubstituiu = 1;
        }
        else write(fTemp,buffer,n);
    }
    close(fPtr);
    close(fTemp);
    remove("ARTIGOS.txt");
    rename("replace.tmp", "ARTIGOS.txt");
}

int main(){

    int fd1 = open("ARTIGOS.txt", O_CREAT | O_TRUNC , 0666); 
    int fd2 = open("STRINGS.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd1 == -1 || fd2 == -1) {write(1,"ERRO\n",5); exit(1);}
    int fd_server = open("fifo_server", O_WRONLY);

    int codArtigo = 1; int line = 1;

    while(1){
        char* buf = malloc(BUFFSIZE);
        int pre = sprintf(buf,"%d ",line);
        ssize_t n = readln(0,buf+pre,BUFFSIZE-pre); // Código numérico + linha lida
        if (n <= 0) break;

        int tamanho = 0;
        char** info = tokenizeArtigoDyn(buf,&tamanho,4);
        /*
        * info[0] = código
        * info[1] = letra
        * info[2] = 1º argumento
        * info[3] = 2º argumento
        */

        int offset = lseek(fd2,0,SEEK_END);
        char* linhaEmStr = malloc(BUFFSIZE);
        sprintf(linhaEmStr,"%d",offset);

        switch(buf[pre]){
            case 'i':{
                char* codigoEmStr = malloc(BUFFSIZE);
                sprintf(codigoEmStr,"%d",codArtigo);
                        
                // Escrita no ficheiro de strings
                write(fd2,info[2],strlen(info[2]));

                // Escrita no ficheiro de artigos
                fd1 = open("ARTIGOS.txt", O_APPEND | O_WRONLY, 0666);
                char* num_2ndArg = concat(codigoEmStr,concat(linhaEmStr,info[3])); // Código - Referência - Preço
                write(fd1,num_2ndArg,strlen(num_2ndArg));

                // Print no stdout do código do artigo
                char charLinha[BUFFSIZE];
                sprintf(charLinha,"Código: %d\n",codArtigo);
                write(1,charLinha,strlen(charLinha));

                char novopreco[BUFFSIZE];
                sprintf(novopreco,"p %d %d\n",codArtigo,atoi(info[3]));
                write(fd_server,novopreco,strlen(novopreco));

                codArtigo++;
                free(codigoEmStr);
                break;
            }
            case 'n':{
                write(fd2,removeEnter(info[3]),strlen(removeEnter(info[3])));
                replacer(info[2],linhaEmStr,1); // 1 pois será a referência a ser alterada
                break;
            }
            case 'p':{
                replacer(info[2],info[3],2); // 2 pois será o preço a ser alterado
                write(fd_server,buf+pre,n);
                break;
            }
            case 'a':{
                write(fd_server,buf+pre,n);
                break;
            }
            default: {write(1,"Formato errado\n",16);}
        }
        /*if(!fork()){
            execlp("./compact","./compact",(char*) 0);
            _exit(1);
        }*/
        int status;
        wait(&status);
    }
    close(fd_server);
    close(fd1);
    close(fd2);
    return 0;
}