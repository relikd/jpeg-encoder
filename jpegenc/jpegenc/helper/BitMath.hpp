#ifndef BitMath_hpp
#define BitMath_hpp

class BitMath {
public:
	/**
	 * not really log2(). Real log2() should be (return-1)
	 * @return number of bits needed to represent the value (1-64)
	 */
	static unsigned short log2(unsigned long value);
};

#endif /* BitMath_hpp */
