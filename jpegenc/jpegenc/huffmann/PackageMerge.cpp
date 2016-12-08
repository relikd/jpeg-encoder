#include "PackageMerge.hpp"
#include <map>

Node* PackageMerge::generate(const std::vector<Node*> &input, unsigned short limit) {
	nodesListOriginal.clear();
	for (Node *n : input)
		nodesListOriginal.push_back( PackageNode(n->symbol, n->frequency) );
	
	// start off with the original list and evolve package list with each level
	nodesListPackaged = nodesListOriginal;
	
	for (int i = limit; i > 0; --i) {
		package();
		if (i > 1) // why should we merge in the last step? (just all symbols +1)
			merge();
	}
	std::vector<unsigned short> levelList = calculateLevelList();
	
	return generateTree(levelList, input);
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
std::vector<unsigned short> PackageMerge::calculateLevelList() {
	std::map<Symbol, int> levelMap;
	for (PackageNode pn : nodesListPackaged)
		for (Symbol s : pn.symbols)
			levelMap[s] += 1;
	
	std::vector<unsigned short> levelList;
	for (PackageNode on : nodesListOriginal)
		levelList.push_back( levelMap[on.symbols[0]] );
	
	return levelList;
}

// TODO: refactor this method ... or get rid of it
Node* PackageMerge::generateTree(const std::vector<unsigned short>& levelList, const std::vector<Node*> &nodeList) {
	size_t count = levelList.size();
	if (count != nodeList.size()) {
		fputs("Something is wrong. LevelList and NodeList should always be in sync\n", stderr);
		return nullptr;
	}
	
	if (count <= 1)
		return nodeList[0]; // tree generation done
	
	std::vector<unsigned short> newLevelList;
	std::vector<Node*> newNodeList;
	unsigned int currentLevel = levelList[0];
	for (int i = 0; i < count; ++i) {
		if (i+1 < count && levelList[i] == currentLevel && levelList[i+1] == currentLevel) {
			newLevelList.push_back(levelList[i] - 1);
			newNodeList.push_back( new Node(nodeList[i+1], nodeList[i]) );
			++i;
		} else { // dont merge anything, just copy
			newLevelList.push_back(levelList[i]);
			newNodeList.push_back(nodeList[i]);
		}
	}
	
	if (levelList == newLevelList) {
		fputs("Something went wrong. TODO: prevent a too shallow depth limit here\n", stderr);
		return nullptr;
	}
	
	return generateTree(newLevelList, newNodeList);
}

