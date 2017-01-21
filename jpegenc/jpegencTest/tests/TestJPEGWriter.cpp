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
#include "../../jpegenc/dct/Arai.hpp"
#include "../../jpegenc/segments/ImageDataEncoding.hpp"


void printArray(float* data, int offset, int width, int height) {
	for (int i = offset; i < offset + width * height; ++i) {
		if (i % width == 0) {
			std::cout << std::endl;
		}
		std::cout << data[i] << "\t\t\t";
	}
	
	std::cout << std::endl;
}

TEST_CASE("TestJPEGWriter", "[jpegwriter]") {
    
    int from_image = 0;
    int to_image   = 76;

    for ( int i = from_image; i <= to_image; ++i )
    {
        std::ostringstream input_stream;
        input_stream << "../data/series/" << i << ".ppm";
        const char* input = input_stream.str().c_str();
        
        std::ostringstream output_stream;
        output_stream << "series/" << i << ".jpg";
        const char* output = output_stream.str().c_str();

        std::cout << "Converting " << input << " to " << output << std::endl;
        {
            PPMLoader ppmLoader;
            auto image = ppmLoader.load(input);
        
            RGBToYCbCrConverter converter;
            converter.convert(image);
        
            JPEGSegments::JPEGWriter writer(image);
            writer.writeJPEGImage(output);
        }
    }
    
    /*
	
	ChannelData* channelData = new ChannelData(image);
	channelData->unnormalize(255);
	
	std::cout << "Unnormalized image" << std::endl;
//	channelData->print(1);
//	channelData->print(2);
//	channelData->print(3);
	
	Arai::transform(channelData->channel1->values, 400, 400);
	Arai::transform(channelData->channel2->values, 400, 400);
	Arai::transform(channelData->channel3->values, 400, 400);
	
	std::cout << "After Arai" << std::endl;
//	channelData->print(1);
//	channelData->print(2);
//	channelData->print(3);
	
	auto qTable = Quantization::getLuminanceQT();
	auto q2Table = Quantization::getChrominanceQT();
	Quantization::run(channelData->channel1->values, 400, 400, qTable);
	Quantization::run(channelData->channel2->values, 400, 400, q2Table);
	Quantization::run(channelData->channel3->values, 400, 400, q2Table);
	
	std::cout << "After Quantization" << std::endl;
//	channelData->print(1);
//	channelData->print(2);
//	channelData->print(3);
	
	ImageDataEncoding encoding1(channelData->channel1->values, 400 , 400);
	ImageDataEncoding encoding2(channelData->channel2->values, 400 , 400);
	ImageDataEncoding encoding3(channelData->channel3->values, 400 , 400);
	
	encoding1.init();
	encoding2.init();
	encoding3.init();
	
	auto dc_encodings1 = encoding1.differenceEncoding();
	auto dc_encodings2 = encoding2.differenceEncoding();
	auto dc_encodings3 = encoding3.differenceEncoding();

	std::cout << "DC Encodings 1" << std::endl;

//	for (auto encoding : dc_encodings1) {
//		std::cout << encoding.numberOfBits << " " << encoding.code << std::endl;
//	}
//
//	std::cout << "DC Encodings 2" << std::endl;
//	for (auto encoding : dc_encodings2) {
//		std::cout << encoding.numberOfBits << " " << encoding.code << std::endl;
//	}
//	
//	std::cout << "DC Encodings 3" << std::endl;
//	for (auto encoding : dc_encodings3) {
//		std::cout << encoding.numberOfBits << " " << encoding.code << std::endl;
//	}
	
	
	std::vector<uint8_t> byteReps1;
	std::vector<uint8_t> byteReps2;
	std::vector<uint8_t> byteReps3;
	
	std::vector<Encoding> encodings1;
	std::vector<Encoding> encodings2;
	std::vector<Encoding> encodings3;
	
	encoding1.runLengthEncoding(byteReps1, encodings1);
	encoding2.runLengthEncoding(byteReps2, encodings2);
	encoding3.runLengthEncoding(byteReps3, encodings3);
	
	std::cout << "Print first Block" << std::endl;
	printArray(encoding1.sortedData, 64, 8, 8);

	std::cout << "AC Encoding 1" << std::endl;
	for (int i = 0 ; byteReps1[i] != 0; ++i) {
//		std::cout << encodings1[i] << std::endl;
		std::cout << std::hex << (int)byteReps1[i] << std::endl;
	}
	
//	std::cout << "AC Encoding 2" << std::endl;
//	for (int i = 0 ; i < encodings2.size(); ++i) {
//		std::cout << encodings2[i] << std::endl;
//		std::cout << std::hex << (int)byteReps2[i] << std::endl;
//	}
//	
//	std::cout << "AC Encoding 3" << std::endl;
//	for (int i = 0 ; i < encodings3.size(); ++i) {
//		std::cout << encodings3[i] << std::endl;
//		std::cout << std::hex << (int)byteReps3[i] << std::endl;
//	}

	*/
}
