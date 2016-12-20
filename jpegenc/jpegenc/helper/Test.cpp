#include "Test.hpp"
#include <stdio.h>
#include <thread>

//  ---------------------------------------------------------------
// |
// |  Performance
// |
//  ---------------------------------------------------------------
void Test::howManyOperationsInSeconds(size_t seconds, const char* description, std::function<size_t(bool &shouldExecute)> func) {
	bool runWhile = true;
	
	std::thread([seconds, &runWhile]() {
		std::this_thread::sleep_for(std::chrono::seconds(seconds));
		runWhile = false;
	}).detach();
	
	Timer t;
	size_t iters = func(runWhile);
	printf("Testing <%s> took %lf seconds with %lu iterations (%lfms per operation)\n", description, t.elapsed(), iters, t.elapsed() / iters * 1000);
}

void Test::performance(std::function<void()> func) {
	performance(1, 1, [&func](size_t numberOfElements){func();});
}

void Test::performance(const size_t iterations, const size_t rounds, std::function<void(size_t numberOfElements)> func) {
	size_t r = rounds;
	size_t it = iterations;
	
	Timer t;
	while (r--) {
		func(it);
		it = iterations;
	}
	double averageTime = t.elapsed() / rounds;
	
	// generate output
	if (iterations > 1)
		printf("%lu iterations, ", iterations);
	
	printf("%lf seconds (%lfms)", averageTime, averageTime*1000);
	
	if (rounds > 1)
		printf(" on average (%lu times)", rounds);
	printf(".\n");
}

