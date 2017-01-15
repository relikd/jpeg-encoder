#include <algorithm>
#include <functional>
#include "Huffman.hpp"
#include "PackageMerge.hpp"
#include "../helper/BitMath.hpp"

//  ---------------------------------------------------------------
// |
// |  Step I: Setup & Preparation
// |
//  ---------------------------------------------------------------

/** Add a single symbol to an internal map and increase it's frequency by 1 */
void Huffman::addSymbol(Symbol input) {
//	symbolBook.insert(input, 0); // no need to since vector is initialized with 0
	symbolBook[input] += 1;
}

/** Each element in given array will call @b addSymbol() */
void Huffman::addSymbols(std::vector<Symbol> input) {
	size_t count = input.size();
	for (size_t i = 0; i < count; ++i)
		addSymbol(input[i]);
}

/** Prepare list with symbols and correlating frequencies */
void Huffman::generateNodeList() {
	singleLeafNodes.clear();
	for (const std::pair<Symbol, Frequency> &entry : symbolBook) {
		singleLeafNodes.push_back( new Node(entry.first, entry.second) );
	}
	std::sort(singleLeafNodes.begin(), singleLeafNodes.end(), sortNode);
}

/**
 This will prevent an encoding where every bit is 1. The rightmost leaf will be replaced by -1
 * @param insertArtificialLeaf @b True: insert -1 @b False: remove previously inserted True
 */
void Huffman::preventAllOnesPath(bool insertArtificialLeaf) {
	if (insertArtificialLeaf) {
		if (singleLeafNodes[0]->frequency != 0)
			singleLeafNodes.insert(singleLeafNodes.begin(), new Node(DEFAULT_SYMBOL, 0));
	} else {
		if (singleLeafNodes[0]->frequency == 0)
			singleLeafNodes.erase(singleLeafNodes.begin());
	}
}


//  ---------------------------------------------------------------
// |
// |  Step II: Generate Encoding Table
// |
//  ---------------------------------------------------------------

/** @return Map with Symbol as key and bit pattern as value. Based on a right-aligned tree */
const EncodingTable Huffman::canonicalEncoding() {
	Node* huffmanTree = standardTree();
	
	std::vector<Level> levelList;
	recursivelyGenerateLevelList(levelList, huffmanTree);
	sort(levelList.begin(), levelList.end(), std::greater<Level>());
	
	return generateEncodingTable(levelList);
}

/** @return Map with Symbol as key and bit pattern as value. Based on optimal length-limited tree */
const EncodingTable Huffman::canonicalEncoding(Level lengthLimit) {
	if ((long long)singleLeafNodes.size() > (long long)1 << lengthLimit) {
		lengthLimit = BitMath::log2(singleLeafNodes.size());
		printf("Error: Can't create limited tree with given limit. Using limit %d instead.\n", lengthLimit);
	}
	std::vector<Level> levelList = PackageMerge().generate(singleLeafNodes, lengthLimit);
	return generateEncodingTable(levelList);
}


//  ---------------------------------------------------------------
// |
// |  Step III: Trees
// |
//  ---------------------------------------------------------------

/** Classical Huffman tree by combining the two least frequent nodes */
Node* Huffman::standardTree() {
	std::vector<Node*> input = singleLeafNodes;
	while (input.size() > 1) {
		std::sort(input.begin(), input.end(), sortNode);
		input.push_back( new Node(input[0], input[1], input[0]->frequency > input[1]->frequency) );
		input.erase(input.begin(), input.begin() + 2);
	}
	return input[0];
}

/** Create Huffman Tree from an given encoding table */
Node* Huffman::treeFromEncodingTable(const EncodingTable &encodingTable) {
	Node* root = new Node();
	for (const std::pair<Symbol, Encoding> &pair : encodingTable) {
		Node* runningNode = root;
		Level remainingLevel = pair.second.numberOfBits;
		
		while (remainingLevel--) {
			// create any missing Nodes while going deeper
			if (runningNode->isLeaf()) {
				runningNode->left = new Node();
				runningNode->right = new Node();
			}
			runningNode = runningNode->deeper( (pair.second.code >> remainingLevel) & 1 );
		}
		runningNode->symbol = pair.first;
	}
	return root;
}


//  ---------------------------------------------------------------
// |
// |  Step IV: Decode With Given Tree
// |
//  ---------------------------------------------------------------

/** Parse whole bitstream with given tree and return vector with all Symbols */
std::vector<Symbol> Huffman::decode(Bitstream* bitstream, Node* rootNode) {
	std::vector<Symbol> symbols;
	size_t numberOfBits = bitstream->numberOfBits();
	Node* node = rootNode;
	for (size_t i = 0; i < numberOfBits; ++i) {
		if (!node) {
			fputs("Error: Bitstream decode, unexpected symbol found.\n", stderr);
			break;
		}
		if (node->isLeaf()) {
			symbols.push_back(node->symbol);
			node = rootNode;
		}
		// TODO: sequencial read + read single symbol
		node = node->deeper( bitstream->read(i) );
	}
	
	return symbols;
}


//  ---------------------------------------------------------------
// |
// |  Internal
// |
//  ---------------------------------------------------------------

/**
 * Traverse the whole tree and save the level of each leaf node
 * @param list Result will be saved here
 * @param node Pass root node as a starting point
 */
void Huffman::recursivelyGenerateLevelList(std::vector<Level> &list, Node* node, Level level) {
	if (node) {
		if (node->isLeaf()) {
			list.push_back(level);
		} else {
			recursivelyGenerateLevelList(list, node->left, level + 1);
			recursivelyGenerateLevelList(list, node->right, level + 1);
		}
	}
}


/**
 * Convert a level list to a map with @a Symbol as key
 * @param levelList Must be sorted in @b descending order
 * @return The encoding map
 * @see http://imrannazar.com/Let%27s-Build-a-JPEG-Decoder%3A-Huffman-Tables
 */
const EncodingTable Huffman::generateEncodingTable(const std::vector<Level> &levelList) {
	EncodingTable encodingMap;
	Encoding enc = Encoding(0b0, 1);
	size_t count = levelList.size();
	
	if (count != singleLeafNodes.size()) {
		fputs("Error: generateEncodingTable() Level list has to be the same size() like nodes list.\n", stderr);
	} else {
		while (count--) { // for all symbols
			while (enc.numberOfBits < levelList[count]) { // between levels
				enc.code <<= 1;
				enc.numberOfBits++;
			}
			encodingMap[ singleLeafNodes[count]->symbol ] = enc;
			enc.code++; // on same level
		}
		encodingMap.erase(DEFAULT_SYMBOL); // discard artificial node
	}
	
	return encodingMap;
}

