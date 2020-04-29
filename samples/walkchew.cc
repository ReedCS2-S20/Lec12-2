#include <pthread.h>
#include <iostream>

typedef void *tmain(void *);

void *walk(long *v) {
  (*v) = 0;
  for (long i=100000; i>0; i--, (*v)++) { std::cout << "step #" << *v << "\n"; }   
  return NULL;
}

void *chewGum(long *v) {
  (*v) = 0;
  for (long i=100000; i>0; i--, (*v)++) { std::cout << "chomp #" << *v << "\n"; }   
  return NULL;
}

int main(int argc, char **argv) {
  pthread_t id1, id2;
  long a1, a2, r1, r2;
  pthread_create(&id1,NULL,(tmain *)walk,   (void *)&a1);
  pthread_create(&id2,NULL,(tmain *)chewGum,(void *)&a2);
  pthread_join(id1,(void **)&r1);
  pthread_join(id2,(void **)&r2);
  std::cout << "Number of completed steps: " << a1 << " ...chomps: " << a2 << std::endl;
  pthread_exit(NULL);
}

