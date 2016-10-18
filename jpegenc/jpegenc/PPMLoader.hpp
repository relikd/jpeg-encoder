//
//  PPMLoader.hpp
//  jpegenc
//
//  Created by Christian Braun on 18/10/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <stdio.h>
#include "Image.hpp"


struct PPMLoader {
    const char* file;
    
    PPMLoader(const char* file): file(file) {}
    
    Image* load();
};
#endif /* PPMLoader_hpp */
