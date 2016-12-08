#ifndef Huffman_hpp
#define Huffman_hpp

#include <map>
#include <vector>
#include "Node.hpp"
#include "../bitstream/Bitstream.hpp"


struct SymbolBits {
	Word bits = 0;
	unsigned short numberOfBits = 0;
	
	SymbolBits() {};
	SymbolBits(Word bits, unsigned short numberOfBits) : bits(bits), numberOfBits(numberOfBits) {};
	
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
	std::map<Symbol, SymbolBits>* generateCanonicalEncodingTable(Node* node);
	std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	
	bool noAllOnesPath = false;
	void climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map);
	std::map<Symbol, int> generateLevelList(Node* node);
	void generateLevelList(int level, Node* node, std::map<Symbol, int> &levelMap);
	bool isLeadingBitsInVector(std::vector<SymbolBits> usedPaths, SymbolBits symbolBits);
};


#endif /* Huffman_hpp */
