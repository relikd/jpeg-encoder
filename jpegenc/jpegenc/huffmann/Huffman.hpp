#ifndef Huffman_hpp
#define Huffman_hpp

#include <map>
#include <vector>
#include "Node.hpp"
#include "../bitstream/Bitstream.hpp"

struct Encoding {
	Word code = 0;
	unsigned short numberOfBits = 0;
	
	Encoding() {};
	Encoding(Word code, unsigned short numberOfBits) : code(code), numberOfBits(numberOfBits) {};
};

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
	const EncodingTable lengthLimitedEncoding(Level limit);
	
	// Step III: Trees
	Node* standardTree();
	static Node* treeFromEncodingTable(const EncodingTable &encodingTable);
	
	// Step IV: Decode With Given Tree
	static std::vector<Symbol> decode(Bitstream* bitstream, Node* rootNode);
	
private:
	void recursivelyGenerateLevelList(std::vector<Level> &list, Node* node, Level level = 0);
	const std::vector<Encoding> generateEncodingList(const std::vector<Level> &levelList);
	const EncodingTable generateEncodingTable(const std::vector<Node*> &symbolList, const std::vector<Encoding> &codeList);
};


#endif /* Huffman_hpp */
