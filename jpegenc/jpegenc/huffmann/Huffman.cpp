#include "Huffman.hpp"
#include <iostream>
#include "PackageMerge.hpp"
#include "BitMath.hpp"

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
	for (int i = 0; i < count; ++i)
		addSymbol(input[i]);
}

/** Prepare list with symbols and correlating frequencies */
void Huffman::generateNodeList() {
	singleLeafNodes.clear();
	for (std::pair<Symbol, Frequency> entry : symbolBook) {
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
	
	std::vector<Encoding> encList = generateEncodingList(levelList);
	return generateEncodingTable(singleLeafNodes, encList);
}

/** @return Map with Symbol as key and bit pattern as value. Based on optimal length-limited tree */
const EncodingTable Huffman::lengthLimitedEncoding(Level limit) {
	if (singleLeafNodes.size() > (1 << limit)) {
		limit = BitMath::log2(singleLeafNodes.size());
		printf("Error: Can't create limited tree with given limit. Using limit %d instead.\n", limit);
	}
	std::vector<Level> levelList = PackageMerge().generate(singleLeafNodes, limit);
	std::vector<Encoding> encList = generateEncodingList(levelList);
	return generateEncodingTable(singleLeafNodes, encList);
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
	for (std::pair<Symbol, Encoding> pair : encodingTable) {
		Node* runningNode = root;
		Level remainingLevel = pair.second.numberOfBits;
		
		while (remainingLevel--) {
			// create any missing Nodes while going deeper
			if (runningNode->left == nullptr)
				runningNode->left = new Node();
			if (runningNode->right == nullptr)
				runningNode->right = new Node();
			
			// traverse though bit pattern
			if ((pair.second.code >> remainingLevel) & 1)
				runningNode = runningNode->right;
			else
				runningNode = runningNode->left;
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
	bool bit;
	for (int i = 0; i < numberOfBits; ++i) {
		if (!node) {
			fputs("Error: Bitstream decode, unexpected symbol found.\n", stderr);
			break;
		}
		if (node->isLeaf()) {
			symbols.push_back(node->symbol);
			node = rootNode;
		}
		// TODO: sequencial read + read single symbol
		bit = bitstream->read(i);
		if (bit)
			node = node->right;
		else
			node = node->left;
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
	if (node->isLeaf()) {
		list.push_back(level);
	} else if (node) {
		recursivelyGenerateLevelList(list, node->left, level + 1);
		recursivelyGenerateLevelList(list, node->right, level + 1);
	}
}

/**
 * Transforms a level list to an encoding list
 * @param levelList Must be sorted in @b descending order
 * @return Sorted list of encoding bit codes. Same ordering as @a singleLeafNodes
 */
const std::vector<Encoding> Huffman::generateEncodingList(const std::vector<Level> &levelList) {
	std::vector<Encoding> encodingList;
	Level currentLevel = 255;
	Word code = 0;
	
	for (Level lvl : levelList) { // level == numberOfBits
		if (lvl < currentLevel) { // if level stays the same we just decrease code
			currentLevel = lvl;
			code = (1 << lvl) - 1;
			
			for (Encoding prevCode : encodingList)
				if (code == (prevCode.code >> (prevCode.numberOfBits - lvl)))
					--code; // found, but not a leaf, so decreas and continue
		}
		encodingList.push_back( Encoding(code--, lvl) );
	}
	return encodingList;
}

/**
 * Convert an encoding list to a map with @a Symbol as key
 * @param symbolList Sorted list of symbols, least significant first
 * @param codeList Sorted list of codes, longest sequence first
 * @return The encoding map
 */
const EncodingTable Huffman::generateEncodingTable(const std::vector<Node*> &symbolList, const std::vector<Encoding> &codeList) {
	EncodingTable encodingMap;
	size_t count = symbolList.size();
	
	if (count == codeList.size()) {
		while (count--)
			encodingMap[ symbolList[count]->symbol ] = codeList[count];
		
		encodingMap.erase(DEFAULT_SYMBOL);
	} else {
		fputs("Error: generateEncodingTable() Both lists have to be same size().\n", stderr);
	}
	
	return encodingMap;
}

