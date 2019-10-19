#include <iostream>
#include <string>
#include <time.h>
#define random(x) (rand()%x)

typedef struct BTNode {
	int data;
	struct BTNode *lChild;
	struct BTNode *rChild;
}BTNode;

using namespace std;

// 工具：输出结点函数
void printBTNodeData(BTNode *root) {
	
	if (root) {
		cout << root -> data << endl;
	}
}

BTNode * CreateBiTree(BTNode *root) {
	
	char ch = 0;
	cout << "输入希望生成的二叉树结构：" << endl;
	cin >> ch;
	if (ch == '#') {
		root = NULL;
	} else {
		root = (BTNode *)malloc(sizeof(BTNode));
		root -> data = random(100);
		cout << "生成随机数：" << root -> data << endl;
		root -> lChild = CreateBiTree(root -> lChild);
		root -> rChild = CreateBiTree(root -> rChild);
	}
	return root;
}

// 递归版先序遍历
void preOrderInternally(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	printBTNodeData(root);
	preOrderInternally(root -> lChild);
	preOrderInternally(root -> rChild);
}



int main(int argc, char *argv[]) {
	
	srand((int)time(0));
	BTNode *T;
	T = CreateBiTree(T);
	cout << "构造完成" << endl;
	cout << "开始递归版先序遍历" << endl;
	preOrderInternally(T);
}