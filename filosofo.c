#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define ESQUERDA (filosofo + numeroDeFilosofos - 1) % numeroDeFilosofos
#define DIREITA (filosofo + 1) % numeroDeFilosofos
#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1
#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2

int numeroDeFilosofos;
int tempoPensando;
int tempoComendo;
int tempoImpressora;

int* acao;

sem_t mutex;
sem_t* vetorMutex;

pthread_t threadImpressora;
pthread_t* threadFilosofo;

void numeroDeArgumentosEstaCorreto(int);
void criaFilosofos();
void printaAcaoDoFilosofo(int, int);
void verificaAcoesDosFilosofos();
void* filosofo();
void* impressora();
void pegaGarfos (int);
void largaGarfos (int);
void testa (int);
void pense(int);
void coma(int);


int main (int argc, char* argv[]) {   
  numeroDeArgumentosEstaCorreto(argc);
  
  sscanf(argv[1],"%d", &numeroDeFilosofos);
  sscanf(argv[2],"%d", &tempoPensando);
  sscanf(argv[3],"%d", &tempoComendo);
  sscanf(argv[4],"%d", &tempoImpressora);
  
  acao = malloc(sizeof(int) * numeroDeFilosofos);
  threadFilosofo = malloc(sizeof(pthread_t) * numeroDeFilosofos);
  vetorMutex = malloc(sizeof(sem_t) * numeroDeFilosofos);
    
  sem_init(&mutex, 0, 1);  
  
  pthread_create(&threadImpressora, NULL, &impressora, NULL );
  
  criaFilosofos();
  
  pthread_join(threadImpressora, NULL);    
  
  for (int i = 0; i < numeroDeFilosofos; i++) {
  	pthread_join(threadFilosofo[i], NULL);
  }
  
  return 0;
}


void numeroDeArgumentosEstaCorreto(int argumentos) {
   if(argumentos < 5) {
    printf("Modo de uso: ./jantar numeroDeFilosofos tempoPensando tempoComendo tempoImpressora\n");
    exit(EXIT_FAILURE);
  }
}


void criaFilosofos() {
  for (int i = 0; i < numeroDeFilosofos; i++) {
    sem_init(&(vetorMutex[i]), 0, 0);
    
    if (pthread_create(&(threadFilosofo[i]), NULL, &filosofo, NULL) != 0) {
      printf("Erro ao criar filosofo %d", i);
      exit(EXIT_FAILURE);
    }
    
    acao[i] = FAMINTO;
  }
}


void* filosofo() {  
  pthread_t currThread = pthread_self();  

  int i;
  for (i = 0; i < numeroDeFilosofos; i++) {
    if (threadFilosofo[i] == currThread) {
      break;
    }
  }

  int filosofo = i;
  
  while (1) {
    pense(filosofo);
    pegaGarfos(filosofo);
    coma(filosofo);
    largaGarfos(filosofo);
  }
}


void printaAcaoDoFilosofo(int filosofo, int acao) {
   switch (acao) {
      case COMENDO:
         printf("%d: C    ", filosofo);
         break;
      
      case FAMINTO:
         printf("%d: F    ", filosofo);
         break;

      case PENSANDO:
         printf("%d: P    ", filosofo);
         break;
   }     
}


void verificaAcoesDosFilosofos() {
   for(int i = 0; i < numeroDeFilosofos; i++) {
      printaAcaoDoFilosofo(i, acao[i]);
   }
}


void* impressora() {
  while(1) {
    sem_wait(&mutex);
    
    printf("_____________________________________________");
    printf("\n\n");

    verificaAcoesDosFilosofos();

    printf("\n");
    sem_post(&mutex);
    usleep(tempoImpressora * 500000);
  }
}


void largaGarfos (int filosofo) {
  sem_wait(&mutex);
  acao[filosofo] = PENSANDO;
  testa(ESQUERDA);
  testa(DIREITA);
  sem_post(&mutex);
}


void pegaGarfos (int filosofo) { 
  sem_wait(&mutex);
  acao[filosofo] = FAMINTO;
  testa(filosofo);
  sem_post(&mutex);
  sem_wait(&vetorMutex[filosofo]);
}


void pense (int filosofo) {
  //printf("O Filosofo %d está pensando\n", filosofo);
  sleep(tempoPensando);
}


void coma (int filosofo) {
  //printf("O Filosofo %d está comendo\n", filosofo);
  sleep(tempoComendo);
}


void testa (int filosofo) {
  if (acao[filosofo] == FAMINTO && 
      acao[ESQUERDA] != COMENDO && 
      acao[DIREITA] != COMENDO) {        
    acao[filosofo] = COMENDO;
    sem_post(&vetorMutex[filosofo]);
  }
}