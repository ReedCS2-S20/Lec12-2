# Lecture 12-2: THREADS

This lecture's `samples` folder contains several examples of writing
multi-threaded code in C++. I use two methods of creating threads
in these programs. 

### POSIX thread examples

The first method looks more like old-fashioned C code (excepting the
use of iostream) and uses the POSIX threads library.  These each
include the <pthread.h> header file. They are each as follows:

* `walkchew.cc`: creates two threads, one that "steps" and one that
"chomps".  Their output is interleaved because they are run
concurrently.

* `count.cc`: creates two threads that each attempt to increment an
integer stored in memory many many times. Because their access to this
memory is not synchronized, their full work is not reflected in the
final count.

* `count_mutex.cc`: does the same, except now each thread locks a
shared `mutex` variable before accessing the count. After incrementing,
a thread releases the mutex so that another thread can lock it. By
using this synchronization mechanism, each thread gets *mutually
exclusive* access to the section of code that performs the count
update. As a consequence, the final count reflects both threads'
total effort.

These can each be compiled in the same way, for example,

    g++ std=c++11 -o walkchew -lpthread walkchew.cc

will create an executable named `walkchew` that can be run.
Note the inclusion of the `-l` flag there to link in the POSIX
thread library `libpthread`.

### STL thread examples

I've also written these again using the C++ `<thread>` and `<mutex>`
Standard Template Libraries as `walkchew_stl.cc` and `count_stl.cc`.
Here, instead, if we have a lambda `f` of type `std::function<void(void)>`
we can start a thread that runs `f` with the code

    std::thread t {f};

This creates a thread object instance named `t` that runs concurrently
with the thread that "spawned" it. It runs `f` to completion, and that
completion can be checked by 

    t.join();

The line above blocks and waits until `t` is done.

The `count_stl.cc` program can run two ways. If you compile it with

    g++ -std=c++11 -o countm -DLOCK count.cc

it will compile a version of the code that uses a `std::mutex` object
to synchronize access to the shared counter. If you instead compile it
with

    g++ -std=c++11 -o count count.cc

then it will not use a mutex and the count will be off.

NOTE: someone on-line claims that the `std::thread` objects are a cover
for the POSIX threads library. While this doesn't surprise me, that same
posting claimed that I need to compile this C++ STL code with `lpthread`
like I did with the older code above. I did not need to do this on my
Mac OSX running g++, however.

NOTE: many `std::thread` examples on-line use a more general form of
spawning a thread to run a fuinction that takes arguments, something like

    std::thread t {f,a1,a2,...,an};

I could not get my old Mac OSX running g++ to get the template types
figured out so all my thread creations take a `std::function<void(void)`.

### Parallelism with Pi

Last but not least is an example of using threads to perform a large 
calculation more quickly by running several concurrent threads, with
the hope that the hardware can run several *in parallel*. 

The code `pi.cc` contains STL code using `<thread>` and `<mutex>`
that computes an approximation of $\pi$ in a compute-intensive way.
In essence, it throws darts randomly at a 2x2 square and counts
what proportion of those darts hit a radius 1 dartboard centered in
that square. Since the area of that circle is $\pi$ and the area of
the square is 4, computing

    4 * hits / throws

should be about 3.14.

You can compile this code with

    g++ -std=c++11 -o pi pi.cc

and run it with a line like

    ./pi 10000 1

This will make 10000 throws and use a single thread to count how
many hit the dartboard. Running

    ./pi 10000 4

will do the same but use 4 threads to compute the final count.  They
count hits individually, then lock a mutex to add their count to the full
tally. Only the single `main` thread throws the darts, but then a bunch
of "worker" threads perform the tallying.

I've written the code so that it times how long the multi-threaded
tallying takes to complete. For large numbers of throws (100000 or
more) there is definitely a 2x speedup on my old Mac when I switch
from using one thread to using two threads. But then 3 or more threads
only gives a slight additional speedup. I'm not sure why this is the
case as, on-line, my processor seems documented to have 4
hyperthreaded cores. The document suggests that I should get a speedup
with 2 to 8 threads. 

I wrote a different program `par.cc` that doesn't access a vector and
that still does not get much additional speedup over 8 threads. I'm
flummoxed as to why.
