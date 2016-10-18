//
//  PPMLoader.cpp
//  jpegenc
//
//  Created by Christian Braun on 18/10/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load() {
    
    FILE* file = fopen(this->file, "r");
    
    /*if(file == nullptr) {
        std::cout << "File does not exist" << std::endl;
        
        return Image(0, 0);
    }*/
    
    char magicNumber[255];
    fscanf(file, "%s\n", magicNumber);
    
    
    int width, height, maxValue;
    
    fscanf(file, "%d %d\n", &width, &height);
    fscanf(file, "%d\n", &maxValue);
    
    auto image = std::make_shared<Image>(width, height);
    
    size_t index = 0;
    
    while(1){
        
        int r,g,b;
        
        int elementsRead = fscanf(file, "%d %d %d", &r, &g, &b);
        
        if(elementsRead < 3){
            break;
        }
        
        image->setPixel(index++, Pixel(r,g,b));
    }
    
    return image;
}
