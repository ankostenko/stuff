/**
	Rope data structure.
	Represents by binary tree with substrings on the leaf
	and intermidiante nodes that consider the size of left
	node.(actually i don't know why)

	short LEAF_LEN = 6;

	  1) Create new root     Hello my ;  
							 nananana;

			6					2																									   4
		6		3			2		6																								4	  4
	  Hello 	my			na	  nanana      (it's example, in a program string devided by 2 so we will have to strings like this nana  nana)


	  2) Concatenate 2 roots

					9
			6				2
		6		3		2		5
	  Hello 	my		na	  nanan


	  So we have string concatenate for O(1)
*/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

const int LEAF_LEN = 2;

struct Rope {
	Rope* left, * right, * parent;
	char* string;
	int lCount;

	Rope(Rope* left = NULL, Rope* right = NULL, Rope* parent = NULL, int lCount = 0, char* string = NULL): 
		left(left), right(right), parent(parent), lCount(lCount), string(string) {}

	~Rope() {
		delete[] string;
		delete left;
		delete right;
		delete parent;
	}
};

/*
	if u use strlen() for r then subract 1 , if u use sizeof(arr) / sizeof(elem) then subtract 2
*/

void createRope(Rope*& node, Rope* par, char* str, int l, int r) {
	
	node = new Rope();

	node->parent = par;

	if ((r - l) > LEAF_LEN) {
		node->lCount = (r - l) / 2;
		int m = (r + l) / 2;
		createRope(node->left, node, str, l, m);
		createRope(node->right, node, str, m + 1, r);
	}
	else {
		node->lCount = r - l;
		int j = 0;
		node->string = new char[r - l + 2]; // for '\0'
		for (int i = l; i <= r; i++) {
			node->string[j++] = str[i];
		}
		node->string[j] = '\0';
	}
}

void show(Rope* node) {
	if (!node)
		return;

	if (!node->left && !node->right)
			std::cout << node->string;

	show(node->left);
	show(node->right);
}

void concat(Rope*& generalroot, Rope* root1, Rope* root2, int strLen1) {
	generalroot = new Rope();
	generalroot->lCount = strLen1;
	generalroot->left = root1;
	generalroot->right = root2;
}


int main() {
	
	char str1[] = "Lol this ";
	Rope* root1;
	int strlen1 = sizeof(str1) / sizeof(*str1);
	createRope(root1, NULL, str1, 0, strlen1 - 2); // -1 '\0' and -1 for indx started from 0

	char str2[] = "shhit gona work";
	Rope* root2;
	int strlen2 = sizeof(str2) / sizeof(*str2);
	createRope(root2, NULL, str2, 0, strlen2 - 2);

	Rope* root3;
	concat(root3, root1, root2, strlen1);

	show(root3);
	
}
