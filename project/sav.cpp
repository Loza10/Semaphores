#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

mutex locker;
sem_t need_filled, is_filled;
int servings_available = 1;
int M = 5;

void *cook_action(void *noparam) {
  while (true) {
    sem_wait(&need_filled);
    servings_available = M;
    printf("Cook refilled pot!\n");
    sem_post(&is_filled);
  }
}

void *savage_action(void *noparam) {
  int t;
  while (true) {
    locker.lock();
    if (servings_available <= 0) {
      printf("Pot is empty!\n");
      sem_post(&need_filled);
      sem_wait(&is_filled);
    }
    servings_available--;
    locker.unlock();
    t = (int)rand() % 10;
    printf("Savage took 1 serving from the pot and is eating, leaving %d "
           "servings\n",
           servings_available);
    sleep(t);
  }
}
int main(int argc, char *argv[]) {
  if (argc == 2) {
    M = atoi(argv[1]);
  }
  sem_init(&need_filled, 0, 0);
  sem_init(&is_filled, 0, 0);
  pthread_t cookThread;
  pthread_t savThread[10];
  for (int i = 0; i < 10; i++) {
    if (pthread_create(&savThread[i], 0, savage_action, 0)) {
      perror("error creating the thread");
      exit(1);
    }
  }
  if (pthread_create(&cookThread, 0, cook_action, 0)) {
    perror("error creating the thread");
    exit(1);
  }
  pthread_join(cookThread, 0);
  for (int i = 0; i < 10; i++) {
    pthread_join(savThread[i], 0);
  }
  pthread_join(cookThread, 0);
}
