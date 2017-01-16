//
//  JPEGImage.cpp
//  jpegenc
//
//  Created by Christian Braun on 16/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "JPEGSegments.hpp"
#include <iostream>
#include "../Quantization.hpp"
#include "../dct/Arai.hpp"
#include "ImageDataEncoding.hpp"

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
    
    addTableData(0, 0, Y_DC, stream);
    addTableData(1, 1, Y_AC, stream);
    addTableData(2, 0, CbCr_DC, stream);
    addTableData(3, 1, CbCr_AC, stream);
}

void DefineHuffmanTable::addTableData(uint8_t htNumber, uint8_t htType, EncodingTable table, Bitstream &stream) {
    // HT Information
    stream.add(htNumber, 4);
    stream.add(htType, 1);
    stream.add(0, 3); // rest

    // number of symbols per level
    unsigned char symbolsPerLevel[16] = {0};

    for (const std::pair<Symbol, Encoding> &encoding : table) {
        unsigned short numberOfBits = encoding.second.numberOfBits;
        symbolsPerLevel[numberOfBits - 1] += 1;
    }

    for (int i = 0; i < 16; ++i) {
        stream.add(symbolsPerLevel[i], 8);
    }

    for (const std::pair<Symbol, Encoding> &encoding : table) {
        stream.add(encoding.first, 8);
    }
}

void DefineQuantizationTable::addToStream(Bitstream &stream) {
    stream.add(type, 16);
    stream.add(length, 16);
    
    stream.add(0, 4); // 0 = Y
    stream.add(precision, 4);
    
    for (int i = 0; i < 64; ++i) {
        stream.add(Quantization::getLuminanceQT()[i], 8);
    }

    stream.add(1, 4); // 1 = CbCr
    stream.add(precision, 4);
    
    for (int i = 0; i < 64; ++i) {
        stream.add(Quantization::getChrominanceQT()[i], 8);
    }
}

void StartOfScan::addToStream(Bitstream &stream) {
	stream.add(type, 16);
	stream.add(length, 16);
	stream.add(numberOfComponents, 8);
	//todo add components
	//stream.add(ignorableBytes, 24);
	
}

void JPEGWriter::writeJPEGImage(std::shared_ptr<Image> image, const char *pathToFile, EncodingTable encodingTable) {

    StartOfImage* soi = new StartOfImage();
    segments.push_back(soi);
    
    APP0* app0 = new APP0();
    segments.push_back(app0);
    
    DefineQuantizationTable* dqt = new DefineQuantizationTable(0); // 0 = 8bit precision
    segments.push_back(dqt);

    StartOfFrame0* sof0 = new StartOfFrame0(1, image);  // 1 = numberOfComponents
    segments.push_back(sof0);

    
    // TODO
    
    
    DefineHuffmanTable* dht = new DefineHuffmanTable(encodingTable, encodingTable, encodingTable, encodingTable);
    segments.push_back(dht);

    // sos
    
    EndOfImage* eoi = new EndOfImage();
    segments.push_back(eoi);
    
    for (size_t i = 0; i < segments.size(); ++i) {
        segments[i]->addToStream(stream);
    }
    stream.saveToFile(pathToFile);
}
