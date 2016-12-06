#include "Huffman.hpp"
#include <iostream>

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
	for (std::pair<Symbol, unsigned int> entry : symbolBook) {
		singleLeafNodes.push_back( new Node(entry.first, entry.second) );
	}
	std::sort(singleLeafNodes.begin(), singleLeafNodes.end(), sortNode);
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
	
	for (auto &symbol : symbolBits) {
		std::cout << symbol.bits << " " << symbol.numberOfBits << std::endl;
	}
	
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
					std::cout << bitset << std::endl;
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
					std::cout << bitset << std::endl;
				}
			}
		}
	}
	
	return root;
}

// A fast algorithm for optimal length-limited Huffman codes
Node* Huffman::lengthLimitedTree(unsigned short limit) {
	std::vector<Node*> packages;
	std::vector<Node*> evolutionalList = singleLeafNodes;
	for (int i = limit; i > 0; --i) {
		packages = lengthLimitedHuffmanPackage(evolutionalList);
		// TODO: Merge kann hier mit 3 Code Zeilen implementiert werden
		evolutionalList = lengthLimitedHuffmanMerge(packages);
	}
	// TODO: Logic for tree evaluation can be implemented in HuffmanPackage directly
	std::map<Symbol, int> treeCreationMap;
	std::vector<int> levelList;
	for (Node *n : singleLeafNodes)
		treeCreationMap[n->symbol] = 0;
	for (Node *n : evolutionalList)
		recursivelyCountSymbolMapping(treeCreationMap, n);
	for (int i = 0; i < singleLeafNodes.size(); ++i)
		levelList.push_back( treeCreationMap[singleLeafNodes[i]->symbol] -1 );
	
	return lengthLimitedHuffmanGenerateTree(levelList, singleLeafNodes);
}

void Huffman::recursivelyCountSymbolMapping(std::map<Symbol, int>& map, Node* node) {
	if (node->left != nullptr)
		recursivelyCountSymbolMapping(map, node->left);
	if (node->right != nullptr)
		recursivelyCountSymbolMapping(map, node->right);
	if (node->left == nullptr && node->right == nullptr)
		map[node->symbol] += 1;
}

std::vector<Node*> Huffman::lengthLimitedHuffmanPackage(std::vector<Node*> input) {
	std::vector<Node*> newList;
	size_t count = input.size();
	for (int i = 0; i < count; i += 2) {
		if ((i + 1) < count) {
			// TODO: generate list of Symbol count here?
			newList.push_back( new Node(input[i+1], input[i]) );
		}
	}
	return newList;
}

std::vector<Node*> Huffman::lengthLimitedHuffmanMerge(std::vector<Node*> packagedList) {
	std::vector<Node*> newList;
	for (Node *n : singleLeafNodes)
		newList.push_back(n);
	for (Node *n : packagedList)
		newList.push_back(n);
	std::sort(newList.begin(), newList.end(), sortNode);
	return newList;
}

Node* Huffman::lengthLimitedHuffmanGenerateTree(std::vector<int>& levelList, std::vector<Node*> nodeList) {
	size_t count = levelList.size();
	if (count != nodeList.size()) {
		fputs("Something is wrong. LevelList and NodeList should always be in sync\n", stderr);
		return nullptr;
	}
	
	if (count <= 1)
		return nodeList[0]; // tree generation done
	
	std::vector<int> newLevelList;
	std::vector<Node*> newNodeList;
	unsigned int currentLevel = levelList[0];
	for (int i = 0; i < count; ++i) {
		if (i+1 < count && levelList[i] == currentLevel && levelList[i+1] == currentLevel) {
			newLevelList.push_back(levelList[i] - 1);
			newNodeList.push_back( new Node(nodeList[i+1], nodeList[i]) );
			++i;
		} else { // dont merge anything, just copy
			newLevelList.push_back(levelList[i]);
			newNodeList.push_back(nodeList[i]);
		}
	}
	
	if (levelList == newLevelList) {
		fputs("Something went wrong. TODO: prevent a too shallow depth limit here\n", stderr);
		return nullptr;
	}
	
	return lengthLimitedHuffmanGenerateTree(newLevelList, newNodeList);
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


