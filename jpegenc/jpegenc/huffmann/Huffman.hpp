#ifndef Huffman_hpp
#define Huffman_hpp

#include <map>
#include <vector>
#include "Node.hpp"
#include "../bitstream/Bitstream.hpp"


struct SymbolBits {
	Word bits = 0;
	unsigned short numberOfBits = 0;
	
	bool operator  < (const SymbolBits& input) const { return (numberOfBits  < input.numberOfBits); }
	bool operator  > (const SymbolBits& input) const { return (numberOfBits  > input.numberOfBits); }
};

class Huffman {
	std::map<Symbol, Frequency> symbolBook;
	std::vector<Node*> singleLeafNodes; // sorted: least significant ones first
	
public:
	Huffman() {}
	Huffman(std::vector<Symbol> symbols) {
		addSymbols(symbols);
		generateNodeList();
	}
	
	void addSymbol(Symbol);
	void addSymbols(std::vector<Symbol>);
	
	void generateNodeList();
	void preventAllOnesPath(bool prevent = true);
	
	Node* standardTree();
	Node* canonicalTree();
	Node* lengthLimitedTree(unsigned short limit);
	
	std::map<Symbol, SymbolBits>* generateEncodingTable(Node* node);
	std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	void climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map);
};


#endif /* Huffman_hpp */
