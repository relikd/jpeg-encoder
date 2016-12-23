#include "BitMath.hpp"

#include <stdlib.h>

static const char LogTable256[256] = {
#define B8(n)  n, n, n, n, n, n, n, n
#define B16(n) B8(n), B8(n)
#define B32(n) B16(n), B16(n)
#define B64(n) B32(n), B32(n)
	1, 1, 2, 2, 3, 3, 3, 3, B8(4), B16(5), B32(6), B64(7), B64(8), B64(8)
};

unsigned short BitMath::log2(unsigned long v) {
	if (v>>32) {
		if (v>>48) { if (v>>56) return LogTable256[v>>56]+56;
					 else       return LogTable256[v>>48]+48;
		} else {	 if (v>>40) return LogTable256[v>>40]+40;
					 else       return LogTable256[v>>32]+32;}
	} else {
		if (v>>16) { if (v>>24) return LogTable256[v>>24]+24;
					 else       return LogTable256[v>>16]+16;
		} else {	 if (v>>8)  return LogTable256[v>>8]+8;
					 else       return LogTable256[v];}
	}
}
