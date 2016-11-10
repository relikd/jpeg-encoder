#ifndef Test_hpp
#define Test_hpp

#include <functional>

class Test {
public:
	static void performance(std::function<void()> func, const size_t iterations = 1);
	static void performance(const size_t iterations, const size_t rounds, std::function<void()> initOnce, std::function<void()> func);
};

#endif /* Test_hpp */
