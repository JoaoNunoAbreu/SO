#include "Auxiliares.h"

int elemOf(int a[], int var,int x){
    
    int i,r = 0;
    for(i = 0; i < x;i++)
        if(a[i] == var) r = 1;   
    return r;   
}

int min_after(int N,int a[N],int key){
    int min = INT_MAX;
    for(int i = 0; i < N; i++)
        if(a[i] < min && a[i] > key) min = a[i];
    return min;
}

int length(int offset, int N, int array_offsets[N],int tamanhoficheiro){

    int res = 0;
    for(int i = 0; i < N; i++)
        if(offset == array_offsets[i] && i != N-1) res = min_after(N,array_offsets,array_offsets[i]) - array_offsets[i];
        else if(offset == array_offsets[i]) res = tamanhoficheiro - array_offsets[i];
    return res;
}

int intervalos_validos(int N1, int N2, int array_offsets[N1] ,int array_offsets_validos[N2], int* res, int tamanhoficheiro){

    int index = 0;

    for(int i = 0; i < N2; i++){
        for(int j = 0; j < length(array_offsets_validos[i],N1,array_offsets,tamanhoficheiro); j++){
            res[index++] = array_offsets_validos[i] + j;
        }
    }
    res[index] = '\0';
    return index;
}

void compactador(int N1, int N2, int array_offsets[N1], int array_offsets_validos[N2],int tamanhoficheiro){

    int fPtr  = open("STRINGS.txt", O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY,0666); 
    int res[BUFFSIZE] = {};
    int offset = 0; int por_apagar = 0;
    int count = intervalos_validos(N1,N2,array_offsets,array_offsets_validos,res,tamanhoficheiro);

    while(1){
        char c = '\0';
        size_t n = read(fPtr,&c,1);
        if(n <= 0) break;
        if(!elemOf(res,offset,count)){
            write(fTemp,"",0);
            por_apagar++;
        }
        else{
            write(fTemp,&c,1);
            for(int i = 0; i < N2; i++){
                if(array_offsets_validos[i] >= offset)
                    array_offsets_validos[i] -= por_apagar;
            }
            por_apagar = 0;
        }
        offset++;
    }

    close(fPtr);
    close(fTemp);
    remove("STRINGS.txt");
    rename("replace.tmp","STRINGS.txt");
    
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

double quant_lixo(int N1, int N2, int array_offsets[N1], int array_offsets_validos[N2]){

    int sum = 0;

    for(int i = 0; i < N1; i++)
        if(!elemOf(array_offsets_validos,array_offsets[i],N2))
            sum += min_after(N1,array_offsets,array_offsets[i]) - array_offsets[i];
    return (double)sum;
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
    int fd2 = open("STRINGS.txt", O_CREAT | O_TRUNC , 0666);
    if(fd1 == -1 || fd2 == -1) {write(1,"ERRO\n",5); exit(1);}
    int fd_server = open("fifo_server", O_WRONLY);

    int codArtigo = 1; int line = 1; int i = 0; int j = 0;
    int array_offsets[BUFFSIZE] = {};
    int array_offsets_validos[BUFFSIZE] = {};

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

        int fd2 = open("STRINGS.txt",O_WRONLY, 0666);
        int offset = lseek(fd2,0,SEEK_END);
        char* linhaEmStr = malloc(BUFFSIZE);
        sprintf(linhaEmStr,"%d",offset);
        if(!elemOf(array_offsets,offset,i)) array_offsets[i++] = offset;

        switch(buf[pre]){
            case 'i':{

                if(!elemOf(array_offsets_validos,offset,j)){
                    array_offsets_validos[j++] = offset;
                }

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
                array_offsets_validos[atoi(info[2])-1] = offset;
                write(fd2,removeEnter(info[3]),strlen(removeEnter(info[3])));
                replacer(info[2],linhaEmStr,1); // 1 pois será a referência a ser alterada
                break;
            }
            case 'p':{
                replacer(info[2],info[3],2); // 2 pois será o preço a ser alterado
                write(fd_server,buf+pre,n);
                break;
            }
            // "a" para agregar normalmente
            // "a x y" para agregar no intervalo x a y
            // "a x" para agregar concorrentemente com x intervalos 
            case 'a':{
                write(fd_server,buf+pre,n);
                break;
            }
            default: {write(1,"Formato errado\n",16);}
        }

        if(quant_lixo(i,j,array_offsets,array_offsets_validos) / (double)offset > 0.2){

            compactador(i,j,array_offsets,array_offsets_validos,lseek(fd2,0,SEEK_END));
            for(int i = 0; i < codArtigo; i++){

                char* str_i = malloc(BUFFSIZE);
                char* str_offset = malloc(BUFFSIZE);

                sprintf(str_i,"%d",i+1);
                sprintf(str_offset,"%d",array_offsets_validos[i]);

                replacer(str_i,str_offset,1);
            }
            memset(array_offsets,0,sizeof array_offsets);
            for(int x = 0; x < j; x++)
                array_offsets[x] = array_offsets_validos[x];
            i = j;
        }
    }
    close(fd_server);
    close(fd1);
    close(fd2);
    return 0;
}