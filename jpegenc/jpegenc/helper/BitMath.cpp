#include "BitMath.hpp"

static const char LogTable256[256] = {
#define B8(n)  n, n, n, n, n, n, n, n
#define B16(n) B8(n), B8(n)
#define B32(n) B16(n), B16(n)
#define B64(n) B32(n), B32(n)
	1, 1, 2, 2, 2, 2, B8(3), B16(4), B32(5), B64(6), B64(7), B64(7)
};

inline unsigned short bitsNeededChar(unsigned char v) {
//	return LogTable256[v]; // alternatively. But will stay in RAM
	if (v & 0xF0) { if (v & 0xC0) return (v & 0x80 ? 8 : 7);
					else          return (v & 0x20 ? 6 : 5);
	} else {		if (v & 0x0C) return (v & 0x08 ? 4 : 3);
					else          return (v & 0x02 ? 2 : 1);}
}

inline unsigned short bitsNeededLong(unsigned long v) {
	if (v & 0xFFFFFFFF00000000UL) { // divide long in two parts and see if upper part has any bit set
		if (v & 0xFFFF000000000000UL)
			return (v & 0xFF00000000000000UL ? bitsNeededChar(v>>56)+56 : bitsNeededChar(v>>48)+48);
		else
			return (v & 0x0000FF0000000000UL ? bitsNeededChar(v>>40)+40 : bitsNeededChar(v>>32)+32);
	} else {
		if (v & 0xFFFF0000UL)
			return (v & 0xFF000000UL ? bitsNeededChar(v>>24)+24 : bitsNeededChar(v>>16)+16);
		else
			return (v & 0x0000FF00UL ? bitsNeededChar(v>>8)+8 : bitsNeededChar(v));
	}
}

unsigned short BitMath::log2(unsigned long value) {
	return bitsNeededLong(value);
}


inline unsigned short bitsNeededCharS(unsigned char v) {
	if (v>>4) { if (v>>6) return (v>>7 ? 8 : 7);
				else      return (v>>5 ? 6 : 5);
	} else {	if (v>>2) return (v>>3 ? 4 : 3);
				else      return (v>>1 ? 2 : 1);}
}

inline unsigned short bitsNeededLongShift(unsigned long v) {
	if (v>>32) { // divide long in two parts and see if upper part has any bit set
		if (v>>48) return (v >> 56 ? bitsNeededCharS(v>>56)+56 : bitsNeededCharS(v>>48)+48);
		else       return (v >> 40 ? bitsNeededCharS(v>>40)+40 : bitsNeededCharS(v>>32)+32);
	} else {
		if (v>>16) return (v >> 24 ? bitsNeededCharS(v>>24)+24 : bitsNeededCharS(v>>16)+16);
		else       return (v >>  8 ? bitsNeededCharS(v>>8)+8   : bitsNeededCharS(v));
	}
}

unsigned short BitMath::log2Shift(unsigned long value) {
	return bitsNeededLongShift(value);
}


inline unsigned short bitsNeededNotOptimalButSimple(unsigned long v) {
	unsigned short result = 0;
	unsigned short shifter = 32; // half long
	while (shifter) {
		if (v >> shifter)
			result += shifter;
		shifter >>= 1;
	}
	return result; // 0-63.  0, 0, 1, 1, 2, ...
}

inline unsigned short bitsNeededLongAlternative(unsigned long v) {
	unsigned short tmp = 56
	- (v & 0xFFFFFFFF00000000UL ? 0 : 32)
	- (v & 0xFFFF0000FFFF0000UL ? 0 : 16)
	- (v & 0xFF00FF00FF00FF00UL ? 0 : 8);
//	- (v & 0xF0F0F0F0F0F0F0F0UL ? 0 : 4)
//	- (v & 0xCCCCCCCCCCCCCCCCUL ? 0 : 2)
//	- (v & 0xAAAAAAAAAAAAAAAAUL ? 0 : 1);
//
//	unsigned short tmp = 0b111000
//	& (v & 0xFFFFFFFF00000000UL ? 0b011000 : 0b111000)
//	& (v & 0xFFFF0000FFFF0000UL ? 0b101000 : 0b111000)
//	& (v & 0xFF00FF00FF00FF00UL ? 0b110000 : 0b111000);
	
	return bitsNeededChar(v>>tmp) + tmp;
	//	if (v & 0xF0F0F0F0F0F0F0F0UL)
}

unsigned short BitMath::log2Slow(unsigned long value) {
	return bitsNeededLongAlternative(value);
}



