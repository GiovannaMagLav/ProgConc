#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// cria a estrutura de dados necessária para armazenar os argumnetos das threads
typedef struct{
    int idThread, nThreads;
} t_Args;

//funcao executada pelas threads 
void *func(void *arg){
    // typecasting dos argumentos
   // t_Args args = *(t_Args*) arg; //desreferencia um ponteiro do tipo targs pra passar o conteudo da estrutura
   t_Args *args = (t_Args*) arg;
    
    printf("Sou a thread %d de %d\n", args->idThread, args->nThreads);

    free(arg); //libera memória alocada na main

    pthread_exit(NULL);
}

//funcao principal - main
int main(int argc, char *argv[]){
    t_Args *args; //receber argumentos pra thread
    int nthreads;

    //verifica argumentos da linha de comando
    if(argc<2){
        printf("Quantidade insuficiente de argumentos\n");
        return 1;
    }
    // armazena numero de threads
    nthreads = atoi(argv[1]);

    pthread_t tid_sistema[nthreads];

    //cria as threads e atribui a função de cada uma
    for(int i=1; i<=nthreads; i++){
        printf("Aloca e preenche argumentos para a thread %d\n", i);
        args = malloc(sizeof(t_Args));
        if(args == NULL){
            printf("Erro de alocação de memória\n");
            return 2;
        }
        args->idThread = i;
        args->nThreads = nthreads;
        
        printf("Criando a thread %d\n", i);
        if(pthread_create(&tid_sistema[i-1], NULL, func, (void*) args)){ //usa parenteses na conversao pra void pq aí faz a operacao primeiro pela precedencia
            printf("Erro ao criar a thread %d", i);
            return 3;
        } 
    }

    //espera todas as threads terminarem
    for(int i=0; i<nthreads; i++){
        if(pthread_join(tid_sistema[i], NULL)){
            printf("ERRO de pthread join da thread %d", i);
            return 4;
        }

    }

    // log da funcao principal
    printf("Função principal terminou\n");
    //pthread_exit(NULL); //nao é mais necessario pelo join

}