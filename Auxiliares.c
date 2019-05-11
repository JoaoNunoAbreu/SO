#include "Auxiliares.h"

int elem (int a[], int var){
    
    int i,r = 0;
    for (i = 0; a[i];i++)
        if (a[i] == var) r = 1;   
    return r;   
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

char* removeEnter(char* str){
    char* p = str;
    if(p[strlen(p)-1] == '\n') p[strlen(p)-1] = 0;
    return p;
}

int contaLinhas(char* file){

    int filefd = open(file, O_RDONLY, 0666);
    int count = 0;
    while(1){
        char buf[100];
        int n = readln(filefd,buf,sizeof buf);
        if(n <= 0) break;
        count++;
    }
    close(filefd);
    return count;
}

char* getTime(){

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);

    return removeEnter(asctime(timeinfo));
}