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

void portionOf(char* input, char* output, int inicio, int fim){

    int fdI = open(input,O_RDONLY);
    int fdO = open(output, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    int count = 1;

    while(1){
        char buffer[BUFFSIZE];
        size_t n = readln(fdI,buffer,sizeof buffer);
        if(n <= 0) break;
        if(count >= inicio && count <= fim)
            write(fdO,buffer,n);
        count++;
    }
    close(fdI);
    close(fdO);
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
            //if(res < 0) res = 0; // Para não dar stocks negativos
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

    mkfifo("fifo_server",0666);

    int tamanho; int fd;
    int montante = 0;
    int vendas = open("VENDAS.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int precos[BUFFSIZE] = {}; // Array que contem preços dos artigos correspondentes do índice

    signal(SIGPIPE,SIG_IGN);
    int fd_server = open("fifo_server", O_RDONLY);
    while(1){
        while(1){
            char buf[BUFFSIZE];
            int n = readln(fd_server,buf,sizeof buf); 
            if(n <= 0) break;
            buf[n] = '\0';
            tamanho = 0;
            char** info = tokenizeArtigoDyn(buf,&tamanho,4);

            // ---- Agregação com intervalos ----

            if(tamanho == 3 && !strcmp(info[0],"a")){

                char* temp = "temp.txt";
                portionOf("VENDAS.txt",temp,atoi(info[1]),atoi(info[2]));
                
                if(!fork()){
                    fd = open(temp,O_RDONLY);
                    dup2(fd,0);
                    close(fd);

                    fd = open(strcat(getTime(),".txt"),O_CREAT | O_WRONLY | O_TRUNC,0666);
                    dup2(fd,1);
                    close(fd);

                    execlp("./ag","./ag",(char*) 0);
                    _exit(1);
                }
                int status;
                wait(&status);
                remove(temp);
            }

            // ------ Agregação concorrente -----

            else if(tamanho == 2 && !strcmp(info[0],"a")){
                int nLinhas = contaLinhas("VENDAS.txt");
                if(nLinhas < atoi(info[1])){
                    write(1,"Mais bocados do que linhas\n",27);
                }
                else{
                    int nLinhas = contaLinhas("VENDAS.txt");
                    int pa = 1;
                    int sa = nLinhas / (atoi(info[1]));
                    if(nLinhas % (atoi(info[1])) > 1) sa++;
                    int valorInitSa = sa; 
                    int eof = 0; 
                    int i;

                    char fich_data[BUFFSIZE];
                    sprintf(fich_data,"%s.txt",getTime());

                    for(i = 0; !eof ; i++){
                        char temp[BUFFSIZE];
                        sprintf(temp,"temp%d.txt",i);
                        portionOf("VENDAS.txt",temp,pa,sa);

                        if(!fork()){

                            fd = open(temp,O_RDONLY);
                            dup2(fd,0);
                            close(fd);
                            remove(temp);

                            fd = open(fich_data,O_CREAT | O_WRONLY | O_APPEND,0666);
                            dup2(fd,1);
                            close(fd);
                
                            execlp("./ag","./ag",(char*) 0);
                            _exit(1);
                        }
                        if(pa == nLinhas || sa == nLinhas) eof = 1;
                        if(sa+1 > nLinhas) pa = nLinhas; else pa = sa + 1;
                        if(sa + sa > nLinhas) sa = nLinhas; else sa += valorInitSa;
                    }
                    for(int j = 0; j < i; j++){
                        int status;
                        wait(&status);
                    }

                    if(!fork()){
                        fd = open(fich_data,O_RDONLY);
                        dup2(fd,0);
                        close(fd);
                        remove(fich_data);

                        fd = open(fich_data,O_CREAT | O_TRUNC | O_WRONLY ,0666);
                        dup2(fd,1);
                        close(fd);

                        execlp("./ag","./ag",(char*) 0);
                        _exit(1);
                    }
                    int status;
                    wait(&status);
                }
            }

            // ------- Caching de preços --------

            else if(tamanho == 3 && !strcmp(info[0],"p")){
                precos[atoi(info[1])-1] = atoi(info[2]);
            }

            // ------- Cliente de vendas --------

            else if(tamanho == 3){

                char* return_fifo = strtok(buf," ");
                int fd_client = open(return_fifo, O_WRONLY);
                char* res = somador(info[1],info[2]);
                write(fd_client,res,strlen(res));
                close(fd_client);

                montante = precos[atoi(info[1])-1] * abs(atoi(info[2]));

                if(atoi(info[2]) < 0){
                    sprintf(info[2],"%d",abs(atoi(info[2])));
                    char* res1 = concat(info[1],info[2]);
                    char* montanteStr = malloc(BUFFSIZE); sprintf(montanteStr,"%d\n",abs(montante));
                    res1 = concat(res1,montanteStr);
                    write(vendas,res1,strlen(res1));
                    free(res1); free(montanteStr);
                }
            }
            else{
                write(1,"Erro no código lido do pipe\n",29);
                printf("tamanho = %d\n",tamanho);
                printf("n = %d\n",n);
                printf("strlen = %lu\n",strlen(buf));
                write(1,buf,n);
            }
            free(info);
        }        
    }
    close(fd_server);
    close(vendas);
    return 0;
}