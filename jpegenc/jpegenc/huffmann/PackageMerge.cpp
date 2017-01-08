#include "PackageMerge.hpp"
#include <map>

/**
 * Sum frequencies and merge symbols
 * @param other If possible use lighter Node here (faster)
 */
void PackageNode::combine(PackageNode other) {
	frequency += other.frequency;
	for (const Symbol &s : other.symbols)
		symbols.push_back(s);
}

/**
 * Perform the Package Merge algorithm
 * @param input Original Nodes list. Should be leafs only and sorted by frequency (ascending)
 * @param limit The maximal depth for the Huffman tree
 * @return Level list with each leaf's depth. Same order as inut
 */
const std::vector<Level> PackageMerge::generate(const std::vector<Node*> &input, Level limit) {
	nodesListOriginal.clear();
	nodesListPackaged.clear();
	nodesListOriginal.reserve(input.size());
	nodesListPackaged.reserve(input.size()*2); // can never be larger than 2*input
	
	for (const Node *n : input)
		nodesListOriginal.push_back( PackageNode(n->symbol, n->frequency) );
	
	// start off with the original list and evolve package list with each level
	nodesListPackaged = nodesListOriginal;
	
	for (int i = limit; i > 0; --i) {
		package();
		if (i > 1) // why should we merge in the last step? (just all symbols +1)
			merge();
	}
	return calculateLevelList();
}

/** Make new packages by combining pairs together */
void PackageMerge::package() {
	size_t index = nodesListPackaged.size();
	
	if (index & 1) { // odd number of elements
		--index; // round to lower even number. Speaking: discard any non pair
		nodesListPackaged.erase(nodesListPackaged.begin()+index); // delete last unpaired element
	}
	
	while (index) {
		index -= 2; // jump through pairwise
		nodesListPackaged[index+1].combine(nodesListPackaged[index]);
		nodesListPackaged.erase(nodesListPackaged.begin()+index);
	}
}

/** Insertion Sort insert for original list with newly found packages */
void PackageMerge::merge() {
	size_t o = nodesListOriginal.size();
	size_t p = nodesListPackaged.size();
	while (o--) {
		while (p && nodesListOriginal[o].frequency <= nodesListPackaged[p-1].frequency)
			--p;
		nodesListPackaged.insert(nodesListPackaged.begin() + p, nodesListOriginal[o]);
	}
}

/** Sum all symbols in all packages to create a level list */
std::vector<Level> PackageMerge::calculateLevelList() {
	std::map<Symbol, Level> levelMap;
	for (const PackageNode &pn : nodesListPackaged)
		for (const Symbol &s : pn.symbols)
			levelMap[s] += 1;
	
	std::vector<Level> levelList;
	levelList.reserve(nodesListOriginal.size());
	for (const PackageNode &on : nodesListOriginal)
		levelList.push_back( levelMap[on.symbols[0]] );
	
	return levelList;
}

