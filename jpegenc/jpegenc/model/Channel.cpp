#include "Channel.hpp"

/**
 * Default Constructor
 * @param dim The image size
 * @param blockSize Will resize the image if necessary to allow separation
 */
Channel::Channel(Dimension dim, unsigned short blockSize) : currentBlockSize(blockSize), imageSize(dim) {
	setImageSize(dim, blockSize);
	values = new color[ imageSize.pixelCount ];
	seekTo(ChannelSeekBoth);
}
/**
 * Create a new Channel with bigger dimensions
 * @param base The channel which will be duplicated and enlarged
 * @param xMultiply How often will each values be copied in X direction
 * @param yMultiply How often will each values be copied in Y direction
 * @return New Channel with Dimension( x * xMultiply, y * yMultiply )
 */
Channel* Channel::enlarge(Channel *base, size_t xMultiply, size_t yMultiply) {
	Dimension newSize = Dimension(base->imageSize.width * xMultiply, base->imageSize.height * yMultiply);
	Channel *newChan = new Channel(newSize, base->currentBlockSize);
	
	base->setReadIndexAccessMappingRule(newSize);
	
	size_t byteCount = newSize.pixelCount;
	while (byteCount--)
		newChan->appendValue( base->readNextValue() );
	
	newChan->seekTo(ChannelSeekBoth);
	base->deleteReadIndexAccessMappingRule();
	return newChan;
}



//  ---------------------------------------------------------------
// |
// |  Helper
// |
//  ---------------------------------------------------------------

/**
 * Helper method to get an divisible size
 * @param originalSize Input will be adjusted to to fit block size
 * @param blockSize Image will be divisible by this number
 * @return Can be same size or at most input size + (blockSize - 1)
 */
inline Dimension enlargeSizeToFitBlocksize(Dimension originalSize, unsigned short blockSize) {
	unsigned short missingCols = (blockSize - (originalSize.width % blockSize)) % blockSize;
	unsigned short missingRows = (blockSize - (originalSize.height % blockSize)) % blockSize;
	return Dimension(originalSize.width + missingCols, originalSize.height + missingRows);
}
/**
 * Helper method to set all appropriate internal counter needed for initial block filled write
 * @param size Image size
 * @param blockSize Image will be divisable by this number
 */
inline void Channel::setImageSize(Dimension size, unsigned short blockSize) {
	imageSize = enlargeSizeToFitBlocksize(size, blockSize);
	originalWidth = size.width;
	originalHeight = size.height;
	blockAddCols = (unsigned short)(imageSize.width - originalWidth);
	blockAddRows = (unsigned short)(imageSize.height - originalHeight);
}
/**
 * Set internal data pointer to specified position and recalculate internal counter
 * @param type Set which pointer should be changed (Read, Write, Both)
 * @param index New pointer position (default: START_INDEX)
 */
void Channel::seekTo(ChannelSeekType type, size_t index) {
	if (type == ChannelSeekBoth || type == ChannelSeekWrite) {
		writePointer = &values[index];
		currentWriteColumn = index % imageSize.width;
		currentWriteRow = index / imageSize.width;
	}
	if (type == ChannelSeekBoth || type == ChannelSeekRead) {
		readPointer = &values[index];
		if (index == 0) {
			countSkipX = subsampleReadSkipX;
			countSkipY = subsampleReadSkipY;
			remainingReadInLine = imageSize.width;
		}
	}
}



//  ---------------------------------------------------------------
// |
// |  Conditional Reading / Rules
// |
//  ---------------------------------------------------------------

/**
 * Remove previously set reading condition
 */
void Channel::deleteReadIndexAccessMappingRule() {
	shouldReadSubsampleX = false;
	shouldReadSubsampleY = false;
	seekTo(ChannelSeekRead);
}
/**
 * Apply a new reading condition. Will be used to skip single items or address already existing value multiple times
 * @param mapping_size The supposed size you would like to map it to. Has to be >= the underlying data size.
 */
void Channel::setReadIndexAccessMappingRule(Dimension mapping_size) {
	mapping_size = enlargeSizeToFitBlocksize(mapping_size, currentBlockSize);
	subsampleReadSkipX = mapping_size.width / imageSize.width;
	subsampleReadSkipY = mapping_size.height / imageSize.height;
	shouldReadSubsampleX = (subsampleReadSkipX > 1);
	shouldReadSubsampleY = (subsampleReadSkipY > 1);
	
	countSkipX = subsampleReadSkipX;
	countSkipY = subsampleReadSkipY;
	remainingReadInLine = imageSize.width;
	seekTo(ChannelSeekRead);
	
	if (imageSize.width * subsampleReadSkipX != mapping_size.width ||
		imageSize.height * subsampleReadSkipY != mapping_size.height) {
		fputs("Error: An Access Mapping rule has to be a multiple size of the underlying image.\n", stderr);
	}
}



