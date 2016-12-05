#ifndef Huffman_hpp
#define Huffman_hpp

#include <map>
#include <vector>
#include "Node.hpp"
#include "../bitstream/Bitstream.hpp"


struct SymbolBits {
	Word bits = 0;
	unsigned short numberOfBits = 0;
};

class Huffman {
	std::vector<InputWord> words;
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
	
	Node* generateTree();
	Node* generateRightAlignedTree();
	Node* lengthLimitedHuffmanAlgorithm(unsigned short limit);
	
	std::map<Symbol, SymbolBits>* generateEncodingTable(Node* node);
	std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	void climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map);
	
	// A fast algorithm for optimal length-limited Huffman codes
	std::vector<Node*> lengthLimitedHuffmanPackage(std::vector<Node*> input);
	std::vector<Node*> lengthLimitedHuffmanMerge(std::vector<Node*> packagedList);
	void recursivelyCountSymbolMapping(std::map<Symbol, int>& map, Node* node);
	Node* lengthLimitedHuffmanGenerateTree(std::vector<int>& levelList, std::vector<Node*> nodeList);
};


#endif /* Huffman_hpp */
