#ifndef JPEGSegmentsMarv_hpp
#define JPEGSegmentsMarv_hpp

#include <stdio.h>
#include "Bitstream.hpp"

#endif /* JPEGSegmentsMarv_hpp */

struct Segment {
    uint16_t marker;
    
    Segment(uint16_t marker) : marker(marker) {
    }
    virtual void write(Bitstream *bitStream);
};

struct TestSegment : Segment {

    TestSegment() : Segment(0xFFFF) {
    }
    
};

struct SegmentWriter {
    Bitstream *bitStream;
    
    SegmentWriter() {
    }
    
    ~SegmentWriter() {
    }
    
    void addSegment(Segment *segment);
    void writeToFile(const char *pathToFile);
};
