#ifndef Performance_hpp
#define Performance_hpp

#include <functional>
#include <chrono>

#define PerformancePrintOperationsPerSecond(__desc, __time, __count) \
printf("Testing <%s> took %lf seconds with %lu iterations (%lfms per operation)\n", __desc, __time, (unsigned long)__count, (__time / __count) * 1000);


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
	static void howManyOperationsInSeconds(const double seconds, const char* description, std::function<void()> func, bool multiThreadingEnabled = false);
	static void compareExecutionTime(const unsigned long iterations, const char* description, std::function<void()> funcA, std::function<void()> funcB);
	static void time(std::function<void()> func);
	static void repeat(const unsigned long iterations, const unsigned long rounds, std::function<void(unsigned long numberOfElements)> func);
};

#endif /* Performance_hpp */
