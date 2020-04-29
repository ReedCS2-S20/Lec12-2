// Reed CSCI 221
//
// pi.c
//
// Compile with:
//   g++ -std=c++11 -o pi -lpthread pi.cc
//
// Run with:
//   ./pi <NUM_SAMPLES> <NUM_THREADS>
//
// Example run:
//     > ./pi 10000 1
//     The total count was 31520.
//     That took me 0.000311 seconds.
//     >
//
// This is multithreaded code that (in essence) computes the value of
// pi. It does so by generating NUM_SAMPLES random points in the
// rectangle [-1.0,1.0] x [-1.0,1.0] and then counting how many of
// those points fall within a circle of radius 1.0.
//
// An approximation of pi := 4 * count / NUM_SAMPLES
//
// This program outputs 4 * count. 
// If you choose NUM_SAMPLES to be a power of 10 then the count will
// look like the digits of pi.
//
// This breaks up the counting task into several thread subtasks.
//
// If you have enough cores, then you'll see decent speedup for
// large sample sizes.
//
// SOME ADDITIONAL NOTES:
//
//  * I spawned the threads in this strange way with a `computeOn`
//    function that returns a functional of type void(void). This is
//    because I could not get my g++ compiler to handle a thread
//    constructed with a functional of type void(work&) handed an
//    work& argument.
//
//  * My old laptop seems to only have two cores, so I could not test
//    to see if I got great speedups with 3 or more cores.  Using more
//    than two threads gave me *some* additional speedup.
//
//  * I decided to make vectors and then count their contents, just so
//    I could show off dividing up the data. If I actually just wanted
//    to compute pi in parallel, I would have instead had each thread
//    do its own random point generation and use no memory.
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
  int amount;
  std::vector<double>::iterator startx;
  std::vector<double>::iterator starty;

  // Keep a shared place to store the result.
  std::shared_ptr<std::mutex> mutex;
  std::shared_ptr<long> count;
};


//
// Produces a work function for each work item.
//
std::function<void(void)> computeOn(work &w) {
  return [&w](void)->void {
    int mycount = 0;
    auto i = w.startx;
    auto j = w.starty;
    for (int todo = w.amount; todo > 0; todo--) {
      double x = (*i);
      double y = (*j);
      if ((x*x + y*y) <= 1.0) mycount++;
      i++;
      j++;
    }
    
    w.mutex->lock();
    *(w.count) += mycount;
    //std::cout << "Thread #" << w.item_id << " is done." << std::endl;
    w.mutex->unlock();
  };
}

//
// Fill a vector with random double values between min and max.
//
void randomly_fill(std::vector<double>& vec, double min, double max) {
  std::random_device rd;  
  std::mt19937 gen(rd()); 
  std::uniform_real_distribution<double> rand {min,max};
  for (double& e: vec) {
    e = rand(gen);
  }
}

// Performs the calculation using `ns` samples and `nt` threads.
// Usage:
//
//     ./pi <ns> <nt>
//
int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <number of samples> <number of threads>" << std::endl;
    return -1;
  }

  long NUM_SAMPLES = std::atol(argv[1]);
  int  NUM_THREADS = std::atoi(argv[2]);
  int PER_THREAD  = NUM_SAMPLES / NUM_THREADS;
  int SLOP        = NUM_SAMPLES % NUM_THREADS;

  // Initialize all the data
  //
  std::vector<double> xs {};
  xs.resize(NUM_SAMPLES,0.0);
  std::vector<double> ys {};
  ys.resize(NUM_SAMPLES,0.0);
  //
  randomly_fill(xs,-1.0,1.0);
  randomly_fill(ys,-1.0,1.0);
  //
  //
  // The code above fills each array with a collection of random
  // values uniformly distributed between -1.0 and 1.0.
  //
  // Treated as a set of coordinate pairs { (xs[i],ys[i]) }
  // we can count how many appear within the unit circle.
  // 
  // 4 * count / NUM_SAMPLES should be about PI.
  //

  // Create the shared lock and result.
  //
  std::shared_ptr<std::mutex> lock {new std::mutex {}};
  std::shared_ptr<long> count {new long {0}};

  // Divide up the total work into several jobs.
  //
  std::vector<work> jobs {};  
  int where = 0;
  for (int t=0; t<NUM_THREADS; t++) {

    work w;
    w.item_id = t;
    w.amount  = t < SLOP ? PER_THREAD+1 : PER_THREAD;
    w.startx = xs.begin() + where;
    w.starty = ys.begin() + where;
    w.mutex  = lock; 
    w.count  = count;
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
  std::cout << "The total count was " << 4 * (*count) << "." << std::endl;

  // Report the time taken.
  //
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  std::cout << "That took me " << duration.count()/static_cast<double>(1000000) << " seconds." << std::endl;

  return 0;
}

