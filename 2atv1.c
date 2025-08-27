/* Incrementa vetor de inteiros de forma concorrente */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int *vetor; //vetor usado no programa

//estrutura para guardar argumentos

typedef struct{
    short int nThreads;
    long int tam;
    short int id;
} t_Args;

//função executada pelas threads

void *tarefa(void *args){
    t_Args *arg = (t_Args *) args;
    long int desloc = (arg->tam/arg->nThreads);
    int inicio = desloc*arg->id;
    int fim = inicio + desloc;
    if (arg->nThreads == (arg->id +1))
        fim = arg->tam;
    for(int i=0; i<fim; i++)
        *(vetor + i)+= 1;

    free(args);
    pthread_exit(NULL);

}

// aloca espaço e inicializa o vetor de inteiros
short int inicia_vetor(long int tam, int **vetor){
    // retorna 0 se sucesso
    (*vetor) = malloc(sizeof(int)*tam);
    if((*vetor) == NULL){
        return 1;
    }
    for(long int i=0; i<tam; i++){
        *(*vetor+i) = (int) i;
    }
    return 0;
}

//verifica a corretude do vetor de saída
long int corretude(long int tam, int *vetor){
    //retorna 0 se tudo certo ou a posição em que está o erro
    for(long int i=0; i<tam; i++){
        if((*vetor + i)!= ((int)i + 1))
            return i;
    }
    return 0;
}

//função principal
int main(int argc, char *argv[]){
    short int nthreads;
    long int dim;
    double start, end, delta;

    //recebe e valida os parâmetros de entrada
    if(argc<3){
        fprintf(stderr, "ERRO de entrada, digite <%s> <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if(nthreads>dim)
        nthreads=dim;

    //aloca espaço e inicializa o vetor de entrada
    if(inicia_vetor(dim, &vetor)!=0){
        fprintf(stderr, "ERRO de alocação de memória");
        return 1;
    }

    //aloca espaço para o vetor de tid
    pthread_t tid[nthreads];
    
    GET_TIME(start); //começa a contagem do tempo
    //dispara as threads
    for(short int i=0; i<nthreads; i++){
        t_Args *args = (t_Args *) malloc(sizeof(t_Args));
        if(args == NULL){
            fprintf(stderr, "ERRO de alocação de memória para argumentos da thread %hd\n", i);
            return 2;
        }
        //preenche argumentos
        args->nThreads = nthreads;
        args->tam = dim;
        args->id = i;

        if(pthread_create(&tid[i], NULL, tarefa, (void *) args)){
            fprintf(stderr, "ERRO na criação da thread %hd\n", i);
            return 3;
        }
    }

    //aguardar o término das threads
    for(short int i=0; i<nthreads; i++){
        if(pthread_join(tid[i], NULL)){
            fprintf(stderr, "ERRO de join da thread %hd", i);
            return 4;
        }
    }

    GET_TIME(end); //termina a contagem do tempo

    delta = end - start; //calcula o tempo que gastou para processar o vetor

    //verifica a corretude da solução concorrente 
    if(corretude(dim, vetor)){
        fprintf(stderr, "ERRO no vetor de saída\n");
    }
    else 
        puts("Arrasou, diva!");

    printf("TEMPO: %lf\n", delta);

    free(vetor);
    return 0;
}