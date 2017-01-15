//
//  JPEGImage.cpp
//  jpegenc
//
//  Created by Christian Braun on 16/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "JPEGSegments.hpp"
#include <iostream>
using namespace JPEGSegments;

void APP0::addToStream(Bitstream &stream) {
	stream.add(type, 16);
	stream.add(length, 16);
	stream.add(JFIF[0], 8);
	stream.add(JFIF[1], 8);
	stream.add(JFIF[2], 8);
	stream.add(JFIF[3], 8);
	stream.add(JFIF[4], 8);
	stream.add(MAJOR_REVISION_NUMBER, 8);
	stream.add(MINOR_REVISION_NUMBER, 8);
	stream.add(PIXEL_UNIT, 8);
	stream.add(X_DENSITY, 16);
	stream.add(Y_DENSITY, 16);
	stream.add(PREVIEW_WIDTH, 8);
	stream.add(PREVIEW_HEIGHT, 8);
}

void StartOfFrame0::addToStream(Bitstream &stream) {
	stream.add(type, 16);
	stream.add(length, 16);
	stream.add(precision, 8);
	stream.add(height, 16);
	stream.add(width, 16);
	stream.add(numberOfComponents, 8);
	while (numberOfComponents--) {
		stream.add(0x01, 8);    // ID (Y)
		stream.add(0x22, 8);    // Subsampling
		stream.add(0x00, 8);    // Quantisierungstabelle
	}
}

void StartOfImage::addToStream(Bitstream &stream) {
    stream.add(type, 16);
}

void EndOfImage::addToStream(Bitstream &stream) {
    stream.add(type, 16);
}

void DefineHuffmanTable::addToStream(Bitstream &stream) {
    stream.add(type, 16);
    stream.add(length, 16);
    stream.add(htNumber, 4);
    stream.add(htType, 1);
    stream.add(htRest, 3);
	
    for ( int i = 0; i < 16; ++i ) {
        stream.add(symbolsPerLevel[i], 8);
    }
	
    for (const std::pair<Symbol, Encoding> &enc : encodingTable) {
        stream.add(enc.first, 8);
    }
}

void DefineQuantizationTable::addToStream(Bitstream &stream) {
    stream.add(type, 16);
    stream.add(length, 16);
    stream.add(qtNumber, 4);
    stream.add(qtPrecision, 4);
    
    for (int i = 0; i < 64; ++i) {
        stream.add(values[i], 8);
    }
}

void JPEGWriter::writeJPEGImage(std::shared_ptr<Image> image, const char *pathToFile, EncodingTable encodingTable) {

    StartOfImage* soi = new StartOfImage();
    segments.push_back(soi);
    
    APP0* app0 = new APP0();
    segments.push_back(app0);
    
    StartOfFrame0* sof0 = new StartOfFrame0(1, image);  // 1 = numberOfComponents
    segments.push_back(sof0);
	
	DefineHuffmanTable* dht = new DefineHuffmanTable(0,0,0, encodingTable);
	segments.push_back(dht);
	
    EndOfImage* eoi = new EndOfImage();
    segments.push_back(eoi);
	
    
    for (size_t i = 0; i < segments.size(); ++i) {
        segments[i]->addToStream(stream);
    }
    stream.saveToFile(pathToFile);
}