//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

/**
 * Read current value and move internal pointer to the next value
 * @return A reference to the value which can be edited
 */
color& Channel::readNextValue() {
	if (shouldReadSubsampleX || shouldReadSubsampleY)
		return readNextSubsampleValue();
	return *(readPointer++);
}
/**
 * Read next value but apply some conditions to skip values or re-reading it multiple times
 * @return A reference to the value which can be edited
 */
color& Channel::readNextSubsampleValue() {
	// Subsampling in X dimension (missing every nth column)
	if (shouldReadSubsampleX) {
		if (--countSkipX)
			return *(readPointer);
		
		countSkipX = subsampleReadSkipX;
	}
	// Subsampling in Y dimension (missing every nth row)
	if (shouldReadSubsampleY) {
		if (--remainingReadInLine == 0) {
			remainingReadInLine = imageSize.width;
			if (--countSkipY) {
				color &lastColorInRow = *readPointer;
				readPointer -= (imageSize.width - 1);
				return lastColorInRow;
			}
			countSkipY = subsampleReadSkipY;
		}
	}
	return *(readPointer++);
}



//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

/**
 * Write value to data stream and move internal pointer to next value.
 * The appending can be done anywhere in the data stream. 
 * Use @b seekTo() to move internal pointer position and overwrite data.
 * @param value Data to be written or replaced
 */
void Channel::appendValue(const color &value) {
	*(writePointer++) = value;
}
/**
 * Same as @b appendValue() but with additional logic to duplicate edge values if @a blockSize doesn't fit
 * @param value Data to be written or replaced
 */
void Channel::appendValueAndFillupBlocks(const color &value) {
	*(writePointer++) = value;
	fillupValuesByDuplicating();
}
/**
 * Helper method to duplicate edge values if @a blockSize doesn't fit image dimension
 */
inline void Channel::fillupValuesByDuplicating() {
	if (++currentWriteColumn >= originalWidth) {
		currentWriteColumn = 0;
		// fillup after each line
		unsigned short missingColumns = blockAddCols;
		while (missingColumns--) {
			*writePointer = *(writePointer - 1); // repeat left value
			++writePointer;
		}
		
		// fillup remaining lines at the bottom
		if (++currentWriteRow >= originalHeight) {
			size_t missingRows = blockAddRows * imageSize.width;
			while (missingRows--) {
				*writePointer = *(writePointer - imageSize.width); // repeat the value above
				++writePointer;
			}
		}
	}
}



//  ---------------------------------------------------------------
// |
// |  Sub Sampling + Averaging
// |
//  ---------------------------------------------------------------

/**
 * Discard values between sample points
 * @param stepWidth Skip values in X direction
 * @param stepHeight Skip values in Y direction
 */
void Channel::reduceBySubSampling(unsigned short stepWidth, unsigned short stepHeight) {
	size_t prevWidth = imageSize.width;
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		return values[col * stepWidth + row * stepHeight * prevWidth];
	});
}
/**
 * Sum all sample points and calculate avarage
 * @param stepWidth Take all values in X direction into account
 * @param stepHeight Take all values in Y direction into account
 */
void Channel::reduceByAveraging(unsigned short stepWidth, unsigned short stepHeight) {
	size_t prevWidth = imageSize.width;
	short row_offset = stepHeight;
	short col_offset = stepWidth;
	
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		float sum = 0;
		size_t pxCount = stepWidth * stepHeight;
		
		if (pxCount == 0) // then sub sampling
			return values[col * stepWidth + row * stepHeight * prevWidth];
		
		size_t old_row = row * stepHeight;
		size_t old_col = col * stepWidth;
		row_offset = stepHeight;
		
		while (row_offset--) {
			while (col_offset--) { // Average
				sum += values[old_col + col_offset + (old_row + row_offset) * prevWidth];
			}
			col_offset = stepWidth;
		}
		return (color)(sum / pxCount);
	});
}
/**
 * Helper method for any given sub sampling algorithm
 * @param f A function which will be called on every reduced data point with @b f(row,column)
 */
void Channel::reduceWithFunction(size_t stepWidth, size_t stepHeight, std::function<color(size_t, size_t)> f) {
	
	setImageSize( Dimension(imageSize.width / stepWidth, imageSize.height / stepHeight) );
	
	size_t newByteCount = imageSize.pixelCount;
	for (size_t i = 0; i < newByteCount; i++) {
		size_t row = i / imageSize.width;
		size_t col = i % imageSize.width;
		values[i] = f(row, col);
	}
}

