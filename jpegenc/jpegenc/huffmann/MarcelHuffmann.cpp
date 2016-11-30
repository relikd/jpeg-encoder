//
//  MarcelHuffmann.cpp
//  jpegenc
//
//  Created by Marcel Groß on 22.11.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "MarcelHuffmann.hpp"

void InputWord::increase() {
	++amount;
}




void MarcelHuffmann::addToWords(std::vector<int> input) {
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
			words.push_back(InputWord(input[i]));
		}
		
	}
	
	sortByAppearance();
}

Node* MarcelHuffmann::generateTree() {
	Node* node = new Node();
	node->left = new Node(&words[0]);
	node->right = new Node(&words[1]);
	node->depth = 1;
	node->calculateValue();
	
	InputWord* currentWord;
	Node* newNode;
	for (int i = 2; i < words.size(); ++i) {
		currentWord = &words[i];
		newNode = new Node();
		newNode->depth = node->depth + 1;
		if (currentWord->amount > node->value->amount) {
			newNode->right = new Node(currentWord);
			newNode->left = node;
		} else{
			newNode->left = new Node(currentWord);
			newNode->right = node;
		}
		newNode->calculateValue();
		node = newNode;
	}
	
	return node;
}


std::vector<Node> MarcelHuffmann::generateNodeList() {
	std::vector<Node> nodeList;
	for (int i = 0; i < words.size(); ++i) {
		nodeList.push_back(*new Node(&words[i]));
	}
	return nodeList;
}

bool sortNode(const Node &node1, const Node &node2) {
	if (node1.value->amount == node2.value->amount) {
		return node1.depth < node2.depth;
	}
	return node1.value->amount < node2.value->amount;
}

/*Node* MarcelHuffmann::generateRightAlignedTree(std::vector<Node> input){
	std::sort(input.begin(), input.end(), sortNode);
	
	
	if (input.size() <= 1) {
		return &input[0];
	} else {
		Node* root = new Node();
		root->left = &input[0];
		root->right = &input[1];
		root->depth = root->right->depth + 1;
		root->calculateValue();
		
		input.push_back(*root);
		input.erase(input.begin(), input.begin() + 2);
		
		return generateRightAlignedTree(input);

	}
}
*/

Node* MarcelHuffmann::generateRightAlignedTree(std::vector<Node> input){
	while (input.size() > 1) {
		std::sort(input.begin(), input.end(), sortNode);
		Node* root = new Node();
		root->left = &input[0];
		root->right = &input[1];
		root->depth = root->right->depth + 1;
		root->calculateValue();
		
		input.push_back(*root);
		input.erase(input.begin(), input.begin() + 2);
	}
	
	return &input[0];
}



std::map<Symbol, SymbolBits>* MarcelHuffmann::generateEncodingTable(Node* node) {
	SymbolBits bitsForSymbol;
	std::map<Symbol, SymbolBits>* map = new std::map<Symbol, SymbolBits>();
	climbTree(bitsForSymbol, node, map);
	
	return map;
}

void MarcelHuffmann::climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map) {
	Node* left = node->left;
	Node* right = node->right;
	
	if (left == nullptr && right == nullptr) {
		map->insert(std::make_pair(node->value->symbol, bitsForSymbol));
	} else{
		++bitsForSymbol.numberOfBits;
		bitsForSymbol.bits <<= 1;
		climbTree(bitsForSymbol, left, map);
		bitsForSymbol.bits |= 1;
		climbTree(bitsForSymbol, right, map);
	}
}

std::vector<Symbol> MarcelHuffmann::decode(Bitstream* bitstream, Node* rootNode) {
	std::vector<Symbol> symbols;
	size_t numberOfBits = bitstream->numberOfBits();
	Node* node = rootNode;
	bool bit;
	for (int i = 0; i < numberOfBits; ++i) {
	
		if (node->left == nullptr && node->right == nullptr) {
			symbols.push_back(node->value->symbol);
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


void MarcelHuffmann::sortByAppearance() {
	std::sort(words.begin(), words.end());
}




void Node::calculateValue() {
	Node* left = this->left;
	Node* right = this->right;
	
	if ((left != nullptr) && (right != nullptr)) {
		this->value = new InputWord(left->value->amount + right->value->amount, NULL);
	}
	
}


void Node::print() {
	std::vector<Node*> arr;
	arr.push_back(this);
	this->printWithDepth(arr, depth);
}

void Node::printWithDepth(std::vector<Node*> arr, int level) {
	for (int x = (1 << (level+1) ) - 2; x > 0; --x)
		printf(" ");
	
	std::vector<Node*> newArr;
	unsigned long maxWidth = arr.size();
	for (int i = 0; i < maxWidth; i++) {
		
		if (arr[i] == NULL) {
			newArr.push_back(NULL);
			newArr.push_back(NULL);
		} else {
			newArr.push_back(arr[i]->left);
			newArr.push_back(arr[i]->right);
		}
		
		if (arr[i] == NULL) {
			printf("   "); // change the three lines below if you change padding (03)
		} else {
			if (arr[i]->depth > 0)
				printf(" * ");
			else
				printf("%03d", arr[i]->value->symbol);
		}
		
		if (i < maxWidth-1) {
			for (int x = ((((1 << (level)) - 1) * 4) + 1); x > 0; --x) // change 4 if number padding isnt 3
				printf(" ");
		}
	}
	printf("\n");
	if (level > 0)
		printWithDepth(newArr, level-1);
}
