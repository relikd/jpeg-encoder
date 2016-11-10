#include "Test.hpp"
#include <stdio.h>

//  ---------------------------------------------------------------
// |
// |  Performance
// |
//  ---------------------------------------------------------------

void Test::performance(std::function<void()> func, const size_t iterations) {
	performance(iterations, 1, []{}, func);
}

void Test::performance(const size_t iterations, const size_t rounds, std::function<void()> initOnce, std::function<void()> func) {
	clock_t timeStamp;
	clock_t totalTime = 0;
	size_t r = rounds;
	size_t it = iterations;
	
	while (r--) {
		timeStamp = clock();
		initOnce(); // eg. reset object to initial state
		while (it--)
			func();
		totalTime += clock() - timeStamp;
		it = iterations;
	}
	clock_t averageTime = totalTime / rounds;
	
	// generate output
	if (iterations > 1)
		printf("%lu iterations, ", iterations);

	printf("%lu clicks (%f seconds)", averageTime, ((float)averageTime)/CLOCKS_PER_SEC);
	
	if (rounds > 1)
		printf(" on average (%lu times)", rounds);
	printf(".\n");
}

