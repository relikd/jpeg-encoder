#include <bitset>
#include "Encoding.hpp"

std::ostream& operator<<(std::ostream& os, const Encoding& obj) {
	switch (obj.numberOfBits) {
		case 0:
		case 1: os << std::bitset<1>(obj.code); break;
		case 2: os << std::bitset<2>(obj.code); break;
		case 3: os << std::bitset<3>(obj.code); break;
		case 4: os << std::bitset<4>(obj.code); break;
		case 5: os << std::bitset<5>(obj.code); break;
		case 6: os << std::bitset<6>(obj.code); break;
		case 7: os << std::bitset<7>(obj.code); break;
		case 8: os << std::bitset<8>(obj.code); break;
		case 9: os << std::bitset<9>(obj.code); break;
		case 10: os << std::bitset<10>(obj.code); break;
		case 11: os << std::bitset<11>(obj.code); break;
		case 12: os << std::bitset<12>(obj.code); break;
		case 13: os << std::bitset<13>(obj.code); break;
		case 14: os << std::bitset<14>(obj.code); break;
		case 15: os << std::bitset<15>(obj.code); break;
		case 16: os << std::bitset<16>(obj.code); break;
		case 17: os << std::bitset<17>(obj.code); break;
		case 18: os << std::bitset<18>(obj.code); break;
		case 19: os << std::bitset<19>(obj.code); break;
		case 20: os << std::bitset<20>(obj.code); break;
		case 21: os << std::bitset<21>(obj.code); break;
		case 22: os << std::bitset<22>(obj.code); break;
		case 23: os << std::bitset<23>(obj.code); break;
		case 24: os << std::bitset<24>(obj.code); break;
		case 25: os << std::bitset<25>(obj.code); break;
		case 26: os << std::bitset<26>(obj.code); break;
		case 27: os << std::bitset<27>(obj.code); break;
		case 28: os << std::bitset<28>(obj.code); break;
		case 29: os << std::bitset<29>(obj.code); break;
		case 30: os << std::bitset<30>(obj.code); break;
		case 31: os << std::bitset<31>(obj.code); break;
		case 32: os << std::bitset<32>(obj.code); break;
		case 33: os << std::bitset<33>(obj.code); break;
		case 34: os << std::bitset<34>(obj.code); break;
		case 35: os << std::bitset<35>(obj.code); break;
		case 36: os << std::bitset<36>(obj.code); break;
		case 37: os << std::bitset<37>(obj.code); break;
		case 38: os << std::bitset<38>(obj.code); break;
		case 39: os << std::bitset<39>(obj.code); break;
		case 40: os << std::bitset<40>(obj.code); break;
		case 41: os << std::bitset<41>(obj.code); break;
		case 42: os << std::bitset<42>(obj.code); break;
		case 43: os << std::bitset<43>(obj.code); break;
		case 44: os << std::bitset<44>(obj.code); break;
		case 45: os << std::bitset<45>(obj.code); break;
		case 46: os << std::bitset<46>(obj.code); break;
		case 47: os << std::bitset<47>(obj.code); break;
		case 48: os << std::bitset<48>(obj.code); break;
		case 49: os << std::bitset<49>(obj.code); break;
		case 50: os << std::bitset<50>(obj.code); break;
		case 51: os << std::bitset<51>(obj.code); break;
		case 52: os << std::bitset<52>(obj.code); break;
		case 53: os << std::bitset<53>(obj.code); break;
		case 54: os << std::bitset<54>(obj.code); break;
		case 55: os << std::bitset<55>(obj.code); break;
		case 56: os << std::bitset<56>(obj.code); break;
		case 57: os << std::bitset<57>(obj.code); break;
		case 58: os << std::bitset<58>(obj.code); break;
		case 59: os << std::bitset<59>(obj.code); break;
		case 60: os << std::bitset<60>(obj.code); break;
		case 61: os << std::bitset<61>(obj.code); break;
		case 62: os << std::bitset<62>(obj.code); break;
		case 63: os << std::bitset<63>(obj.code); break;
		default: os << std::bitset<64>(obj.code); break;
	}
	return os;
}
