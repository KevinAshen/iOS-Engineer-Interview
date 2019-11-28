#include <iostream>
#include <queue>
#include <stack>

#define random(x) (rand()%x)

typedef struct BTNode {
	int data;
	struct BTNode *lChild;
	struct BTNode *rChild;
}BTNode;

using namespace std;

void PrintNode(BTNode *root) {
	
	if (root != NULL) {
		cout << root -> data << endl;
	}
}

// 先序创建二叉树
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
void PreOrderInt(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	PrintNode(root);
	PreOrderInt(root -> lChild);
	PreOrderInt(root -> rChild);
}

// 借助队列层次遍历，打印一个pop一个
void LevelOrder(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	
	BTNode *p = root;
	queue<BTNode *> s;
	s.push(p);
	while (!s.empty()) {
		int width = s.size();
		for (int i = 0; i < width; i++) {
			p = s.front();
			PrintNode(p);
			s.pop();
			if (p->lChild != NULL) {
				s.push(p->lChild);
			}
			if (p->rChild != NULL) {
				s.push(p->rChild);
			}
		}
	}
}

// 非递归先序遍历，压入一个打印一个
void PreOrder(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	
	BTNode *p = root;
	stack<BTNode *> st;
	while (p != NULL || !st.empty()) {
		while (p != NULL) {
			st.push(p);
			PrintNode(p);
			p = p->lChild;
		}
		if (!st.empty()) {
			p = st.top();
			st.pop();
			p = p->rChild;
		}
	}
}

// 非递归版中续遍历
void InOrder(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	
	BTNode *p = root;
	stack<BTNode *> st;
	while (p != NULL || !st.empty()) {
		while (p != NULL) {
			st.push(p);
			p = p->lChild;
		}
		if (!st.empty()) {
			p = st.top();
			PrintNode(p);
			st.pop();
			p = p->rChild;	
		}
	}
}

// 非递归版后序遍历
void PostOrder(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	
	BTNode *p = root;
	BTNode *q = NULL;
	stack<BTNode *> st;
	while (p != NULL || !st.empty()) {
		while (p != NULL) {
			st.push(p);
			p = p->lChild;
		}
		if (!st.empty()) {
			p = st.top();
			if (p->rChild == NULL || p->rChild == q) {
				st.pop();
				PrintNode(p);
				q = p;
				p = NULL;
			} else {
				p = p->rChild;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	srand((int)time(0));
	BTNode *T;
	T = CreateBiTree(T);
	cout << "构造完成" << endl;
	cout << "开始递归版先序遍历" << endl;
	PreOrderInt(T);
	cout << "开始层次遍历" << endl;
	LevelOrder(T);
	cout << "非递归先序遍历" << endl;
	PreOrder(T);
	cout << "非递归中序遍历" << endl;
	InOrder(T);
	cout << "非递归后序遍历" << endl;
	PostOrder(T);
}