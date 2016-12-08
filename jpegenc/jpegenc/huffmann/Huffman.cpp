#include "Huffman.hpp"
#include <iostream>
#include "PackageMerge.hpp"

//  ---------------------------------------------------------------
// |
// |  Step I: Setup
// |
//  ---------------------------------------------------------------

void Huffman::addSymbol(Symbol input) {
//	symbolBook.insert(input, 0); // no need to since vector is initialized with 0
	symbolBook[input] += 1;
}

void Huffman::addSymbols(std::vector<int> input) {
	size_t count = input.size();
	for (int i = 0; i < count; ++i)
		addSymbol(input[i]);
}

void Huffman::generateNodeList() {
	singleLeafNodes.clear();
	for (std::pair<Symbol, Frequency> entry : symbolBook) {
		singleLeafNodes.push_back( new Node(entry.first, entry.second) );
	}
	std::sort(singleLeafNodes.begin(), singleLeafNodes.end(), sortNode);
}

void Huffman::preventAllOnesPath(bool insertArtificialZeroFrequency) {
	noAllOnesPath = insertArtificialZeroFrequency;
	if (insertArtificialZeroFrequency) {
		if (singleLeafNodes[0]->frequency != 0)
			singleLeafNodes.insert(singleLeafNodes.begin(), new Node(DEFAULT_SYMBOL, 0));
	} else {
		if (singleLeafNodes[0]->frequency == 0)
			singleLeafNodes.erase(singleLeafNodes.begin());
	}
}


//  ---------------------------------------------------------------
// |
// |  Step II: Generate Optimal Tree
// |
//  ---------------------------------------------------------------

Node* Huffman::standardTree() {
	std::vector<Node*> input = singleLeafNodes;
	while (input.size() > 1) {
		std::sort(input.begin(), input.end(), sortNode);
		input.push_back( new Node(input[0], input[1], input[0]->frequency > input[1]->frequency) );
		input.erase(input.begin(), input.begin() + 2);
	}
	return input[0];
}

Node* Huffman::canonicalTree() {
	Node* huffmanTree = standardTree();
	auto encodingTableHuffmanTree = generateEncodingTable(huffmanTree);
	std::vector<SymbolBits> symbolBits;
	for(std::map<Symbol,SymbolBits>::iterator it = encodingTableHuffmanTree->begin(); it != encodingTableHuffmanTree->end(); ++it) {
		it->second.bits = it->first;
		symbolBits.push_back(it->second);
	}
	std::sort(symbolBits.begin(), symbolBits.end(), std::greater<SymbolBits>());
	
//	auto set = std::bitset<16>(2);
//	std::cout << set[2] << std::endl;
	int maxDepth = symbolBits[0].numberOfBits;
	auto root = new Node();
	root->depth = maxDepth;
	int iteration = 1;
	while (symbolBits.size() > 0) {
		auto currentNode = root;
		auto bitset = std::bitset<8>((1 << symbolBits[0].numberOfBits) - iteration);
//		std::cout << "Begin: " << bitset << std::endl;
		
//		std::cout << bitset << std::endl;
		
		for (int i = symbolBits[0].numberOfBits - 1; i >= 0; --i) {
			if(bitset[i]) {
				if (currentNode->right != nullptr && i == 0) {
					++iteration;
					break;
				} else if (currentNode->right == nullptr) {
					currentNode->right = new Node();
					currentNode->right->depth =  symbolBits[0].numberOfBits - (symbolBits[0].numberOfBits - i);
				}
				
				currentNode = currentNode->right;
				
				if (i == 0) {
					currentNode->symbol = symbolBits[0].bits;
					currentNode->frequency = symbolBits[0].numberOfBits;
					
					iteration = 1;
					symbolBits.erase(symbolBits.begin(), symbolBits.begin() + 1);
//					std::cout << bitset << std::endl;
				}
			} else {
				if (currentNode->left != nullptr && i == 0) {
					++iteration;
					break;
				} else if (currentNode->left == nullptr) {
					currentNode->left = new Node();
					currentNode->left->depth = symbolBits[0].numberOfBits - (symbolBits[0].numberOfBits - i);
				}
				currentNode = currentNode->left;
				
				if (i == 0) {
					currentNode->symbol = symbolBits[0].bits;
					currentNode->frequency = symbolBits[0].numberOfBits;
					
					iteration = 1;
					symbolBits.erase(symbolBits.begin(), symbolBits.begin() + 1);
//					std::cout << bitset << std::endl;
				}
			}
		}
	}
	
	return root;
}

