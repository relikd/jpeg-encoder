#include <memory>
#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	FILE *file = fopen(pathToImage, "r");
	
	char magicNumber[8];
	scanForPattern(file, "%s\n", magicNumber);
	int width=0, height=0;
	scanForPattern(file, "%d %d\n", &width, &height);

	int maxValue=0;
	scanForPattern(file, "%d\n", &maxValue);

	auto image = std::make_shared<Image>(Dimension(width, height));

	size_t index = 0;

	while (1) {
		color r=0, g=0, b=0;
		int elementsRead = scanForPattern(file, "%d %d %d", &r, &g, &b);

		if (elementsRead  == EOF) {
			break;
		}
		image->channel1->setValue( index, normalize(r, maxValue, 255));
		image->channel2->setValue( index, normalize(g, maxValue, 255));
		image->channel3->setValue( index, normalize(b, maxValue, 255));
		++index;
	}
	image->colorSpace = ColorSpaceRGB;
	return image;
}

std::shared_ptr<Image> PPMLoader::customLoad(const char *pathToImage) {
	
	FILE *file = fopen(pathToImage, "rb");
	if (file==NULL) {fputs ("File error",stderr); exit (1);}
	
	fseek (file , 0 , SEEK_END);
	long lSize = ftell (file);
	rewind (file);
	
	// allocate memory to contain the whole file:
	char * buffer = new char[lSize];
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
	
	// copy the file into the buffer:
	size_t result = fread (buffer,1,lSize,file);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	
	/* the whole file is now loaded in the memory buffer. */
	
	bool isComment = false;
	bool skipNextWhitespace = true;
	int step = 0;
	int width=0, height=0, maxValue=0;
	
	size_t lastPos = 0;
	
	for (size_t i=0; i < lSize; i++) {
		char c = buffer[i];
		if (c=='#')
			isComment = true;
		else if (isComment == false)
		{
			if (skipNextWhitespace == false && (c==' ' || c=='\t' || c=='\n')) {
				step++;
				
				if (step == 4) {
					lastPos = i+1;
					break;
				}
				
				skipNextWhitespace = true;
				continue;
			}
			
			bool changed = true;
			if (step==0 && c=='P') // read Magic Number
				if (buffer[i+1]!='3') {
					fputs ("Invalid format. PPM P3 expected.\n",stderr);
					exit (4);
				} else {}
			else if (step==1 && c>47 && c<59) // width
				width = width*10 + c-48;
			else if (step==2 && c>47 && c<59) // height
				height = height*10 + c-48;
			else if (step==3 && c>47 && c<59) // max Value
				maxValue = maxValue*10 + c-48;
			else
				changed = false;
			
			if (changed)
				skipNextWhitespace = false;
		}
		else if (c=='\n')
			isComment = false;
	}
	
	
	auto image = std::make_shared<Image>(Dimension(width, height));
	size_t index = 0;
	size_t singleValue = 0;
	Channel * channels[] = {image->channel1, image->channel2, image->channel3};
	char cSel = 0;
	
	skipNextWhitespace = true;
	
	for (size_t i=lastPos; i < lSize; i++) {
		char c = buffer[i];
		if (c=='#')
			isComment = true;
		else if (isComment == false)
		{
			if (skipNextWhitespace == false && (c==' ' || c=='\t' || c=='\n')) {
				
				channels[cSel]->setValue(index, normalize(singleValue, maxValue, 255));
				
				cSel = (cSel+1)%3;
				if (cSel==0)
					index++;
				singleValue = 0;
				skipNextWhitespace = true;
				continue;
			}
			
			if (c>47 && c<59) {
				singleValue = singleValue*10 + c-48;
				skipNextWhitespace = false;
			}
			
			
		}
		else if (c=='\n')
			isComment = false;
	}
	
	if (singleValue > 0)
		channels[cSel]->setValue(index, normalize(singleValue, maxValue, 255));
	
	
	// terminate
	fclose (file);
	delete[] buffer;
	
	image->colorSpace = ColorSpaceRGB;
	return image;

}

color PPMLoader::normalize(color colorValue, int originalMaxValue, int normalizedMaxValue) {
	return (color) ((colorValue / (float) originalMaxValue) * normalizedMaxValue);
}

int	PPMLoader::scanForPattern(FILE * file, const char * fmt , void* arg0, void* arg1, void* arg2) {
	int actualFound = 0;
	int continueSearch = 1;
	while(continueSearch != EOF && continueSearch > 0) {
		char comment[255];
		continueSearch = fscanf(file, "# %99[^\n]", comment);
		
		// No comments found
		if (continueSearch == 0) {
			if (arg2 != nullptr) {
				actualFound = fscanf(file, fmt, arg0, arg1, arg2);
			} else if (arg1 != nullptr) {
				actualFound = fscanf(file, fmt, arg0, arg1);
			} else {
				actualFound = fscanf(file, fmt, arg0);
			}
		}
	}
	
	return actualFound;
}
