#ifndef PackageMerge_hpp
#define PackageMerge_hpp

#include "Node.hpp"
#include <vector>

struct PackageNode {
	Frequency frequency = 0;
	std::vector<Symbol> symbols;
	
	PackageNode(Symbol s, Frequency f) : frequency(f) { symbols.push_back(s); }
	
	void combine(PackageNode other) { // heavyNode.combine(lightNode) is faster
		frequency += other.frequency;
		for (Symbol s : other.symbols)
			symbols.push_back(s);
	}
};


// A fast algorithm for optimal length-limited Huffman codes
class PackageMerge {
	std::vector<PackageNode> nodesListOriginal;
	std::vector<PackageNode> nodesListPackaged;
	
public:
	Node* generate(const std::vector<Node*> &input, unsigned short limit);
	
private:
	void package();
	void merge();
	std::vector<unsigned short> calculateLevelList();
	Node* generateTree(const std::vector<unsigned short>& levelList, const std::vector<Node*> &nodeList);
};

#endif /* PackageMerge_hpp */
