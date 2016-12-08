#ifndef Node_hpp
#define Node_hpp

#include <vector>

typedef int Symbol;
typedef unsigned int Frequency;
static const Symbol DEFAULT_SYMBOL = -1;

struct Node {
	// Node Vars
	Node* left = nullptr; //0
	Node* right = nullptr; //1
	unsigned short depth = 0;
	// Leaf Vars
	Symbol symbol = DEFAULT_SYMBOL;
	Frequency frequency = 0;
	
	Node() {};
	Node(Symbol symbol, Frequency frequency) : symbol(symbol), frequency(frequency) {}
	Node(Node* left, Node* right, bool swapLeftRight = false);
	
	void print();
	
private:
	inline void accumulateFrequency();
	void printWithDepth(std::vector<Node*> arr, int level);
};


bool sortNode(const Node* node1, const Node* node2);

#endif /* Node_hpp */
