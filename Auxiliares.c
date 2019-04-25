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

char* concat(const char *s1, const char *s2){

    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +2 para o espaço e para o fim
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

void replacer(char* cod, char* new, int index, char* filename){

    int tamanho = 0;

    int fPtr  = open(filename, O_RDONLY, 0666); 
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
        if(!strcmp(info[0],cod) && jaSubstituiu == 0){
            info[index] = strdup(new);
            newline = concat(info[0],concat(info[1],info[2]));
            write(fTemp,newline,strlen(newline));
            jaSubstituiu = 1;
        }
        else write(fTemp,buffer,n);
    }
    remove(filename);
    rename("replace.tmp", filename);
}

int strToInt(char* str){

    char *ptr;
    long ret;
    ret = strtol(str, &ptr, 10);

    return ret;
}