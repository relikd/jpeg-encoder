#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>
#include "Mat.hpp"

struct Arai
{
public:
    static Mat transform(Mat input);
    static void transformLine(float *values);
private:
    static void processColumns(Mat input);
    static void processRows(Mat input);
};

#endif /* Arai_hpp */
