#include <iostream>
#include <stdlib.h>
#include "SpeedContest.hpp"

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	
	// call with: dctCPU 5 -skip -valid
	// 5 : run for 5 seconds
	// -skip : skip single core test
	// -valid : test all implementations for correctness
	
	double testTime = 10.0F;
	bool skipSingeCore = false;
	
	int i = argc;
	while (--i) { // skip the first param, which is the path of this executable
		const char* param = argv[i];
		if (param[0] == '-') {
			if (strncmp(param, "-valid", 6) == 0) // -valid111 (three 1 for three bool parameter)
			{
				long validateParam = strtol(param + 6, NULL, 2);
				SpeedContest::testForCorrectness((bool)(validateParam & 1), (bool)(validateParam & 2), (bool)(validateParam & 4));
				exit(EXIT_SUCCESS);
			}
			else if (strcmp(param, "-skip") == 0)
			{
				skipSingeCore = true;
			}
		} else {
			double tmp = strtod(param, NULL); // see if time provided, otherwise default to 10
			if (tmp > 0.5) testTime = tmp;
		}
	}
	
	printf("Starting Performance Test with %1.1fs\n", testTime);
	SpeedContest::run(testTime, skipSingeCore);
	
	return 0;
}
