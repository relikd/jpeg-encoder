//
//  JPEGImage.cpp
//  jpegenc
//
//  Created by Christian Braun on 16/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "JPEGSegments.hpp"
#include <iostream>
#include "../dct/Arai.hpp"
#include "ImageDataEncoding.hpp"

using namespace JPEGSegments;

void StartOfImage::addToStream(Bitstream &stream) {
    stream.add(type, 16);
}

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
    
    for (int i = 1; i <= numberOfComponents; ++i) {
        stream.add(i, 8);    // ID
        stream.add(0x11, 8); // Subsampling
        stream.add(i != 1, 8);
    }
}

void EndOfImage::addToStream(Bitstream &stream) {
    stream.add(type, 16);
}

unsigned short ffCounter = 0;
void addToStreamNoFF(Bitstream &stream, Encoding enc) {
    unsigned short n = enc.numberOfBits;
    while (n--) {
        if (enc.code & (1 << n)) {
            stream.add(1);
            ++ffCounter;
            if (ffCounter >= 8) {
                stream.add(0x00, 8);
            }
        } else {
            stream.add(0);
            ffCounter = 0;
        }
    }
}

bool comparePairs(const std::pair<Symbol, Encoding> &pair1, const std::pair<Symbol, Encoding> &pair2) {
    return pair1.second.code < pair2.second.code;
}

void DefineHuffmanTable::addToStream(Bitstream &stream) {
    stream.add(type, 16);
    stream.add(length, 16);

    // HT Information
    stream.add(0, 3); // rest
    stream.add(htType, 1);
    stream.add(htNumber, 4);
    
    // number of symbols per level
    unsigned char symbolsPerLevel[16] = {0};
    
    for (const std::pair<Symbol, Encoding> &encoding : table) {
        unsigned short numberOfBits = encoding.second.numberOfBits;
        symbolsPerLevel[numberOfBits - 1] += 1;
    }
    
    for (int i = 0; i < 16; ++i) {
        stream.add(symbolsPerLevel[i], 8);
    }
    
    std::vector<std::pair<Symbol, Encoding>> sortedEncodings;

    for (const std::pair<Symbol, Encoding> &encoding : table) {
        sortedEncodings.push_back(encoding);
    }
    std::sort(sortedEncodings.begin(), sortedEncodings.end(), comparePairs);
    
    for (auto encoding : sortedEncodings) {
        stream.add(encoding.first, 8);
    }
}

uint8_t* DefineQuantizationTable::sortZickZack(const uint8_t* table) {
    uint8_t *sortedTable = new uint8_t[64];
    
    for (int i = 0; i < 64; ++i) {
        sortedTable[i] = table[ZICK_ZACK_INDEXES[i]];
    }
    return sortedTable;
}

void DefineQuantizationTable::addToStream(Bitstream &stream) {
    stream.add(type, 16);
    stream.add(length, 16);
    
    stream.add(0, 4); // precision
    stream.add(qt_number, 4); // number
    uint8_t* qt_sorted = sortZickZack(qt);
    
    for (int i = 0; i < 64; ++i) {
        stream.add(qt_sorted[i], 8);
    }
}

