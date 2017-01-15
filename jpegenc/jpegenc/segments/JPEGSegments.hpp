//
//  JPEGImage.hpp
//  jpegenc
//
//  Created by Christian Braun on 16/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef JPEGSegemens_hpp
#define JPEGSegemens_hpp

#include <stdio.h>
#include <stdint.h>
#include <memory>
#include "../bitstream/Bitstream.hpp"
#include "../model/Image.hpp"
#include "../huffmann/Huffman.hpp"

namespace JPEGSegments {
	
	struct JpegSegment {
		const uint16_t type;
		
		JpegSegment(uint16_t type) : type(type) {}
		
		virtual void addToStream(Bitstream &stream) = 0;
	};
	
	struct APP0 : JpegSegment {
		const unsigned char JFIF[5] = {0x4a, 0x46, 0x49, 0x46, 0x00};
		const unsigned char MAJOR_REVISION_NUMBER = 1;
		const unsigned char MINOR_REVISION_NUMBER = 1;
		const unsigned char PIXEL_UNIT = 0;
		const uint16_t X_DENSITY = 0x0048;
		const uint16_t Y_DENSITY = 0x0048;
		const unsigned char PREVIEW_WIDTH = 0;
		const unsigned char PREVIEW_HEIGHT = 0;
		
		uint16_t length;
		
		APP0() : JpegSegment(0xFFE0){
			length = 16;
		}
		
		virtual void addToStream(Bitstream &stream);
	};
	
	struct StartOfFrame0 : JpegSegment {
		uint16_t length;
		unsigned char precision;
		unsigned char numberOfComponents;
		uint16_t height;
		uint16_t width;
		std::shared_ptr<Image> image;
		
		StartOfFrame0(unsigned char numberOfComponents,
					  std::shared_ptr<Image> image,
					  unsigned char precision = 8)
		: JpegSegment(0xFFC0) {
			this->numberOfComponents = numberOfComponents;
			this->image = image;
			this->width = image->imageSize.width;
			this->height = image->imageSize.height;
			this->precision = precision;
			this->length = numberOfComponents * 3 + 8;
		}
		
		virtual void addToStream(Bitstream &stream);
	};
	
	struct StartOfImage : JpegSegment {
		
		StartOfImage() : JpegSegment(0xFFD8) {
		}
		virtual void addToStream(Bitstream &stream);
	};
	
	struct EndOfImage : JpegSegment {
		
		EndOfImage() : JpegSegment(0xFFD9) {
		}
		virtual void addToStream(Bitstream &stream);
	};
	
	struct DefineHuffmanTable : JpegSegment {
		uint16_t length;
		EncodingTable encodingTable;
		unsigned char htInfoNumber;
		unsigned char htInfoType;
		unsigned char htInfoRest;
		unsigned char symbolsPerLevel[16] = {0};
		
		DefineHuffmanTable(EncodingTable encodingTable) : JpegSegment(0xFFC4) {
			this->encodingTable = encodingTable;
			this->htInfoNumber = 0; // hardcoded
			this->htInfoType = 0; // hardcoded
			this->htInfoRest = 0; // hardcoded
			
			for (const std::pair<Symbol, Encoding> &enc : encodingTable) {
				unsigned short numberOfBits = enc.second.numberOfBits;
				symbolsPerLevel[numberOfBits - 1] += 1;
			}
			this->length = 2 + 17 + encodingTable.size();
		}
		
		virtual void addToStream(Bitstream &stream);
	};
    
    struct DefineQuantizationTable : JpegSegment {
        uint16_t length;
        unsigned char qtNumber;
        unsigned char qtPrecision;
        unsigned char* values; // TODO: Replace by real table

        DefineQuantizationTable(unsigned char qtNumber, unsigned char qtPrecision) : JpegSegment(0xFFDB) {
            this->qtNumber = qtNumber;
            this->qtPrecision = qtPrecision;
            
        }
    };
	
	struct JPEGWriter {
		std::vector<JpegSegment*> segments;
		Bitstream stream;
		
		JPEGWriter() {
		}
		
		void writeJPEGImage(std::shared_ptr<Image> image, const char *pathToFile, EncodingTable encodingTable);
	};
}



#endif
