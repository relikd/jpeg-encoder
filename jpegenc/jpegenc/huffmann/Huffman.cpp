#include "Huffman.hpp"

void Huffman::addToWords(std::vector<int> input) {
	for (int i = 0; i < input.size(); ++i) {
		bool notFount = true;
		for (int j = 0; j < words.size(); ++j) {
			if (input[i] == words[j].symbol) {
				words[j].increase();
				notFount = false;
				break;
			}
		}
		if (notFount) {
			words.push_back(InputWord(1, input[i]));
		}
	}
	std::sort(words.begin(), words.end());
}

Node* Huffman::generateTree() {
	Node* node = new Node( words[0], words[1] );
	
	Node* newNode;
	for (int i = 2; i < words.size(); ++i) {
		InputWord currentWord = words[i];
		bool shouldSwapLeftRight = (currentWord.amount > node->value.amount);
		newNode = new Node( new Node(currentWord), node, shouldSwapLeftRight );
		node = newNode;
	}
	
	return node;
}


std::vector<Node*> Huffman::generateNodeList() {
	std::vector<Node*> nodeList;
	for (int i = 0; i < words.size(); ++i) {
		nodeList.push_back( new Node(words[i]) );
	}
	return nodeList;
}

bool sortNode(const Node* node1, const Node* node2) {
	if (node1->value.amount == node2->value.amount) {
		return node1->depth < node2->depth;
	}
	return node1->value.amount < node2->value.amount;
}

Node* Huffman::generateRightAlignedTree(std::vector<Node*> input){
	while (input.size() > 1) {
		std::sort(input.begin(), input.end(), sortNode);
		input.push_back( new Node(input[0], input[1], input[0]->depth > input[1]->depth) );
		input.erase(input.begin(), input.begin() + 2);
	}
	return input[0];
}

// A fast algorithm for optimal length-limited Huffman codes
Node* Huffman::lengthLimitedHuffmanAlgorithm(unsigned short limit) {
	std::vector<Node*> origList = generateNodeList();
	std::sort(origList.begin(), origList.end(), sortNode);
	
	std::vector<Node*> packages;
	std::vector<Node*> evolutionalList = origList;
	for (int i = limit; i > 0; --i) {
		packages = lengthLimitedHuffmanPackage(evolutionalList);
		// TODO: Merge kann hier mit 3 Code Zeilen implementiert werden
		evolutionalList = lengthLimitedHuffmanMerge(origList, packages);
	}
	// TODO: Logic for tree evaluation can be implemented in HuffmanPackage directly
	std::map<Symbol, int> treeCreationMap;
	std::vector<int> levelList;
	for (Node *n : origList)
		treeCreationMap[n->value.symbol] = 0;
	for (Node *n : evolutionalList)
		recursivelyCountSymbolMapping(treeCreationMap, n);
	for (int i = 0; i < origList.size(); ++i)
		levelList.push_back( treeCreationMap[origList[i]->value.symbol] );
	
	return lengthLimitedHuffmanGenerateTree(levelList, origList);
}

void Huffman::recursivelyCountSymbolMapping(std::map<Symbol, int>& map, Node* node) {
	if (node->left != nullptr)
		recursivelyCountSymbolMapping(map, node->left);
	if (node->right != nullptr)
		recursivelyCountSymbolMapping(map, node->right);
	if (node->left == nullptr && node->right == nullptr)
		map[node->value.symbol] += 1;
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

std::vector<Node*> Huffman::lengthLimitedHuffmanMerge(std::vector<Node*> originalList, std::vector<Node*> packagedList) {
	std::vector<Node*> newList;
	for (Node *n : originalList)
		newList.push_back(n);
	for (Node *n : packagedList)
		newList.push_back(n);
	std::sort(newList.begin(), newList.end(), sortNode);
	return newList;
}

Node* Huffman::lengthLimitedHuffmanGenerateTree(std::vector<int>& levelList, std::vector<Node*> nodeList) {
	size_t count = levelList.size();
	if (count != nodeList.size())
		fputs("Something is wrong. LevelList and NodeList should always be in sync", stderr);
	
	if (count == 1)
		return nodeList[0]; // tree generation done
	
	std::vector<int> newLevelList;
	std::vector<Node*> newNodeList;
	for (int i = 0; i < count; ++i) {
		if (i+1 < count && levelList[i] == levelList[i+1]) {
			newLevelList.push_back(levelList[i] - 1);
			newNodeList.push_back( new Node(nodeList[i+1], nodeList[i]) );
			++i;
		} else { // dont merge anything, just copy
			newLevelList.push_back(levelList[i]);
			newNodeList.push_back(nodeList[i]);
		}
	}
	return lengthLimitedHuffmanGenerateTree(newLevelList, newNodeList);
}


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
		map->insert(std::make_pair(node->value.symbol, bitsForSymbol));
	} else{
		++bitsForSymbol.numberOfBits;
		bitsForSymbol.bits <<= 1;
		climbTree(bitsForSymbol, left, map);
		bitsForSymbol.bits |= 1;
		climbTree(bitsForSymbol, right, map);
	}
}

std::vector<Symbol> Huffman::decode(Bitstream* bitstream, Node* rootNode) {
	std::vector<Symbol> symbols;
	size_t numberOfBits = bitstream->numberOfBits();
	Node* node = rootNode;
	bool bit;
	for (int i = 0; i < numberOfBits; ++i) {
	
		if (node->left == nullptr && node->right == nullptr) {
			symbols.push_back(node->value.symbol);
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


