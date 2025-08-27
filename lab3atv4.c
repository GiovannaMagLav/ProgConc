/*
Disciplina: Programação Concorrente 
Profa.: Silvana Rossetto 
Aluna: Giovanna Lavouras
LAB 3 - atividade 4
Descricao: calcular o produto interno de dois vetores de numeros reais de forma concorrente
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

//definir os vetores globalmente
float *vetor1;
float *vetor2;

// estrutura para passar argumentos para a função das threads
typedef struct{
    long int N; //dimensão dos vetores
    int nthreads; //quantidade de threads
    long int id; // identificador da thread atual
} t_args;

//função executada pelas threads
//para ficar balanceado, vamos dividir em blocos iguais e tratar possíveis restos
void *ProdInterno(void *args){
    t_args *arg = (t_args *) args; //typecasting
    int fatia = arg->N / arg->nthreads; // tamanho dos blocos pelos quais as threads ficarão responsáveis
    int inicio = arg->id * fatia;
    int fim = (arg->id == (arg->nthreads - 1)) ? arg->N : (inicio+fatia);
    float *parcela_prodint = (float *) malloc(sizeof(float)); // guarda o produto interno para as coordenadas que estão na fatia usada
   
    if(parcela_prodint == NULL){
        fprintf(stderr, "Erro de alocação de memória");
    }
    *parcela_prodint = 0;

    //calcula o prod interno para determinadas coordenadas
    for(long int i = inicio; i < fim ; i++){
        *parcela_prodint += (*(vetor1 + i))*(*(vetor2 + i));
    }
    free(arg);
    pthread_exit((void *)parcela_prodint); //finaliza a thread e passa o endereço de retorno  
}

int main(int argc, char *argv[]){

    FILE *arq; //arquivo do qual leremos as informações

    int T; // número de threads
    long int N; //dimensão dos vetores de entrada
    size_t ret; //retorno da função de leitura no arquivo de entrada
    float prod_interno = 0; //valor final do produto interno entre os dois vetores
    float *auxiliar_prodinterno;
    float prod_original; // produto interno original
    float var_rel; //variacao relativa entre o calculado e o original
    double start, end, delta; //calculo de tempo

    pthread_t *tid; //vetor que guarda os identificadores das threads no sistema

    //verifica e recebe os argumentos
    if(argc<3){
        printf("Informe %s <número de threads> <nome do arquivo>\n", argv[0]);
        return 1;
    } 
    //abre o arquivo
    arq = fopen(argv[2], "rb");
    if(!arq){
        fprintf(stderr, "Erro de abertura de arquivo\n");
        return 2;
    }
    // atribui os valores dos parametros do arquivo e da linha de comando
    ret = fread(&N, sizeof(long int), 1, arq); //lê o tamanho dos vetores a partir do arquivo
    if(!ret){
        fprintf(stderr, "Erro na leitura das dimensões no arquivo\n");
        return 3;
    }
    T = atoi(argv[1]);
    if(T > N) T = N; //limita o número de threads
    //aloca espaço para os vetores e preenche os elementos a partir do arquivo
    vetor1 = (float *) malloc(N * sizeof(float));
    if(vetor1 == NULL){
        fprintf(stderr, "Erro de alocacao de memória");
        return 4;
    } 
    vetor2 = (float *) malloc(N * sizeof(float));
    if(vetor2 == NULL){
        fprintf(stderr, "Erro de alocacao de memória");
        free(vetor1);
        return 4;
    }
    ret = fread(vetor1, sizeof(float), N, arq);
    if(!ret){
        fprintf(stderr, "Erro na leitura do vetor 1 do arquivo\n");
        return 3;
    }
    ret = fread(vetor2, sizeof(float), N, arq);
     if(!ret){
        fprintf(stderr, "Erro na leitura do vetor 2 do arquivo\n");
        return 3;
    }

    //aloca espaço para o vetor de tid
    tid = (pthread_t *) malloc(T * sizeof(pthread_t));
    if(tid == NULL){
        fprintf(stderr,"Erro de alocacao de memória\n");
        free(vetor1);
        free(vetor2);
        return 4;
    }

    GET_TIME(start); //começa a contagem de tempo
    //trecho para criar as threads
    for(int i=0; i<T; i++){
        //aloca e preenche os valores da estrutura que guarda os argumentos a serem passados
        t_args *args;
        args = (t_args *) malloc(sizeof(t_args));
        if(args == NULL){
            fprintf(stderr, "Erro de alocacao de memória\n");
            return 4;
        }

        args->N = N;
        args->nthreads = T;
        args->id = i;

        //cria as threads
        if(pthread_create(&tid[i], NULL, ProdInterno, (void *)args)){
            fprintf(stderr, "Erro na criação da thread %d\n", i);
            return 5;
        }
    }

    //espera as threads terminarem e calcula o produto interno total a partir dos retornos recebidos
    for(int i=0; i<T; i++){
        if(pthread_join(tid[i], (void **)&auxiliar_prodinterno)){
            fprintf(stderr, "Erro de join da thread %d\n", i);
            return 5;
        }
        prod_interno += *auxiliar_prodinterno; //soma as parcelas das coordenadas ao produto interno geral
        free(auxiliar_prodinterno);
    }

    GET_TIME(end); //termina a contagem de tempo

    delta = end - start; // calcula o tempo que foi gasto pra calcular o prodinterno usando concorrencia

    //imprimir os resultados
    printf("Produto interno calculado de forma concorrente: %.26f\n\n", prod_interno);
    // le o valor do arquivo do resultado do produto interno
    ret = fread(&prod_original, sizeof(float), 1, arq);
    printf("Produto interno original: %.26f\n\n", prod_original);
    //calculo da variacao relativa
    var_rel = fabs((prod_original - prod_interno)/prod_original);
    printf("Variação relativa: %.26f\n\n", var_rel);

    //Cálculo do tempo
    printf("TEMPO: %lf\n", delta);

    //finalizar a main - desalocar memoria e fechar arquivo
    free(vetor1);
    free(vetor2);
    free(tid);
    fclose(arq);

    return 0;
}