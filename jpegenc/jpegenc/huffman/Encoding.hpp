#ifndef Encoding_hpp
#define Encoding_hpp

#include <iostream>

struct Encoding {
	unsigned long code = 0;
	unsigned short numberOfBits = 0;
	
	Encoding() {};
	Encoding(unsigned long code, unsigned short numberOfBits) : code(code), numberOfBits(numberOfBits) {};
	
	friend std::ostream& operator<<(std::ostream& os, const Encoding& obj);
};

#endif /* Encoding_hpp */
