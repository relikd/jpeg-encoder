#ifndef BitMath_hpp
#define BitMath_hpp

class BitMath {
public:
	/**
	 * not really log2(). Real log2() should be (return-1)
	 * @return number of bits needed to represent the value (1-64)
	 */
	static unsigned short log2(unsigned long value);
	
	/**
	 * Get the number of character places for any given int number
	 * @return Returns 1 for (0-9), 2 for (10-99), etc.
	 */
	static inline unsigned short numberOfDigitsBase10 ( unsigned long number ) {
		unsigned short count = 1;
		while (number > 9) {
			number /= 10;
			++count;
		}
		return count;
	}
};

#endif /* BitMath_hpp */
