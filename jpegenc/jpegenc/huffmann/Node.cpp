#include "Node.hpp"

void InputWord::increase() {
	++amount;
}

void Node::calculateValue() {
	if ((left != nullptr) && (right != nullptr)) {
		value = InputWord(left->value.amount + right->value.amount);
	}
}

void Node::print() {
	std::vector<Node*> arr;
	arr.push_back(this);
	this->printWithDepth(arr, depth);
}

void Node::printWithDepth(std::vector<Node*> arr, int level) {
	for (int x = (1 << (level+1) ) - 2; x > 0; --x)
		printf(" ");
	
	std::vector<Node*> newArr;
	unsigned long maxWidth = arr.size();
	for (int i = 0; i < maxWidth; i++) {
		
		if (arr[i] == NULL) {
			newArr.push_back(NULL);
			newArr.push_back(NULL);
		} else {
			newArr.push_back(arr[i]->left);
			newArr.push_back(arr[i]->right);
		}
		
		if (arr[i] == NULL) {
			printf("   "); // change the three lines below if you change padding (03)
		} else {
			if (arr[i]->depth > 0)
				printf(" * ");
			else
				printf("%03d", arr[i]->value.symbol);
		}
		
		if (i < maxWidth-1) {
			for (int x = ((((1 << (level)) - 1) * 4) + 1); x > 0; --x) // change 4 if number padding isnt 3
				printf(" ");
		}
	}
	printf("\n");
	if (level > 0)
		printWithDepth(newArr, level-1);
}
