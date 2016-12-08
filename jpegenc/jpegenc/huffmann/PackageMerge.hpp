#ifndef PackageMerge_hpp
#define PackageMerge_hpp

#include "Node.hpp"
#include <vector>

struct PackageNode {
	Frequency frequency = 0;
	std::vector<Symbol> symbols;
	
	PackageNode(Symbol s, Frequency f) : frequency(f) { symbols.push_back(s); }
	
	void combine(PackageNode other);
};


// A fast algorithm for optimal length-limited Huffman codes
class PackageMerge {
	std::vector<PackageNode> nodesListOriginal;
	std::vector<PackageNode> nodesListPackaged;
	
public:
	const std::vector<Level> generate(const std::vector<Node*> &input, Level limit);
	
private:
	void package();
	void merge();
	std::vector<Level> calculateLevelList();
};

#endif /* PackageMerge_hpp */