void StartOfScan::addToStream(Bitstream &stream) {
	stream.add(type, 16);
	stream.add(length, 16);
	stream.add(numberOfComponents, 8);
    
    stream.add(1, 8); // Y
    stream.add(0, 4); // Y_DC
    stream.add(0, 4); // Y_AC
    
    stream.add(2, 8); // Cb
    stream.add(1, 4); // CbCr_DC
    stream.add(1, 4); // CbCr_AC
    
    stream.add(3, 8); // Cr
    stream.add(1, 4); // CbCr_DC
    stream.add(1, 4); // CbCr_AC
    
    stream.add(0x00, 8);
    stream.add(0x3f, 8);
    stream.add(0x00, 8);
	
    size_t numberOfBlocks = encodedImageData->Y_DC_encoding.size();

    int k_y = 0;
    int k_cb = 0;
    int k_cr = 0;
    
    for (int i = 0; i < numberOfBlocks; ++i) {
        
        // Y_DC
        auto index = encodedImageData->Y_DC_encoding[i].numberOfBits;
//		std::cout << encodedImageData->Y_DC.at(index) << std::endl;
        addToStreamNoFF(stream, encodedImageData->Y_DC.at(index));
        addToStreamNoFF(stream, encodedImageData->Y_DC_encoding.at(i));
//		std::cout << encodedImageData->Y_DC_encoding.at(i) << std::endl;
        
        // Y_AC
        for (; encodedImageData->Y_AC_byteReps[k_y] != 0; ++k_y)
        {
            index = encodedImageData->Y_AC_byteReps[k_y];
            addToStreamNoFF(stream, encodedImageData->Y_AC.at(index));
            addToStreamNoFF(stream, encodedImageData->Y_AC_encoding.at(k_y));
        }
        stream.add(encodedImageData->Y_AC.at(0).code, encodedImageData->Y_AC.at(0).numberOfBits);
        ++k_y;
		
		
        
        // Cb_DC
        index = encodedImageData->Cb_DC_encoding[i].numberOfBits;
//		std::cout << encodedImageData->CbCr_DC.at(index) << std::endl;
        addToStreamNoFF(stream, encodedImageData->CbCr_DC.at(index));
        addToStreamNoFF(stream, encodedImageData->Cb_DC_encoding.at(i));
//		std::cout << encodedImageData->Cb_DC_encoding.at(i)<< std::endl;

        
        // Cb_AC
        for (; encodedImageData->Cb_AC_byteReps[k_cb] != 0; ++k_cb)
        {
            index = encodedImageData->Cb_AC_byteReps[k_cb];
            addToStreamNoFF(stream, encodedImageData->CbCr_AC.at(index));
            addToStreamNoFF(stream, encodedImageData->Cb_AC_encoding.at(k_cb));
        }
        stream.add(encodedImageData->CbCr_AC.at(0).code, encodedImageData->CbCr_AC.at(0).numberOfBits);
        ++k_cb;

        
        // Cr_DC
        index = encodedImageData->Cr_DC_encoding[i].numberOfBits;
//		std::cout << encodedImageData->CbCr_DC.at(index) << std::endl;
        addToStreamNoFF(stream, encodedImageData->CbCr_DC.at(index));
        addToStreamNoFF(stream, encodedImageData->Cr_DC_encoding.at(i));
//		std::cout << encodedImageData->Cr_DC_encoding.at(i)<< std::endl;
		

        // Cr_AC
        for (; encodedImageData->Cr_AC_byteReps[k_cr] != 0; ++k_cr)
        {
            index = encodedImageData->Cr_AC_byteReps[k_cr];
            addToStreamNoFF(stream, encodedImageData->CbCr_AC.at(index));
            addToStreamNoFF(stream, encodedImageData->Cr_AC_encoding.at(k_cr));
        }
        stream.add(encodedImageData->CbCr_AC.at(0).code, encodedImageData->CbCr_AC.at(0).numberOfBits);
        ++k_cr;
    }

    auto bitsToFill = 8 - (stream.numberOfBits() % 8);
    stream.add(0xFFF, bitsToFill);
}

