#include <glob.h>
#include <vector>

#ifndef JPEG_ENCODER_BITSTREAMMARV_H
#define JPEG_ENCODER_BITSTREAMMARV_H

#endif //JPEG_ENCODER_BITSTREAMMARV_H

struct BitStreamMarv
{
private:
	size_t initialCapacity;
	std::vector<bool> *bits;

public:
	BitStreamMarv() {
		bits = new std::vector<bool>();
	}

	BitStreamMarv(unsigned long initialCapacity) {
		bits = new std::vector<bool>();
		bits->reserve(initialCapacity);
	}

	~BitStreamMarv() {
		delete bits;
	}

	void add(bool bit);
	void add(char byte);
	bool read(size_t index);
	bool *read(size_t firstIndex, size_t stop);
	void print();

	unsigned long size();
	unsigned long capacity();
};
// add( bit )
// add ( byte, numberOfBits )
// print
// save( fileName )
// read( index )
// read( start,end )