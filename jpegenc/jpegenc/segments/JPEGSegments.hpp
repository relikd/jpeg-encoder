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
#include "../Quantization.hpp"
#include "../model/Channel.hpp"

namespace JPEGSegments {
	
	
	struct JpegSegment {
		const uint16_t type;
		
		JpegSegment(uint16_t type) : type(type) {}
		
		virtual void addToStream(Bitstream &stream) = 0;
	};
    
    struct StartOfImage : JpegSegment {
        
        StartOfImage() : JpegSegment(0xFFD8) {
        }
        virtual void addToStream(Bitstream &stream);
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
	struct EndOfImage : JpegSegment {
		
		EndOfImage() : JpegSegment(0xFFD9) {
		}
		virtual void addToStream(Bitstream &stream);
	};
    
	struct DefineHuffmanTable : JpegSegment {
        uint16_t length;
        uint8_t htNumber;
        uint8_t htType;
        EncodingTable table;
        
        DefineHuffmanTable(uint8_t htNumber, uint8_t htType, EncodingTable table) : JpegSegment(0xFFC4)
        {
            this->htNumber = htNumber;
            this->htType = htType;
            this->table = table;
            this->length = (uint16_t)(2+1+16+table.size());
        }
        
		virtual void addToStream(Bitstream &stream);
    };
    
    struct DefineQuantizationTable : JpegSegment {
        uint16_t length;
        const uint8_t* qt;
        uint8_t qt_number;
        
        DefineQuantizationTable(uint8_t qt_number, const uint8_t* qt) : JpegSegment(0xFFDB) {
            this->qt_number = qt_number;
            this->qt = qt;
            this->length = 67;
        }
        
        uint8_t* sortZickZack(const uint8_t* table);
        virtual void addToStream(Bitstream &stream);
    };
	
    struct EncodedImageData {
        std::vector<Encoding> Y_DC_encoding;
        std::vector<uint8_t> Y_AC_byteReps;
        std::vector<Encoding> Y_AC_encoding;
        EncodingTable Y_DC;
        EncodingTable Y_AC;
        
        std::vector<Encoding> Cb_DC_encoding;
        std::vector<Encoding> Cr_DC_encoding;
        std::vector<Encoding> Cb_AC_encoding;
        std::vector<Encoding> Cr_AC_encoding;
        std::vector<uint8_t> Cb_AC_byteReps;
        std::vector<uint8_t> Cr_AC_byteReps;
        
        EncodingTable CbCr_DC;
        EncodingTable CbCr_AC;
        
        const uint8_t *luminanceQT = Quantization::getLuminanceQT();
        const uint8_t *chrominanceQT = Quantization::getChrominanceQT();
        
        ChannelData* channelData;
        
        size_t Y_numberOfPixels;
        size_t Y_width;
        size_t Y_height;
        
        size_t CbCr_numberOfPixels;
        size_t CbCr_width;
        size_t CbCr_height;
        
        EncodedImageData(ChannelData* channelData) : channelData(channelData),
        Y_numberOfPixels(channelData->channel1->numberOfPixel()),
        Y_width(channelData->channel1->imageSize.width),
        Y_height(channelData->channel1->imageSize.height),
        CbCr_numberOfPixels(channelData->channel2->numberOfPixel()),
        CbCr_width(channelData->channel2->imageSize.width),
        CbCr_height(channelData->channel2->imageSize.height)
        {}
        
        void initialize();
        
    private:
        void generateYDataAndHT();
        void generateCbCrDataAndHT();
        void generateCbCrHT();
        void generateCbCrHT_DC();
        void generateCbCrHT_AC();
    };

    struct StartOfScan : JpegSegment {
		uint16_t length;
		uint8_t numberOfComponents;
        EncodedImageData *encodedImageData;
		
		unsigned short ffCounter = 0;
		unsigned int buffer = 0;
		unsigned int bufferIndex = 0;
		
		StartOfScan(uint8_t numberOfComponents, EncodedImageData *encodedImageData) : JpegSegment(0xFFDA) {
			this->numberOfComponents = numberOfComponents;
			this->length = 6 + 2 * numberOfComponents;
            this->encodedImageData = encodedImageData;
		}
		virtual void addToStream(Bitstream &stream);
		void addToStreamNoFF(Bitstream &stream, Encoding enc);
	};
    
	struct JPEGWriter {
		std::vector<JpegSegment*> segments;
		Bitstream stream;
        std::shared_ptr<Image> image;
        const uint8_t *luminanceQT = Quantization::getLuminanceQT();
        const uint8_t *chrominanceQT = Quantization::getChrominanceQT();
        
        JPEGWriter(std::shared_ptr<Image> image) {
            this->image = image;
		}
		
		void writeJPEGImage(const char *pathToFile);
	};
    
}

#endif
