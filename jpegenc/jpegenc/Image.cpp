//
//  Image.cpp
//  jpegenc
//
//  Created by Marcel Groß on 18.10.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "Image.hpp"

struct Pixel {
    int r, g, b;
    
    Pixel(int r=0, int g=0, int b=0) : r(r), g(g), b(b) {}
};

struct Image {
    size_t width, height;
    Pixel* pixels;
    
    Image(size_t width, size_t height) : width(width), height(height) {
        pixels = (Pixel*) malloc(sizeof(Pixel) * width * height);
    }
    
    ~Image() {
        delete[] pixels;
    }
    
    Pixel getPixel(size_t x, size_t y) {
        if(x >= width) {
            x = width-1;
        }
        if(y >= height) {
            y = height-1;
        }
        
        return pixels[x + width * y];
    }
};