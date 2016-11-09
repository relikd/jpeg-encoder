#include <iostream>
#include "BitstreamMarv.hpp"

void BitStreamMarv::add(bool bit) {
	bits->push_back(bit);
}

void BitStreamMarv::add(char byte) {
	for(int i = 7; 0 <= i; --i) {
		add((bool) (byte >> i & 0x01));
	}
}

bool BitStreamMarv::read(size_t index) {
	return bits->at(index);
}

void BitStreamMarv::print() {
	std::cout << "Elements: " << bits->size() << std::endl;
	std::cout << "Capacity: " << bits->capacity() << std::endl;

	for (unsigned long i = 0; i < bits->size() && i <= 100; ++i) {
		std::cout << bits->at(i);
	}
	if (bits->size() > 100) {
		std::cout << "... (" << bits->size() - 100 << " more)";
	}
	std::cout << std::endl;
}

unsigned long BitStreamMarv::size() {
	return bits->size();
}

unsigned long BitStreamMarv::capacity() {
	return bits->capacity();
}