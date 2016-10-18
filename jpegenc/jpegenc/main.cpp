//
//  main.cpp
//  jpegenc
//
//  Created by Oleg on 18/10/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include <iostream>
#include "PPMLoader.hpp"
#include "Image.hpp"

int main(int argc, const char * argv[]) {
	// insert code here...
	std::cout << "Hello, World!\n";
    
    PPMLoader loader("example.ppm");
    
    auto image = loader.load();
    
    image->print();
    return 0;
}
