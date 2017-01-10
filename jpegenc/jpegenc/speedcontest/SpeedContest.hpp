#ifndef SpeedContest_hpp
#define SpeedContest_hpp

class SpeedContest {
public:
	static void run(double seconds, bool skipCPU = false, bool skipGPU = false);
	static void testForCorrectness(bool ourTestMatrix = false, bool use16x16 = false, bool modifyData = false);
};

#endif /* SpeedContest_hpp */
