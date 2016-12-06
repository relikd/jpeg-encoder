#ifndef BitMath_hpp
#define BitMath_hpp

class BitMath {
public:
	/**
	 * not really log2(). Real log2() should be (return-1)
	 * @return number of bits needed to represent the value (1-64)
	 */
	static unsigned short log2(unsigned long value);
	static unsigned short log2Shift(unsigned long value);
	static unsigned short log2Slow(unsigned long value);
	//	unsigned short out = 0;
	//	Test::performance(100000000, 3, [&out](size_t v){
	//		while (v--) out = BitMath::log2Shift(v);
	//	});
	//	Test::performance(100000000, 3, [&out](size_t v){
	//		while (v--) out = BitMath::log2(v);
	//	});
	//	Test::performance(100000000, 3, [&out](size_t v){
	//		while (v--) out = BitMath::log2Slow(v);
	//	});
};

#endif /* BitMath_hpp */
