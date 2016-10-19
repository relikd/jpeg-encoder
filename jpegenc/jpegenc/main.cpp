#include <iostream>
#include "PPMLoader.hpp"

int main(int argc, const char *argv[]) {
	PPMLoader loader;
	auto image = loader.load("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/very_small.ppm");
	image->print();
	return 0;
}
