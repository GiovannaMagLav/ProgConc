/*
LAB 3 - atividade 4 (programa auxiliar)
Descrição: programa sequencial que gera dois vetores de entrada, do tipo float e tamanho N 
*/

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define MAX 1000
#define CORRETUDE

int main(int argc, char *argv[]){
    float *vetor1;
    float *vetor2;
    long int N; //tamanho do vetor
    float elem1, elem2; // elementos dos vetores 1 e 2
    int fator = 1; // para gerar números negativos
    FILE *arq; // arquivo binario com o valor de N, os dois vetores e o prod interno entre eles
    float parcela = 0; //usado para calcular o produto de cada par de coordendas
    float prod_interno = 0;
    size_t ret; // retorno das funcoes de arquivo
    
    //recebe os argumentos de entrada
    if(argc<3){
        fprintf(stderr, "ERRO. Formato de entrada esperado: <dimensao> <nome do arquivo de saída>\n");
        return 1;
    } 

    N = atoi(argv[1]);

    //aloca memória para os dois vetores
    vetor1 = (float *) malloc(N*sizeof(float));
    vetor2 = (float *) malloc(N*sizeof(float));
    if(!(vetor1)){
        fprintf(stderr, "Erro de alocação de memória para o vetor");
        return 2;
    }
    if(!(vetor2)){
        fprintf(stderr, "Erro de alocação de memória para o vetor");
        free(vetor1);
        return 2;
    }

    // preenche os vetores com valores reais aleatórios e calcula o produto interno entre eles
    srand(time(NULL));
    for(long int i=0; i<N; i++){
        elem1 = ((float)rand() / RAND_MAX) * 20.0 - 10.0;
        elem2 = ((float)rand() / RAND_MAX) * 20.0 - 10.0;
        //elem1 = (rand() % MAX)/3.0 * fator;
        vetor1[i] = elem1;
        //elem2 = (rand() % MAX)/2.5 * fator;
        vetor2[i] = elem2;
        fator*=-1;
        parcela = elem1 * elem2;
        prod_interno += parcela;

    }

    //cria e escreve em um arquivo binário o valor de N e os dois vetores
    arq = fopen(argv[2], "wb");
    if(!arq){
        fprintf(stderr, "Erro na abertura do arquivo\n");
        return 3;
    }
    ret = fwrite(&N, sizeof(long int), 1, arq); // escreve o valor de N
    if(!ret){
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }
    ret = fwrite(vetor1, sizeof(float), N, arq); // escreve o vetor 1
    if(ret < N){
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }
    ret = fwrite(vetor2, sizeof(float), N, arq); // escreve o vetor 2 no arquivo
    if(ret < N){
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }
    // escreve o resultado do produto interno no arquivo
    ret = fwrite(&prod_interno, sizeof(float), 1, arq);
    if(!ret){
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }

    //exibição do arquivo em formato de texto para verificar a corretude do programa
    #ifdef CORRETUDE
    fprintf(stdout, "%ld\n", N);
    for(long int i=0; i<N; i++) {
      fprintf(stdout, "%f ",vetor1[i]);
    }
    fprintf(stdout, "\n");
    for(long int i=0; i<N; i++){
        fprintf(stdout, "%f ", vetor2[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "%lf\n", prod_interno);
    #endif
    
    // fecha o arquivo e libera memória
    fclose(arq);
    free(vetor1);
    free(vetor2);

    return 0;

}