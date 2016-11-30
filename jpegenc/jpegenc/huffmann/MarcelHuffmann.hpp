//
//  MarcelHuffmann.hpp
//  jpegenc
//
//  Created by Marcel Groß on 22.11.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef MarcelHuffmann_hpp
#define MarcelHuffmann_hpp

#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
#include "../bitstream/Bitstream.hpp"

typedef int Symbol;

struct InputWord {
	Symbol symbol = -1;
	int amount;
	
	InputWord(int amount, Symbol symbol) : amount(amount), symbol(symbol) {}
	InputWord(Symbol symbol) : symbol(symbol), amount(1) {}
	
	void increase();
	
	bool operator < (const InputWord& input) const {
		return (amount < input.amount);
	}
	
	bool operator == (const InputWord& input) const {
		return (amount == input.amount);
	}
};

struct Node {
	Node* left = nullptr; //0
	Node* right = nullptr; //1
	InputWord* value = nullptr;
	unsigned short depth = 0;
	
	Node(){}
	Node(InputWord* inputWord) : value(inputWord) {}
	
	void calculateValue();
	void print();
	
private:
	void printWithDepth(std::vector<Node*> arr, int level);
};

struct SymbolBits {
	Word bits = 0;
	unsigned short numberOfBits = 0;
};



class MarcelHuffmann {
	std::vector<InputWord> words; //tree generation works only if words are in mem.
	
	
public:
	MarcelHuffmann() {}
	
	void addToWords(std::vector<Symbol>);
	Node* generateTree();
	std::vector<Node> generateNodeList();
	Node* generateRightAlignedTree(std::vector<Node> input);
	std::map<Symbol, SymbolBits>* generateEncodingTable(Node* node);
	std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	void sortByAppearance();
	void climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map);
};


#endif /* MarcelHuffmann_hpp */
