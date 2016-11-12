#ifndef Test_hpp
#define Test_hpp

#include <functional>

class Test {
public:
	static void performance(std::function<void()> func);
	static void performance(const size_t iterations, const size_t rounds, std::function<void(size_t numberOfElements)> func);
};

#endif /* Test_hpp */
