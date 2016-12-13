#include "Node.hpp"

/** Create new Node by combining two existent ones
 * @param l Left Node
 * @param r Right Node
 * @param swapLeftRight If @b true first and second parameter will be flipped. Default: @e false
 */
Node::Node(Node* l, Node* r, bool swapLeftRight) {
	if (swapLeftRight) {
		left = r;
		right = l;
	} else {
		left = l;
		right = r;
	}
	if (!isLeaf())
		frequency = left->frequency + right->frequency;
}

/** Traverse through tree @param rightNode @b true: returns right Node. @b false: returns left Node. */
Node* Node::deeper(bool rightNode) {
	if (rightNode) return right; // 1
	return left; // 0
}

/** Pretty print Node tree to console */
void Node::print() {
	std::vector<Node*> arr;
	arr.push_back(this);
	this->printWithDepth(arr, maxDepth(this));
}

/** Go through all paths and return deepest leaf level */
Level Node::maxDepth(Node* root) {
	if (!root)
		return -1; // unsigned but works properly
	return std::max( 1 + maxDepth(root->left), 1 + maxDepth(root->right) );
}

/** Internal recursive function to print a tree to console */
void Node::printWithDepth(const std::vector<Node*> arr, Level level) {
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
			if (arr[i]->isLeaf())
				printf("%03d", arr[i]->symbol);
			else
				printf(" * ");
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

void Node::climbTree(Node* root, std::vector<Node*> &noteList) {
	if (root->isLeaf()) {
		return;
	} else {
		noteList.push_back(root);
		noteList.push_back(root->left);
		
		noteList.push_back(root);
		noteList.push_back(root->right);
		
		climbTree(root->left, noteList);
		climbTree(root->right, noteList);
	}
	
}

void Node::exportTree() {
	std::vector<Node*> newArr;
	climbTree(this, newArr);
	newArr.size();
	
	printf("graph tree {");
	for (size_t i = 0; i < newArr.size(); i +=2) {
		if (newArr[i]->isLeaf()) {
			printf(  "%d -- \"%p\"; ", newArr[i]->symbol, newArr[i+1]);
		} else if (newArr[i+1]->isLeaf()) {
			printf(  "\"%p\" -- %d; ", newArr[i], newArr[i+1]->symbol);
		} else {
			printf(  "\"%p\" -- \"%p\"; ", newArr[i], newArr[i+1]);
		}
	}
	printf("}\n");
	
}


/** Function for sorting Node* pointer */
bool sortNode(const Node* node1, const Node* node2) {
	return node1->frequency < node2->frequency;
}
