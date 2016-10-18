//
//  Image.hpp
//  jpegenc
//
//  Created by Marcel Groß on 18.10.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef Image_hpp
#define Image_hpp

#include <stdlib.h>
#include <stdio.h>

struct Pixel {
	size_t r, g, b;
	
	Pixel(size_t r=0, size_t g=0, size_t b=0) : r(r), g(g), b(b) {}
	Pixel(size_t r, size_t g, size_t b, size_t max, size_t new_max)
	: r(Pixel::new_upper_bound(r, max, new_max)),
	  g(Pixel::new_upper_bound(g, max, new_max)),
	  b(Pixel::new_upper_bound(b, max, new_max)) {}
	
	Pixel set_highest_to_255_from(size_t current) {
		r = new_upper_bound(r, current, (size_t)255);
		g = new_upper_bound(g, current, (size_t)255);
		b = new_upper_bound(b, current, (size_t)255);
		return *this;
	}
	
	template<class T>
	T new_upper_bound(T value, T old_bound, T new_bound) {
		return (T)( (value/(float)old_bound) * new_bound);
	}
};

struct Image {
	size_t width, height;
	Pixel* pixels;
	
	
	Image(size_t width, size_t height) : width(width), height(height) {
		pixels = new Pixel[width * height];
	}
	
	~Image() {
		delete[] pixels;
	}
	
	Pixel getPixel(size_t x, size_t y);
	
	void setPixel(size_t x, size_t y, Pixel pixel);
	
	void setPixel(size_t index, Pixel pixel);
	
	size_t getIndex(size_t x, size_t y) const;
	
	void print() const;

};
#endif /* Image_hpp */
