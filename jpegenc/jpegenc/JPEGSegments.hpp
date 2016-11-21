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
#include "model/Image.hpp"

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
		
	private:
		void addChannel1ToStream(Bitstream &stream);
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
	
    struct JPEGWriter {
        std::vector<JpegSegment*> segments;
        Bitstream stream;
        
        JPEGWriter() {
        }
        
        void writeJPEGImage(std::shared_ptr<Image> image, const char *pathToFile);
    };
}



#endif
