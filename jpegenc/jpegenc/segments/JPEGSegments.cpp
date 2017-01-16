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
    
    stream.add(qt_number, 4); // 0 = Y
    stream.add(precision, 4);
    
    for (int i = 0; i < 64; ++i) {
        stream.add(table[i], 8);
    }
}

void StartOfScan::addToStream(Bitstream &stream) {
	stream.add(type, 16);
	stream.add(length, 16);
	stream.add(numberOfComponents, 8);
	//todo add components
	//stream.add(ignorableBytes, 24);
	
}

void JPEGWriter::writeJPEGImage(std::shared_ptr<Image> image, const char *pathToFile) {
    const uint8_t *luminanceQT = Quantization::getLuminanceQT();
    const uint8_t *chrominanceQT = Quantization::getChrominanceQT();
    
    StartOfImage* soi = new StartOfImage();
    segments.push_back(soi);
    
    APP0* app0 = new APP0();
    segments.push_back(app0);

    DefineQuantizationTable* Y_dqt = new DefineQuantizationTable(0, 0, luminanceQT);
    segments.push_back(Y_dqt);
    
    DefineQuantizationTable* CbCr_dqt = new DefineQuantizationTable(1, 0, chrominanceQT);
    segments.push_back(CbCr_dqt);
    
    StartOfFrame0* sof0 = new StartOfFrame0(1, image);  // 1 = numberOfComponents
    segments.push_back(sof0);

    float *copyOfChannel1 = new float[image->channel1->numberOfPixel()];
    float *copyOfChannel2 = new float[image->channel2->numberOfPixel()];
    float *copyOfChannel3 = new float[image->channel3->numberOfPixel()];

    memcpy(copyOfChannel1, image->channel1, image->channel1->numberOfPixel() * sizeof(float));
    memcpy(copyOfChannel2, image->channel2, image->channel2->numberOfPixel() * sizeof(float));
    memcpy(copyOfChannel3, image->channel3, image->channel3->numberOfPixel() * sizeof(float));

    Arai::transform(copyOfChannel1, image->channel1->imageSize.width, image->channel1->imageSize.height);
    Arai::transform(copyOfChannel2, image->channel2->imageSize.width, image->channel2->imageSize.height);
    Arai::transform(copyOfChannel3, image->channel3->imageSize.width, image->channel3->imageSize.height);

    Quantization::run(copyOfChannel1, image->channel1->imageSize.width, image->channel1->imageSize.height, luminanceQT);
    Quantization::run(copyOfChannel2, image->channel2->imageSize.width, image->channel2->imageSize.height, chrominanceQT);
    Quantization::run(copyOfChannel3, image->channel3->imageSize.width, image->channel3->imageSize.height, chrominanceQT);
    
    ImageDataEncoding channel1Encoder(copyOfChannel1, image->channel1->imageSize.width, image->channel1->imageSize.height);
    ImageDataEncoding channel2Encoder(copyOfChannel2, image->channel2->imageSize.width, image->channel2->imageSize.height);
    ImageDataEncoding channel3Encoder(copyOfChannel3, image->channel3->imageSize.width, image->channel3->imageSize.height);
    
    channel1Encoder.init();
    channel2Encoder.init();
    channel3Encoder.init();

    std::vector<Encoding> Y_DC_encoding;
    auto Y_DC = channel1Encoder.generateDCEncodingTable(Y_DC_encoding);
    
    std::vector<uint8_t> Y_AC_byteReps;
    std::vector<Encoding> Y_AC_encoding;
    auto Y_AC = channel1Encoder.generateACEncodingTable(Y_AC_byteReps, Y_AC_encoding);
    
    std::vector<Encoding> Cb_DC_encoding = channel2Encoder.differenceEncoding();
    std::vector<Encoding> Cr_DC_encoding = channel3Encoder.differenceEncoding();
    Huffman DC_huffman;
    for (auto &current : Cb_DC_encoding) {
        DC_huffman.addSymbol(current.numberOfBits);
    }
    for (auto &current : Cr_DC_encoding) {
        DC_huffman.addSymbol(current.numberOfBits);
    }
    DC_huffman.generateNodeList();
    auto CbCr_DC = DC_huffman.canonicalEncoding(16);
    
    std::vector<Encoding> Cb_AC_encoding;
    std::vector<Encoding> Cr_AC_encoding;
    std::vector<uint8_t> Cb_AC_byteReps;
    std::vector<uint8_t> Cr_AC_byteReps;
    channel2Encoder.runLengthEncoding(Cb_AC_byteReps, Cb_AC_encoding);
    channel3Encoder.runLengthEncoding(Cr_AC_byteReps, Cr_AC_encoding);
    Huffman AC_huffman;
    for (auto &current : Cb_AC_byteReps)
    {
        AC_huffman.addSymbol(current);
    }
    for (auto &current : Cr_AC_byteReps)
    {
        AC_huffman.addSymbol(current);
    }
    AC_huffman.generateNodeList();
    auto CbCr_AC = AC_huffman.canonicalEncoding(16);
    
    DefineHuffmanTable* dht = new DefineHuffmanTable(Y_DC, Y_AC, CbCr_DC, CbCr_AC);
    segments.push_back(dht);

    // sos
    
    EndOfImage* eoi = new EndOfImage();
    segments.push_back(eoi);
    
    for (size_t i = 0; i < segments.size(); ++i) {
        segments[i]->addToStream(stream);
    }
    stream.saveToFile(pathToFile);
}
