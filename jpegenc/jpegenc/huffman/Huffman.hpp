#ifndef Huffman_hpp
#define Huffman_hpp

#include <map>
#include <vector>
#include "Node.hpp"
#include "Encoding.hpp"
#include "../bitstream/Bitstream.hpp"

typedef std::map<Symbol, Encoding> EncodingTable;


class Huffman {
	std::map<Symbol, Frequency> symbolBook;
	std::vector<Node*> singleLeafNodes; // sorted: least significant ones first
	
public:
	// Constructor
	Huffman() {}
	Huffman(std::vector<Symbol> symbols) {
		addSymbols(symbols);
		generateNodeList();
	}
	
	// Step I: Setup & Preparation
	void addSymbol(Symbol);
	void addSymbols(std::vector<Symbol>);
	void preventAllOnesPath(bool prevent = true);
	void generateNodeList();
	
	// Step II: Generate Encoding Table
	const EncodingTable canonicalEncoding();
	const EncodingTable canonicalEncoding(Level lengthLimit);
	
	// Step III: Trees
	Node* standardTree();
	static Node* treeFromEncodingTable(const EncodingTable &encodingTable);
	
	// Step IV: Decode With Given Tree
	static std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	void recursivelyGenerateLevelList(std::vector<Level> &list, Node* node, Level level = 0);
	const EncodingTable generateEncodingTable(const std::vector<Level> &levelList);
};


#endif /* Huffman_hpp */
