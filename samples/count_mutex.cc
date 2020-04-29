#include <pthread.h>
#include <iostream>

typedef void *tmain(void *);

struct counter {
  long count;
  pthread_mutex_t mutex;
};

long increment(counter *v) {
  long i;
  for (i=0; i<1000000; i++) { 
    pthread_mutex_lock(&v->mutex);
    long tmp = v->count;
    v->count = tmp + 1;
    pthread_mutex_unlock(&v->mutex);
  }   
  return i;
}

int main(int argc, char **argv) {

  // Set up the shared data.
  counter c;
  c.count = 0;
  pthread_mutex_init(&c.mutex,NULL);

  // Start the threads.
  pthread_t id1, id2;
  pthread_create(&id1,NULL,(tmain *)increment,(void *)&c);
  pthread_create(&id2,NULL,(tmain *)increment,(void *)&c);

  // Wait for each thread to complete its work.
  long r1, r2;
  pthread_join(id1,(void **)&r1);
  pthread_join(id2,(void **)&r2);

  // Report the results.
  std::cout << "Thread 1 incremented " << r1 << " times." << std::endl;
  std::cout << "Thread 2 incremented " << r2 << " times." << std::endl;
  std::cout << "The final count is " << c.count << "." << std::endl;
  pthread_exit(NULL);
}

