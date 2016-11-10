#ifndef Channel_hpp
#define Channel_hpp

typedef float color;

#include <functional>

struct Dimension {
	size_t width, height;
	size_t pixelCount;
	
	Dimension(size_t w, size_t h) : width(w), height(h), pixelCount(w * h) {}
	
	Dimension& operator*=(size_t a) { width*=a; height*=a; pixelCount*=a*a; return *this; }
	Dimension& operator/=(size_t a) { width/=a; height/=a; pixelCount/=a*a; return *this; }
	Dimension& operator/=(const Dimension &other) {
		width/=other.width; height/=other.height; pixelCount/=other.pixelCount; return *this;
	}
	bool operator==(const Dimension &other) { return (pixelCount == other.pixelCount); }
	bool operator!=(const Dimension &other) { return (pixelCount != other.pixelCount); }
};


class Channel {
	Dimension imageSize;
	color *values;
	
public:
	Channel(Dimension dim) : imageSize(dim) {
		values = new color[imageSize.pixelCount];
	}
	
	~Channel() {
		delete[] values;
	}
	
	size_t numberOfPixel() { return imageSize.pixelCount; }
	
	color getValue(size_t x, size_t y, Dimension mapped_size);
	color getValue(size_t index, Dimension mapped_size);
	void setValue(size_t x, size_t y, color &value);
	void setValue(size_t index, color &value);
	void reduceBySubSampling(size_t stepWidth, size_t stepHeight=1);
	void reduceByAveraging(size_t stepWidth, size_t stepHeight=1);
	
private:
	void reduceWithFunction(size_t stepWidth, size_t stepHeight, std::function<color(size_t, size_t)>);
};

#endif /* Channel_hpp */
