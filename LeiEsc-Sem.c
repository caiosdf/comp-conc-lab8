#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <semaphore.h>

#define L 4 //numero de threads leitoras
#define E 4 //numero de threads escritoras

//variaveis do problema
int l=0; //contador de threads lendo
int e=0; //contador de threads escrevendo

sem_t em_e, em_l, escr, leit; //semaforos

//entrada leitura
void InicLeit (int id) {
   sem_wait(&leit);
   sem_wait(&em_l);
   l++;
   printf("L[%d] quer ler\n", id);
   if(l == 1){
       sem_wait(&escr);
       printf("Bloqueou escrita\n");
   } 
   sem_post(&em_l);
   sem_post(&leit);
}

//saida leitura
void FimLeit (int id) {
   sem_wait(&em_l);
   l--;
   printf("L[%d] terminou de ler\n", id);
   if(l == 0){
       sem_post(&escr);
       printf("Desbloqueou escrita\n");
   } 
   sem_post(&em_l);
}

//entrada escrita
void InicEscr (int id) {
   sem_wait(&em_e);
   e++;
   printf("L[%d] quer escrever\n", id);
   if(e == 1){ 
       sem_wait(&leit);
       printf("Bloqueou leitura\n");
    }
   sem_post(&em_e);
   sem_wait(&escr);
}

//saida escrita
void FimEscr (int id) {
    sem_post(&escr);
    sem_wait(&em_e);
    e--;
    printf("L[%d] terminou de escrever\n", id);
    if(e == 0){
        sem_post(&leit);
        printf("Desbloqueou leitura\n");
    } 
    sem_post(&em_e);
}

//thread leitora
void * leitor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    InicLeit(*id);
    printf("Leitora %d esta lendo\n", *id);
    FimLeit(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

void * escritor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    InicEscr(*id);
    printf("Escritora %d esta escrevendo\n", *id);
    FimEscr(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //identificadores das threads
  pthread_t tid[L+E];
  int id[L+E];

  //inicializa os semaforos
  sem_init(&em_e, 0, 1);
  sem_init(&em_l, 0, 1);
  sem_init(&escr, 0, 1);
  sem_init(&leit, 0, 1);

  //cria as threads leitoras
  for(int i=0; i<L; i++) {
    id[i] = i+1;
    if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i])) exit(-1);
  } 
  
  //cria as threads escritoras
  for(int i=0; i<E; i++) {
    id[i+L] = i+1;
    if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L])) exit(-1);
  } 

  pthread_exit(NULL);
  return 0;
}