#include "Performance.hpp"
#include <stdio.h>
#include <float.h>
#include <thread>
#include <atomic>

/**
 * Repeat the same lambda expression for a defined amout of seconds and print out
 * the number of iterations as well as the execution time for a single operation.
 *
 * @param seconds Time in seconds (How long should @b func be executed)
 * @param description Short description for this Performance Test. Will be printed to the console
 * @param func Lambda expression or Functor class to be profiled (simply use \a[&]{code} )
 *
 * @param multiThreadingEnabled If \b true automatically detach new threads for all iterations (default: false)
 */
void Performance::howManyOperationsInSeconds(double seconds, const char* description, std::function<void()> func, bool multiThreadingEnabled) {
	std::atomic<unsigned long> iters(0); // Use GCC 4.7+ (GCC 4.6 atomics are not lock free)
	std::atomic<unsigned int> threadsRunning(0);
	static const unsigned int threadCount = std::thread::hardware_concurrency();
	
	Timer t;
	if (multiThreadingEnabled)
	{
		while (t.elapsed() < seconds) {
			if (threadsRunning < threadCount) {
				++threadsRunning;
				std::thread([&]{
					func();
					++iters;
					--threadsRunning;
				}).detach();
			}
		}
	}
	else // single core (And single thread? Or does C++ optimize for multi-thread automatically?)
	{
		while (t.elapsed() < seconds) {
			func();
			++iters;
		}
	}
	double time = t.elapsed();
	unsigned long numberOfIterations = iters;
	PerformancePrintOperationsPerSecond(description, time, numberOfIterations);
	
	// wait till all previous threads are finished
	while (threadsRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

/**
 * Repeat two different algorithms with the same iteration count and print out which one is faster.
 * Print out will always compare \b funcB in relation to \b funcA
 * (if funcA is faster than funcB value will be less than 1.0x)
 *
 * @param iterations How often to repeat both functions
 * @param description Short description for this Test. Will be printed to the console
 * @param funcA Lambda expression or Functor class to be compared (simply use \a[&]{code} )
 * @param funcB Lambda expression or Functor class to be compared (simply use \a[&]{code} )
 */
void Performance::compareExecutionTime(size_t iterations, const char* description, std::function<void()> funcA, std::function<void()> funcB) {
	
	size_t repeatA = iterations;
	Timer tA;
	while (repeatA--) {
		funcA();
	}
	double timeA = tA.elapsed();
	
	size_t repeatB = iterations;
	Timer tB;
	while (repeatB--) {
		funcB();
	}
	double timeB = tB.elapsed();
	
	printf("Testing <%s> Algorithm B is %1.2fx faster (%1.4f, %1.4f)\n", description, timeA / timeB, timeA, timeB);
}

/**
 * Run the provided code once and print out the execution time
 * @param func Lambda expression or Functor class to be profiled (simply use \a[&]{code} )
 */
void Performance::time(std::function<void()> func) {
	repeat(1, 1, [&func](size_t numberOfElements){func();});
}

/**
 * Run the provided code multiple times and print out the fastest iteration.
 * For further info why it's not avarage time, refer to the implementation body.
 *
 * Use this code inside your Functor:
 @code
 [](size_t numberOfElements){
   <#init#>
   while (numberOfElements--) {
     <#code#>
   }
 }
 @endcode
 *
 * @param iterations How often should \b func be executed in one round
 * @param rounds Multiple rounds will increase the chance for a better time
 * @param func Lambda expression or Functor class to be profiled (simply use \a[&]{code} )
 */
void Performance::repeat(const size_t iterations, const size_t rounds, std::function<void(size_t numberOfElements)> func) {
	size_t r = rounds;
	
	double fastest = DBL_MAX;
	while (r--) {
		Timer t;
		size_t it = iterations;
		func(it);
		double time = t.elapsed();
		
		if (time < fastest) {
			// We use the shortest elapsed time here instead of an avarage time over all iterations.
			// We want to evaluate the runtime of the core algorithm without OS dependent operations.
			// The thinking is that the algorithm will always run in the same amount of time but will
			// be interrupted by the operating system or other running application.
			// An average runtime may be useful for the end user, but not for the purpose of comparing
			// two different implementations for the same logical algorithm.
			fastest = time;
		}
	}
	
	// generate output
	if (iterations > 1)
		printf("%lu iterations, ", iterations);
	
	printf("%lf seconds (%lfms)", fastest, fastest*1000);
	
	if (rounds > 1)
		printf(" on average (%lu times)", rounds);
	printf(".\n");
}

