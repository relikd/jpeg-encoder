#ifndef Test_hpp
#define Test_hpp

#include <functional>
#include <chrono>

class Timer {
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<second_>
		(clock_::now() - beg_).count(); }
	
private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


#define OPERATIONS_IN_TIME(__x__) (const bool &shouldRun){ size_t iters = 0; while (shouldRun) { __x__; ++iters; } return iters; }

class Test {
public:
	static void howManyOperationsInSeconds(size_t seconds, const char* description, std::function<size_t(const bool &shouldExecute)> func);
	static void performance(std::function<void()> func);
	static void performance(const size_t iterations, const size_t rounds, std::function<void(size_t numberOfElements)> func);
};

#endif /* Test_hpp */
