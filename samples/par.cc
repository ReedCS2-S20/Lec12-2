// Reed CSCI 221
//
// par.cc
//

#include <iostream>
#include <vector>
#include <memory>     // for shared_ptr to the lock and the total count
#include <thread>     // for threads
#include <mutex>      // for synchronization with a lock
#include <functional> // for each thread's work function
#include <random>     // for generating random double values
#include <chrono>     // for timing the computation

//
// Work item for each thread.
//
struct work {
  // Identify this work item.
  int item_id;

  // Delineate work to be calculated.
  long amount;
  long start;

  // Keep a shared place to store the result.
  std::shared_ptr<std::mutex> mutex;
  std::shared_ptr<long> sum;
};


//
// Produces a work function for each work item.
//
std::function<void(void)> computeOn(work &w) {
  return [&w](void)->void {
    long amnt = w.amount;
    long strt = w.start;
    long sum = 0;
    for (long i = 0; i < amnt; i++) {
      sum += (strt + i) * (strt + i);
    }
    w.mutex->lock();
    *(w.sum) += sum;
    //std::cout << "Thread #" << w.item_id << " is done." << std::endl;
    w.mutex->unlock();
  };
}

// Performs the calculation from 1 to `n` using `nt` threads.
// Usage:
//
//     ./par <ns <nt>
//
int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <number of samples> <number of threads>" << std::endl;
    return -1;
  }

  long NUM_VALUES  = std::atol(argv[1]);
  int  NUM_THREADS = std::atoi(argv[2]);
  int PER_THREAD  = NUM_VALUES / NUM_THREADS;
  int SLOP        = NUM_VALUES % NUM_THREADS;

  // Create the shared lock and result.
  //
  std::shared_ptr<std::mutex> lock {new std::mutex {}};
  std::shared_ptr<long> sum {new long {0}};

  // Divide up the total work into several jobs.
  //
  std::vector<work> jobs {};  
  long where = 0;
  for (int t=0; t<NUM_THREADS; t++) {

    work w;
    w.item_id = t;
    w.amount  = t < SLOP ? PER_THREAD+1 : PER_THREAD;
    w.start = where;
    w.mutex  = lock; 
    w.sum  = sum;
    jobs.push_back(w);

    where  += w.amount;
  }

  // Start the timer.
  //
  auto start_time = std::chrono::high_resolution_clock::now(); 

  // Start each thread!
  //
  std::vector<std::thread> workers {};
  for (work& w : jobs) {
    workers.push_back(std::thread {computeOn(w)}); 
  }

  // Wait for each thread to complete its work.
  //
  for (std::thread& t : workers) {
    t.join();
  }

  // Stop the timer.
  //
  auto end_time = std::chrono::high_resolution_clock::now(); 

  // Output the result.
  //
  std::cout << "The total sum was " << (*sum) << "." << std::endl;

  // Report the time taken.
  //
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  std::cout << "That took me " << duration.count()/static_cast<double>(1000000) << " seconds." << std::endl;

  return 0;
}

