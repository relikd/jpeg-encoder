#include "PackageMerge.hpp"
#include <map>

const std::vector<Level> PackageMerge::generate(const std::vector<Node*> &input, Level limit) {
	nodesListOriginal.clear();
	nodesListPackaged.clear();
	nodesListOriginal.reserve(input.size());
	nodesListPackaged.reserve(input.size()*2); // can never be larger than 2*input
	
	for (Node *n : input)
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

/** Insertion Sort insert for newly found packages and original list */
void PackageMerge::merge() {
	size_t i = 0;
	for (PackageNode opn : nodesListOriginal) {
		while (opn.frequency > nodesListPackaged[i].frequency)
			++i;
		nodesListPackaged.insert(nodesListPackaged.begin() + i, opn);
		++i;
	}
}

/** Level list is needed for tree concatenation */
std::vector<Level> PackageMerge::calculateLevelList() {
	std::map<Symbol, Level> levelMap;
	for (PackageNode pn : nodesListPackaged)
		for (Symbol s : pn.symbols)
			levelMap[s] += 1;
	
	std::vector<Level> levelList;
	levelList.reserve(nodesListOriginal.size());
	for (PackageNode on : nodesListOriginal)
		levelList.push_back( levelMap[on.symbols[0]] );
	
	return levelList;
}

