#ifndef Node_hpp
#define Node_hpp

#include <vector>

typedef int Symbol;
typedef unsigned int Frequency;
typedef unsigned short Level;

static const Symbol DEFAULT_SYMBOL = -1;

class Node {
public:
	// Node Vars
	Node* left = nullptr;  // 0
	Node* right = nullptr; // 1
	// Leaf Vars
	Symbol symbol = DEFAULT_SYMBOL;
	Frequency frequency = 0;
	
	Node() {};
	Node(Symbol symbol, Frequency frequency) : symbol(symbol), frequency(frequency) {}
	Node(Node* left, Node* right, bool swapLeftRight = false);
	
	
	inline bool isLeaf() { return (left == nullptr && right == nullptr); };
	Node* deeper(bool rightNode);
	void print();
	void exportTree();
	
private:
	Level maxDepth(Node* root);
	void printWithDepth(const std::vector<Node*> arr, Level level);
	void climbTree(Node* root, std::vector<Node*> &nodeList);
};


bool sortNode(const Node* node1, const Node* node2);

#endif /* Node_hpp */