Node* Huffman::lengthLimitedTree(unsigned short limit) {
	return PackageMerge().generate(singleLeafNodes, limit);
}


//  ---------------------------------------------------------------
// |
// |  Step III: Generate Encoding Table
// |
//  ---------------------------------------------------------------
std::map<Symbol, SymbolBits>* Huffman::generateEncodingTable(Node* node) {
	SymbolBits bitsForSymbol;
	std::map<Symbol, SymbolBits>* map = new std::map<Symbol, SymbolBits>();
	climbTree(bitsForSymbol, node, map);
	
	return map;
}

std::map<Symbol, SymbolBits>* Huffman::generateCanonicalEncodingTable(Node* node) {
	auto encodingTable = new std::map<Symbol, SymbolBits>();
	std::vector<SymbolBits> usedPaths;
	auto levelList = generateLevelList(node);
	
	auto oldLevel = -1;
	int pathInTree = 0;
	int startIndex = 0;
	
	if (noAllOnesPath) {
		oldLevel = levelList[singleLeafNodes[1]->symbol];
		pathInTree = (1 << oldLevel) - 2;
		startIndex = 1;
	}
	
	for (; startIndex < singleLeafNodes.size(); ++startIndex) {
		auto singleLeafNode = singleLeafNodes[startIndex];
		auto level = levelList[singleLeafNode->symbol];
		
		if (level != oldLevel) {
			pathInTree = (1 << level) - 1;
			oldLevel = level;
		}
		
		for (; pathInTree >= 0; --pathInTree) {
			SymbolBits symbolBits(pathInTree, level);
			if (isLeadingBitsInVector(usedPaths, symbolBits)) {
				continue;
			} else {
				encodingTable->insert(std::make_pair(singleLeafNode->symbol, symbolBits));
				usedPaths.push_back(symbolBits);
				--pathInTree;
				break;
			}
		}
	}
	return encodingTable;
}



void Huffman::climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map) {
	Node* left = node->left;
	Node* right = node->right;
	
	if (left == nullptr && right == nullptr) {
		map->insert(std::make_pair(node->symbol, bitsForSymbol));
	} else{
		++bitsForSymbol.numberOfBits;
		bitsForSymbol.bits <<= 1;
		climbTree(bitsForSymbol, left, map);
		bitsForSymbol.bits |= 1;
		climbTree(bitsForSymbol, right, map);
	}
}


//  ---------------------------------------------------------------
// |
// |  Step IV: Decode With Given Tree
// |
//  ---------------------------------------------------------------
std::vector<Symbol> Huffman::decode(Bitstream* bitstream, Node* rootNode) {
	std::vector<Symbol> symbols;
	size_t numberOfBits = bitstream->numberOfBits();
	Node* node = rootNode;
	bool bit;
	for (int i = 0; i < numberOfBits; ++i) {
	
		if (node->left == nullptr && node->right == nullptr) {
			symbols.push_back(node->symbol);
			node = rootNode;
		}
		
		bit = bitstream->read(i);
		if (bit) {
			node = node->right;
		} else {
			node = node->left;
		}
	}
	
	return symbols;
}

std::map<Symbol, int> Huffman::generateLevelList(Node* node){
	std::map<Symbol, int> levelList;
	generateLevelList(0,node, levelList);
	
	return levelList;
}

void Huffman::generateLevelList(int level, Node* node, std::map<Symbol, int> &levelList){
	if (node == nullptr) {
		return;
	}
	
	if (node->symbol != -1) {
		levelList[node->symbol] = level;
	}
	++level;
	generateLevelList(level, node->left, levelList);
	generateLevelList(level, node->right, levelList);
	
	
}

bool Huffman::isLeadingBitsInVector(std::vector<SymbolBits> usedPaths, SymbolBits symbolBits) {
	for (auto current: usedPaths) {
		Word shiftedBits = 0;
		if (current.numberOfBits > symbolBits.numberOfBits) {
			shiftedBits = current.bits >> (current.numberOfBits - symbolBits.numberOfBits);
		} else {
			shiftedBits = current.bits >> (symbolBits.numberOfBits - current.numberOfBits);
		}
		
		if (shiftedBits == symbolBits.bits) {
			return true;
		}
	}
	
	return false;
}

