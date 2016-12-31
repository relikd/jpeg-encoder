#ifndef Performance_hpp
#define Performance_hpp

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


class Performance {
public:
	static void howManyOperationsInSeconds(size_t seconds, const char* description, std::function<void()> func, bool multiThreadingEnabled = false);
	static void compareExecutionTime(size_t iterations, const char* description, std::function<void()> funcA, std::function<void()> funcB);
	static void time(std::function<void()> func);
	static void repeat(const size_t iterations, const size_t rounds, std::function<void(size_t numberOfElements)> func);
};

#endif /* Performance_hpp */
