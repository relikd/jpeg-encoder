#include <iostream>
#include "Image.hpp"

Pixel Image::getPixel(size_t x, size_t y) {
	if(x >= width) {
		x = width-1;
	}
	if(y >= height) {
		y = height-1;
	}
	
	return pixels[getIndex(x, y)];
};

void Image::setPixel(size_t x, size_t y, Pixel pixel){
	if (x >= width || y >= height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	
	pixels[getIndex(x, y)] = pixel;
};

void Image::setPixel(size_t index, Pixel pixel) {
	if (index >= width * height) {
		std::cout << "Cant set pixel out of range " << index << std::endl;
		return;
	}
	
	pixels[index] = pixel;
};

size_t Image::getIndex(size_t x, size_t y) const {
	return x + width * y;
};

void Image::print() const {
	for(int i = 0; i < width * height; ++i){
		if (i % width == 0) {
			std::cout << std::endl;
		}
		
		std::cout << pixels[i].r << "\t" << pixels[i].g << "\t" << pixels[i].b << "\t\t";
	}
}


