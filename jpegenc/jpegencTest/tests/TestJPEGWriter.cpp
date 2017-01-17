//
//  TestJPEGWriter.cpp
//  jpegenc
//
//  Created by Marvin Therolf on 16/01/2017.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../../jpegenc/io/PPMLoader.hpp"
#include "../../jpegenc/model/Image.hpp"
#include "../../jpegenc/converter/RGBToYCbCrConverter.hpp"
#include "../../jpegenc/segments/JPEGSegments.hpp"

TEST_CASE("TestJPEGWriter", "[jpegwriter]") {
    PPMLoader ppmLoader;
    auto image = ppmLoader.load("data/very_small.ppm");
    
    RGBToYCbCrConverter converter;
    converter.convert(image);
    
    JPEGSegments::JPEGWriter writer(image);
    writer.writeJPEGImage("data/very_small.jpg");
}
