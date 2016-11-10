#include <iostream>
#include <fstream>
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

bool *BitStreamMarv::read(size_t firstIndex, size_t lastIndex) {
	size_t size = lastIndex - firstIndex + 1;
	bool *result = new bool[size];
	for(size_t i = 0; i < size; ++i) {
		result[i] = bits->at(i);
	}
	return result;
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

void BitStreamMarv::saveToFile(const char *pathToFile) {
	std::ofstream outputStream;
	outputStream.open(pathToFile);
	for (size_t i = 0; i < size(); ++i)
	{
		outputStream << bits->at(i);
	}
	outputStream << "\n";
	outputStream.close();
}

unsigned long BitStreamMarv::size() {
	return bits->size();
}

unsigned long BitStreamMarv::capacity() {
	return bits->capacity();
}