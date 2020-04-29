#include <thread>
#include <mutex>
#include <iostream>
#include <functional>

std::function<void(void)> incrementWith(int num_times, 
					  bool using_lock,
					  std::shared_ptr<long> counter, 
					  std::shared_ptr<std::mutex> mutex,
					  long& mycount) {
  if (using_lock) {
    return [num_times, counter, mutex, &mycount](void) -> void {
      long i;
      for (i=0; i<num_times; i++) { 
	mutex->lock();
	long tmp = *counter;
	(*counter) = tmp + 1;
	mutex->unlock();
      }   
      mycount  = i;
    };
  } else {
    return [num_times, counter, &mycount](void) -> void {
      long i;
      for (i=0; i<num_times; i++) { 
	long tmp = *counter;
	(*counter) = tmp + 1;
      }   
      mycount  = i;
    };
  }
}

int main(int argc, char **argv) {

  // Set up their shared data.
  std::shared_ptr<std::mutex> mutex {new std::mutex {}};
  std::shared_ptr<long> counter {new long{0}};
  
  // Start both threads and wait for them to each complete.
  long count1;
  long count2;
#ifndef LOCK
  std::thread t1 {incrementWith(100000,false,counter,mutex,count1)};
  std::thread t2 {incrementWith(100000,false,counter,mutex,count2)};
#endif
#ifdef LOCK
  std::thread t1 {incrementWith(100000,true,counter,mutex,count1)};
  std::thread t2 {incrementWith(100000,true,counter,mutex,count2)};
#endif
  t1.join();
  t2.join();
    
  // Report the results.
  std::cout << "Thread 1 incremented " << count1 << " times." << std::endl;
  std::cout << "Thread 2 incremented " << count2 << " times." << std::endl;
  std::cout << "The final count is " << (*counter) << "." << std::endl;
}

