#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

int philosopher[5], t;
mutex locker;
sem_t phil_lock[5];

void put_fork(int i) {
  sem_post(&phil_lock[(i+4) % 5]);
  sem_post(&phil_lock[(i+1) % 5]);
}
void take_fork(int i) {
  locker.lock();
    sem_wait(&phil_lock[(i+4) % 5]);
    printf("Philosopher: %d picked up fork for %d\n", i, (i+4) % 5);
    sem_wait(&phil_lock[(i+1) % 5]);
    printf("Philosopher: %d picked up fork for %d\n", i, (i+1) % 5);
  locker.unlock();
}
void* philosophers(void* arg) {
  int i = *(int*)arg;
  while (true) {
    printf("Philosopher %d thinking\n", i);
    t = (int) rand() % 10;
    sleep(t);

    take_fork(i);

    printf("Philosopher %d eating\n", i);
    t = (int) rand() % 10;
    sleep(t);

    put_fork(i);
  }
}
int main() {
  for (int i = 0; i < 5; i++) {
    sem_init(&phil_lock[i], 0, 1);
  }
  pthread_t philosopherThreads[5];
  int args[5];
  for (int i = 0; i < 5; i++) {
    args[i] = i;
    if (pthread_create(&philosopherThreads[i], 0, philosophers, &args[i])) {
      perror("error creating the thread");
      exit(1);
    }
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(philosopherThreads[i], nullptr);
  }
}
