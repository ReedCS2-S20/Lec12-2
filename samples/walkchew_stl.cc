#include <thread>
#include <iostream>
#include <functional>

int main(void) {

  int steps  = 10000;
  int chomps = 10000;

  // Start both threads and wait for them to each complete.
  std::thread t1 {[steps](void)mutable->void {for (int i=0;i<steps;i++) std::cout << "step #" << i << "\n";}};
  std::thread t2 {[chomps](void)mutable->void {for (int i=0;i<chomps;i++) std::cout << "chomp #" << i << "\n";}};
  t1.join();
  t2.join();
  std::cout << "Number of completed steps: " << steps << " ...chomps: " << chomps << std::endl;
}
