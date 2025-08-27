/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */
/* atividade 4 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
    int *vetor;
    int idThread;
    int N;
} t_Args;

//funcao executada pelas threads
void *Incrementa (void *arg) {
  t_Args *args = (t_Args *) arg;

  for(int i=0; i<args->N; i++){
    *(args->vetor+i+(args->idThread*args->N))+=1;
  }

  free(arg); //libera a alocacao feita na main

  pthread_exit(NULL);
}

// funcao para inicializar vetor de entrada
void inicializa_vetor(int **vetor, int tam){
    *vetor = malloc(tam*sizeof(int));
    if(vetor == NULL){
        printf("Erro de alocação dos vetores");
        exit(1);
    }
}

// funcao para verificar os valores do vetor
void print_vetor(int *vetor, int tam){
    for(int i=0; i<tam; i++)
        printf("Elemento na posição %d: %d\n", i, *(vetor+i));
}

//funcao principal do programa
int main(int argc, char* argv[]) {
  t_Args *args; //receberá os argumentos para a thread

  int nthreads = 4; //qtde de threads que serao criadas 
  int *vetor;
  int N;
  int tam_vetor;

  //verifica se o argumento 'N' foi passado e armazena seu valor
  if(argc<2) {
      printf("--ERRO: informe a qtde de threads <%s> <nthreads>\n", argv[0]);
      return 1;
  }
  N = atoi(argv[1]);

  //identificadores das threads no sistema
  pthread_t tid_sistema[nthreads];

   tam_vetor = nthreads*N;
    //inicializar o vetor
    inicializa_vetor(&vetor, tam_vetor);
    for(int i=0; i<nthreads*N; i++){
        *(vetor+i) = 0;
    }

  //cria as threads
  for(int i=0; i<nthreads; i++) {
    printf("--Aloca e preenche argumentos para thread %d\n", i);
    args = malloc(sizeof(t_Args));
    if (args == NULL) {
      printf("--ERRO: malloc()\n"); 
      return 1;
    }

    //preencher args
    args->idThread = i; 
    args->N = N; 
    args->vetor = vetor;
    
    printf("--Cria a thread %d\n", i);
    if (pthread_create(&tid_sistema[i], NULL, Incrementa, (void*) args)) {
      printf("--ERRO: pthread_create()\n"); 
      return 2;
    }
  }

  //espera todas as threads terminarem
  for (int i=0; i<nthreads; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() da thread %d\n", i); 
    } 
  }

  //printar o vetor final
  print_vetor(vetor, tam_vetor);

  free(vetor);
  //log da função principal
  printf("--Thread principal terminou\n");

  //pthread_exit(NULL); //nao é necessario nesse caso pelo join

  return 0;
}