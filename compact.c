#include "Auxiliares.h"

int elem (int a[], int var) {
    
    int i,r = 0;
    for (i = 0; a[i];i++)
        if (a[i] == var) r = 1;   
    return r;
    
}

int remRep(int s1[], int s2[], int s3[]){
    
    int count = 0;

    for(int i = 0; s1[i]; i++){
        if(!elem(s2,s1[i])) s3[count++] = s1[i];
    }
    for(int i = 0; s2[i]; i++){
        if(!elem(s1,s2[i])) s3[count++] = s2[i];
    }
    s3[count] = '\0';
    return count;
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

void deleteLinhas(int num, char* filename){

    int tamanho = 0;
    
    int fPtr  = open(filename, O_RDONLY, 0666); 
    int fTemp = open("replace.tmp", O_CREAT | O_TRUNC | O_WRONLY,0666); 

    if(fPtr == -1 || fTemp == -1) {write(1,"ERRO\n",5); exit(1);}
    
    while(1){
        tamanho = 0;
        char buffer[BUFFSIZE];
        size_t n = readln(fPtr,buffer,sizeof buffer);
        if(n <= 0) break;
        char** info = tokenizeArtigoDyn(buffer,&tamanho,3);
        if(atoi(info[0]) == num) write(fTemp,"",0);
        else write(fTemp,buffer,n);
    }
    remove(filename);
    rename("replace.tmp", filename);
}

int main(){

    int artigos = open("ARTIGOS.txt",O_RDONLY);
    int strings = open("STRINGS.txt",O_RDONLY);
    int tamanho1, tamanho2; int contador;

    int refsArtigos[BUFFSIZE] = {};
    int refsString[BUFFSIZE] = {};

    // Percorre o ficheiro de artigos
    contador = 0;
    while(1){ 
        tamanho1 = 0;
        char buf1[BUFFSIZE];
        int n1 = readln(artigos,buf1,sizeof buf1);
        if(n1 <= 0) break;
        char** info1 = tokenizeArtigoDyn(buf1,&tamanho1,3);
        refsArtigos[contador++] = atoi(info1[1]);
    }
    close(artigos);

    // Percorre o ficheiro de strings
    contador = 0; int nLinhasStrings = 0;
    while(1){ 
        tamanho2 = 0;
        char buf2[BUFFSIZE];
        int n2 = readln(strings,buf2,sizeof buf2);
        if(n2 <= 0) break;
        char** info2 = tokenizeArtigoDyn(buf2,&tamanho2,3);
        refsString[contador++] = atoi(info2[0]);
        nLinhasStrings++;
    }
    close(strings);

    int porRemover[BUFFSIZE] = {};
    int linhasLixo = remRep(refsArtigos,refsString,porRemover);
    
    if((double)linhasLixo / (double)nLinhasStrings > 0.2){
        for(int i = 0; porRemover[i]; i++){
            printf("porRemover[%d] = %d\n",i,porRemover[i]);
            deleteLinhas(porRemover[i],"STRINGS.txt");
        }
    }
    return 0;
}