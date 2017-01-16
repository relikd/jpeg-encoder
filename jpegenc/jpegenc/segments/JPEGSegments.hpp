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
#include "../huffman/Huffman.hpp"

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
			this->width = (uint16_t)(image->imageSize.width);
			this->height = (uint16_t)(image->imageSize.height);
			this->precision = precision;
			this->length = (uint16_t)(numberOfComponents * 3 + 8);
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
        EncodingTable Y_DC;
        EncodingTable Y_AC;
        EncodingTable CbCr_DC;
        EncodingTable CbCr_AC;
		
        DefineHuffmanTable(EncodingTable Y_DC, EncodingTable Y_AC, EncodingTable CbCr_DC, EncodingTable CbCr_AC) : JpegSegment(0xFFC4) {
            this->Y_DC = Y_DC;
            this->Y_AC = Y_AC;
            this->CbCr_DC = CbCr_DC;
            this->CbCr_AC = CbCr_AC;
            this->length = (uint16_t)(
                            2 // 1 *  2 length
                         +  4 // 4 *  1 HT info
                         + 64 // 4 * 16 number of symbols
                         + Y_DC.size()
                         + Y_AC.size()
                         + CbCr_DC.size()
                         + CbCr_AC.size()
            );
        }		
		virtual void addToStream(Bitstream &stream);
        void addTableData(uint8_t htNumber, uint8_t htType, EncodingTable table, Bitstream &stream);
	};
    
    struct DefineQuantizationTable : JpegSegment {
        uint16_t length;
        uint8_t precision;
        
        DefineQuantizationTable(uint8_t precision) : JpegSegment(0xFFDB) {
            this->precision = precision;
            this->length = 2 + 2 * 64 * (precision + 1);
        }
        
        virtual void addToStream(Bitstream &stream);
    };
	
	struct StartOfScan : JpegSegment {
		uint16_t length;
		unsigned char numberOfComponents;
		//todo for every component 2 byte
		/*For each component, read 2 bytes. It contains,
		 1 byte   Component Id (1=Y, 2=Cb, 3=Cr, 4=I, 5=Q),
		 1 byte   Huffman table to use :
		 bit 0..3 : AC table (0..3)
		 bit 4..7 : DC table (0..3)*/
		unsigned char ignorableBytes[3] = {0x00, 0x3f, 0x00};
		
		StartOfScan(unsigned char numberOfComponents) : JpegSegment(0xFFDA) {
			if(numberOfComponents < 1 || numberOfComponents > 4) {
				printf("number of Componentes must be >= 1 and <=4");
				exit(EXIT_FAILURE);
			}
			this->numberOfComponents = numberOfComponents;
			this->length = 6 + 2 * numberOfComponents;
		}
		
		virtual void addToStream(Bitstream &stream);
		
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
