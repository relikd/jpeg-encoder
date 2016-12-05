#ifndef Node_hpp
#define Node_hpp

#include <vector>

typedef int Symbol;
static const Symbol DEFAULT_SYMBOL = -1;

struct InputWord {
	Symbol symbol = DEFAULT_SYMBOL;
	unsigned int amount = 0;
	
	InputWord(unsigned int _amount, Symbol _symbol = DEFAULT_SYMBOL) : amount(_amount), symbol(_symbol) {}
	
	void increase();
	
	bool operator  < (const InputWord& input) const { return (amount  < input.amount); }
	bool operator == (const InputWord& input) const { return (amount == input.amount); }
};

struct Node {
	Node* left = nullptr; //0
	Node* right = nullptr; //1
	InputWord value = NULL;
	unsigned short depth = 0;
	
	Node(InputWord a) : value(a) {}
	Node(InputWord a, InputWord b) : left(new Node(a)), right(new Node(b)), depth(1), value(InputWord(a.amount + b.amount)) {}
	Node(Node* _left, Node* _right, bool swapLeftRight = false) {
		if (swapLeftRight) {
			left = _right;
			right = _left;
		} else {
			left = _left;
			right = _right;
		}
		depth = std::max(right->depth, left->depth) + 1;
		calculateValue();
	}
	
	void calculateValue();
	void print();
	
private:
	void printWithDepth(std::vector<Node*> arr, int level);
};


#endif /* Node_hpp */
