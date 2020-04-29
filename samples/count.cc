#include <pthread.h>
#include <iostream>

typedef void *tmain(void *);

long increment(int *v) {
  long i;
  for (i=0; i<1000000; i++) { 
    long tmp = (*v);
    (*v) = tmp + 1;
  }   
  return i;
}

int main(int argc, char **argv) {

  // Set up the shared data.
  int count;

  // Start the threads.
  pthread_t id1, id2;
  pthread_create(&id1,NULL,(tmain *)increment,(void *)&count);
  pthread_create(&id2,NULL,(tmain *)increment,(void *)&count);

  // Wait for each thread to complete its work.
  long r1, r2;
  pthread_join(id1,(void **)&r1);
  pthread_join(id2,(void **)&r2);

  // Report the results.
  std::cout << "Thread 1 incremented " << r1 << " times." << std::endl;
  std::cout << "Thread 2 incremented " << r2 << " times." << std::endl;
  std::cout << "The final count is " << count << "." << std::endl;
  pthread_exit(NULL);
}

