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
