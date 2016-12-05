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
	std::vector<InputWord> words; //tree generation works only if words are in mem.
	
	
public:
	Huffman() {}
	
	void addToWords(std::vector<Symbol>);
	Node* generateTree();
	std::vector<Node*> generateNodeList();
	Node* generateRightAlignedTree(std::vector<Node*> input);
	std::map<Symbol, SymbolBits>* generateEncodingTable(Node* node);
	std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
	// A fast algorithm for optimal length-limited Huffman codes
	Node* lengthLimitedHuffmanAlgorithm(unsigned short limit);
	
private:
	void climbTree(SymbolBits bitsForSymbol, Node* node, std::map<Symbol, SymbolBits>* map);
	
	// A fast algorithm for optimal length-limited Huffman codes
	std::vector<Node*> lengthLimitedHuffmanPackage(std::vector<Node*> input);
	std::vector<Node*> lengthLimitedHuffmanMerge(std::vector<Node*> originalList, std::vector<Node*> packagedList);
	void recursivelyCountSymbolMapping(std::map<Symbol, int>& map, Node* node);
	Node* lengthLimitedHuffmanGenerateTree(std::vector<int>& levelList, std::vector<Node*> nodeList);
};


#endif /* Huffman_hpp */
