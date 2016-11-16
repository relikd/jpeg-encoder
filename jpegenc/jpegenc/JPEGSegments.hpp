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
#include "bitstream/Bitstream.hpp"

namespace JPEGSegments {
	enum class SegmentType : uint16_t {
		StartOfImage = 0xFFD8, // SOI
		EndOfImage = 0xFFD9, // EOI
		APP0 = 0xFFE0,
		StartOfFrame0 = 0xFFC0, // SOF0
		DefineHuffmannTable = 0xFFC4, // DHT
		DefineQuantizationTable = 0xFFDB, // DQT
		StartOfScan = 0xFFDA, // SOS
	};
	
	
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
		
		StartOfFrame0(unsigned char numberOfComponents,
					  uint16_t width,
					  uint16_t height,
					  unsigned char precision = 8)
		: JpegSegment(0xFFC0) {
			this->numberOfComponents = numberOfComponents;
			this->width = width;
			this->height = height;
			this->precision = precision;
			this->length = numberOfComponents * 3 + 8;
		}
		
		virtual void addToStream(Bitstream &stream);
	};
	
	
}



#endif
