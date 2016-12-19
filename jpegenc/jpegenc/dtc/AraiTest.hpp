#ifndef AraiTest_hpp
#define AraiTest_hpp

#include "Mat.hpp"

class AraiTest {
public:
	static Mat transform(Mat input);
	static void transformLine(float* &values);
private:
	static void processColumns(Mat input);
	static void processRows(Mat input);
};

#endif /* AraiTest_hpp */
