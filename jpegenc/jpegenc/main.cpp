#include <iostream>
#include "PPMLoader.hpp"
#include "Image.hpp"

int main(int argc, const char * argv[]) {
	std::cout << "Hello, World!\n";
	
	PPMLoader loader("data/very_small.ppm");
	
	auto image = loader.load();
	
	image->print();
	return 0;
}