void JPEGWriter::writeJPEGImage(const char *pathToFile) {    
    StartOfImage* soi = new StartOfImage();
    soi->addToStream(stream);
    
    APP0* app0 = new APP0();
    app0->addToStream(stream);

    DefineQuantizationTable* y_dqt = new DefineQuantizationTable(0, luminanceQT);
    y_dqt->addToStream(stream);
    
    DefineQuantizationTable* cbcr_dqt = new DefineQuantizationTable(1, chrominanceQT);
    cbcr_dqt->addToStream(stream);
    
    StartOfFrame0* sof0 = new StartOfFrame0(3, image);
    sof0->addToStream(stream);
	
    ChannelData* channelData= new ChannelData(image);
    EncodedImageData *encodedImageData = new EncodedImageData(channelData);
    encodedImageData->initialize();
    
    DefineHuffmanTable* Y_DC_dht = new DefineHuffmanTable(0, 0, encodedImageData->Y_DC);
    Y_DC_dht->addToStream(stream);
    
    DefineHuffmanTable* Y_AC_dht = new DefineHuffmanTable(0, 1, encodedImageData->Y_AC);
    Y_AC_dht->addToStream(stream);
    
    DefineHuffmanTable* CbCr_DC_dht = new DefineHuffmanTable(1, 0, encodedImageData->CbCr_DC);
    CbCr_DC_dht->addToStream(stream);

    DefineHuffmanTable* CbCr_AC_dht = new DefineHuffmanTable(1, 1, encodedImageData->CbCr_AC);
    CbCr_AC_dht->addToStream(stream);
    
    StartOfScan* sos = new StartOfScan(3, encodedImageData);
    sos->addToStream(stream);
    
    EndOfImage* eoi = new EndOfImage();
    eoi->addToStream(stream);
	
//	stream.print();
    stream.saveToFile(pathToFile);
	
    delete channelData;
    delete encodedImageData;
}

void EncodedImageData::generateYDataAndHT()
{
    ImageDataEncoding channel1Encoder(channelData->channel1->values, (unsigned int) Y_width, (unsigned int) Y_height);
    channel1Encoder.init();

    Y_DC = channel1Encoder.generateDCEncodingTable(Y_DC_encoding);
    Y_AC = channel1Encoder.generateACEncodingTable(Y_AC_byteReps, Y_AC_encoding);
}

void EncodedImageData::generateCbCrDataAndHT()
{
    ImageDataEncoding channel2Encoder(channelData->channel2->values, (unsigned int) CbCr_width, (unsigned int) CbCr_height);
    ImageDataEncoding channel3Encoder(channelData->channel3->values, (unsigned int) CbCr_width, (unsigned int) CbCr_height);
    channel2Encoder.init();
    channel3Encoder.init();
    
    Cb_DC_encoding = channel2Encoder.differenceEncoding();
    Cr_DC_encoding = channel3Encoder.differenceEncoding();
    channel2Encoder.runLengthEncoding(Cb_AC_byteReps, Cb_AC_encoding);
    channel3Encoder.runLengthEncoding(Cr_AC_byteReps, Cr_AC_encoding);
    
    generateCbCrHT();
}

void EncodedImageData::initialize() {
    channelData->unnormalize(255);
    
    Arai::transform(channelData->channel1->values, Y_width, Y_height);
    Arai::transform(channelData->channel2->values, CbCr_width, CbCr_height);
    Arai::transform(channelData->channel3->values, CbCr_width, CbCr_height);

    Quantization::run(channelData->channel1->values, Y_width, Y_height, luminanceQT);
    Quantization::run(channelData->channel2->values, CbCr_width, CbCr_height, chrominanceQT);
    Quantization::run(channelData->channel3->values, CbCr_width, CbCr_height, chrominanceQT);
    
    generateYDataAndHT();
    generateCbCrDataAndHT();
}

void EncodedImageData::generateCbCrHT() {
    generateCbCrHT_DC();
    generateCbCrHT_AC();
}

void EncodedImageData::generateCbCrHT_DC() {
    Huffman DC_huffman;

    for (auto &current : Cb_DC_encoding) {
        DC_huffman.addSymbol(current.numberOfBits);
    }
    
    for (auto &current : Cr_DC_encoding) {
        DC_huffman.addSymbol(current.numberOfBits);
    }
    
    DC_huffman.generateNodeList();
	DC_huffman.preventAllOnesPath();
    
    CbCr_DC = DC_huffman.canonicalEncoding(16);
}

void EncodedImageData::generateCbCrHT_AC() {
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
	AC_huffman.preventAllOnesPath();
    CbCr_AC = AC_huffman.canonicalEncoding(16);
}
