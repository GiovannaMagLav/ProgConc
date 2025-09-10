/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio + implementando requisito condicional*/
/* LAB 5 - ex 5*/
/*Alterando a implementacao da thread extra de modo a garantir que ela imprima todos os valores de soma que sao multiplos de 1000. 
O objetivo e fazer a thread ExecutaTarefa pausar sua execucao quando um multiplo de 1000 e alcancado
e somente continuar depois que o seu valor for impresso pela thread extra. */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0; // variavel compartilhada entre as threads
pthread_mutex_t mutex; // lock para exclusao mutua
pthread_cond_t imprimir; // condição para poder imprimir
pthread_cond_t incrementar; // condição pra voltar a incrementar
int imprimir_multiplo = 0; //avisa se há múltiplo pra imprimir

// funcao executada pelas threads de trabalho
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  //printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--SC (seção critica)
     //espera extra imprimir o múltiplo, se tiver
     while(imprimir_multiplo) {
        pthread_cond_wait(&incrementar, &mutex);
     }

     soma++;

     // sinaliza extra que tem um múktiplo para imprimir
     if(!(soma%1000)) {
        imprimir_multiplo = 1; // novo múltiplo para impressão
        pthread_cond_signal(&imprimir); // manda sinal para a extra imprimir

        //espera extra imprimir
        while (imprimir_multiplo) {
           pthread_cond_wait(&incrementar, &mutex);
        }
     }

     pthread_mutex_unlock(&mutex);
  }
  //printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// funcao executada pela thread extra (logger)
void *extra (void *args) {

  for (int i = 0; i < 100000; i++)  {
     pthread_mutex_lock(&mutex);

     // espera signal das threads
     while (!imprimir_multiplo)
        pthread_cond_wait(&imprimir, &mutex);

     printf("soma = %ld\n", soma);
     // marca como impresso
     imprimir_multiplo = 0;
     
     // libera todas as threads que estavam esperando na executatarefa
     pthread_cond_broadcast(&incrementar);
     pthread_mutex_unlock(&mutex);
  }

  //printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; // identificadores das threads no sistema
   int nthreads; // qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if (argc < 2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t) * (nthreads + 1));
   if (tid == NULL) { puts("ERRO--malloc"); return 2; }

   //--inicializa o mutex e variáveis de condição
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond_print, NULL);
   pthread_cond_init(&cond_continue, NULL);

   //--cria as threads 
   for (long int t = 0; t < nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log 
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t = 0; t < nthreads + 1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);

   //libera a variável de condição
   pthread_cond_destroy(&imprimir);
   pthread_cond_destroy(&incrementar);

   printf("Valor de 'soma' = %ld\n", soma);

   free(tid);
   
   return 0;
}
