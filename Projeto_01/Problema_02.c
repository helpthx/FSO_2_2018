#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include <time.h>

/* Alunos/Matricula:  Brian Lui                 15/0006802 
                      Joao Vitor Rodrigues      15/0013329
                      Lucas Machado Martins     15/0015917
*/
int num_alunos, num_cadeiras;

sem_t customers;                // semaforo
sem_t sem_monitor;              // barreira
sem_t mutex;                    // para exclusão mútua
int waiting = 0;                // alunos na fila de espera

// protótipos
void* monitor(void *arg);
void* student(void *param);

struct ARG{
  int id;
  int num_atendido;
};

int main() {

  srand(time(NULL));
  num_alunos = rand()%41;

  if (num_alunos < 3){
    num_alunos = 3;
  }

  num_cadeiras = num_alunos / 2;

  printf("Quantidade de alunos: %d\n", num_alunos);
  printf("Quantidade de cadeiras: %d\n\n", num_cadeiras);

  sem_init(&customers, 1, 0);
  sem_init(&sem_monitor, 1, 0);
  sem_init(&mutex, 1, 1);

  pthread_t AE;

  pthread_t t_aluno[num_alunos];

  pthread_create(&AE, NULL, (void *) monitor, NULL);

  int i;

  struct ARG args[num_alunos];

  for (i=0;i<=num_alunos;i++)
    args[i].num_atendido = 0;

  while(1) {
    int aleatorio;
    srand(time(NULL));
    aleatorio = rand()%num_alunos+1;

    args[aleatorio].id = aleatorio;
    pthread_create(&t_aluno[aleatorio], NULL, (void *) student, &args[aleatorio]);
    sleep(1);
  }

  return 0;
}

void* monitor(void *arg) {
  while(1) {
    sem_wait(&customers);   // vai dormir se o número de estudantes for 0
    sem_wait(&mutex);       // obtém acesso a 'waiting'

    waiting = waiting - 1;  //descresce o contador de clientes à espera

    sem_post(&sem_monitor);     // o monitor está agora disponível para cortar cabelo
    sem_post(&mutex);       // libera 'waiting'

    // monitor pode atender (fora da região crítica)
    printf("Monitor está livre agora.\n\n");
    sleep(1);
  }

  pthread_exit(NULL);
}

void* student(void *param) {
  struct ARG *arg = (struct ARG*)param;
  if (arg->num_atendido == 3){
    pthread_exit(NULL);
  }
  int aleatorio;

  printf("Numero de vezes que o aluno %d conseguiu ser atendido: %d.\n\n",arg->id, arg->num_atendido);

  srand(time(NULL));
  aleatorio = rand();

  if (aleatorio%2 == 0){
    // Aluno busca ajuda
    arg->num_atendido = arg->num_atendido+1;
    sem_wait(&mutex);           // entra na região crítica

    if(waiting < num_cadeiras) {      // se não houver cadeiras vazias, saia
      printf("Aluno %d entrou na fila.\n\n", arg->id);
      waiting = waiting + 1;  // incrementa o contador de alunos à espera
      printf("Alunos na fila: %d. \n\n", waiting);

      sem_post(&customers);   // acorda o monitor se necessário
      sem_post(&mutex);       // libera o acesso a 'waiting'
      sem_wait(&sem_monitor);     // vai programar se o monitor estiver dormindo

      printf("O aluno %d esta sendo atendido pelo monitor\n\n", arg->id);

    } else {
      sem_post(&mutex);
      printf("Não hà cadeiras disponiveis, aluno %d voltou a programar\n\n", arg->id);
    }

    pthread_exit(NULL);

  }else{
    //Aluno volta a programar por um tempo aleatorio
    printf("Aluno %d está programando.\n\n", arg->id);
    int tempo_prog = rand()%5;
    sleep(tempo_prog);

    pthread_exit(NULL);
  }

}
