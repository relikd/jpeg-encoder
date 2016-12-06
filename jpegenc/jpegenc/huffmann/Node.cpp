#include "Node.hpp"

Node::Node(Node* l, Node* r, bool swapLeftRight) {
	if (swapLeftRight) {
		left = r;
		right = l;
	} else {
		left = l;
		right = r;
	}
	depth = std::max(r->depth, l->depth) + 1;
	accumulateFrequency();
}

inline void Node::accumulateFrequency() {
	if ((left != nullptr) && (right != nullptr))
		frequency = left->frequency + right->frequency;
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
				printf("%03d", arr[i]->symbol);
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


bool sortNode(const Node* node1, const Node* node2) {
	if (node1->frequency == node2->frequency) {
		return node1->depth < node2->depth;
	}
	return node1->frequency < node2->frequency;
}
