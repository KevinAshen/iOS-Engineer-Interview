#include <iostream>
#include <string>
#include <time.h>
#include <stack>
#include <queue>
#define random(x) (rand()%x)

char words[15] = {'A', 'B', 'D', '#', 'G', '#', '#', '#', 'C', 'E', '#', '#', 'F', '#', '#'};
//char words[15] = {ABD#G###CE##F##};
int p = 0;

typedef struct BTNode {
	int data;
	struct BTNode *lChild;
	struct BTNode *rChild;
}BTNode;

using namespace std;

// 工具：输出结点函数
void PrintBTNodeData(BTNode *root) {
	
	if (root) {
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
void PreOrderInternally(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	PrintBTNodeData(root);
	PreOrderInternally(root -> lChild);
	PreOrderInternally(root -> rChild);
}

// 递归版中序遍历
void InOrderInternally(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	InOrderInternally(root -> lChild);
	PrintBTNodeData(root);
	InOrderInternally(root -> rChild);
}

// 递归版后序遍历
void PostOrderInternally(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	PostOrderInternally(root -> lChild);
	PostOrderInternally(root -> rChild);
	PrintBTNodeData(root);
}

// 借助队列的层次遍历
void LevelOrder(BTNode *root) {
	
	if (root == NULL) {
		return;
	}
	BTNode *p = root;
	queue<BTNode *> s;
	s.push(p);
	while (!s.empty()) {
		p = s.front();
		s.pop();
		PrintBTNodeData(p);
		if (p->lChild != NULL) {
			s.push(p->lChild);
		}
		if (p->rChild != NULL) {
			s.push(p->rChild);
		}
	}
}

// 非递归版先序遍历
void PreOrder(BTNode *root) {
	
	BTNode *p = root;
	stack<BTNode *> st;
	
	while (p != NULL || !st.empty()) {
		while (p != NULL) {
			PrintBTNodeData(p);
			st.push(p);
			p = p->lChild;
		}
		if (!st.empty()) {
			p = st.top();
			st.pop();
			p = p -> rChild;
		}
	}
}

// 非递归版中序遍历
void InOrder(BTNode *root) {
	
	BTNode *p = root;
	stack<BTNode *> st;
	
	while (p != NULL || !st.empty()) {
		while (p != NULL) {
			st.push(p);
			p = p->lChild;
		}
		if (!st.empty()) {
			p = st.top();
			PrintBTNodeData(p);
			st.pop();
			p = p -> rChild;
		}
	}
}

// 非递归版后序遍历
void PostOrder(BTNode *root) {
	
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
				p = st.top();
				st.pop();
				PrintBTNodeData(p);
				q = p;
				p = NULL;
			} else {
				p = p->rChild;
			}
		}
	}
}

// 深度优先搜索计算二叉树的高度
int GetHeightDFS(BTNode *root) {
	
	if (root == NULL) {
		// 这里存在一个问题就是对于叶子结点，他会直接视作将其高度记做1，而高度的定义是边数，也就是说对于只有一个节点的残疾树，我们直接视作高度为0，这点要注意
		return 0;
	}
	int leftHeight = GetHeightDFS(root->lChild);
	int rightHeight = GetHeightDFS(root->rChild);
	return max(leftHeight, rightHeight) + 1;
}

// 后序遍历二叉树计算树的高度
int GetHeightPostOrder(BTNode *root) {
	
	if (root == NULL) {
		return 0;
	}
	int height = 0;
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
				int tmpheight = (int)st.size();
				height = max(height, tmpheight);
				p = st.top();
				st.pop();
				q = p;
				p = NULL;
			} else {
				p = p->rChild;
			}
		}
	}
	return height;
}

// 层次遍历二叉树计算树的高度
int GetHeightLevelOrder(BTNode *root) {
	
	if (root == NULL) {
		return 0;
	}
	int height = 0;
	BTNode *p = root;
	queue<BTNode *> s;
	s.push(p);
	while (!s.empty()) {
		height++;
		int width = s.size();
		for (int i = 0; i < width; i++) {
			p = s.front();
			s.pop();
			if (p->lChild != NULL) {
				s.push(p->lChild);
			}
			if (p->rChild != NULL) {
				s.push(p->rChild);
			}
		}
	}
	return height;
}


int main(int argc, char *argv[]) {
	
	srand((int)time(0));
	BTNode *T;
	T = CreateBiTree(T);
	cout << "构造完成" << endl;
	cout << "开始递归版先序遍历" << endl;
	PreOrderInternally(T);
	cout << "开始递归版中序遍历" << endl;
	InOrderInternally(T);
	cout << "开始递归版后序遍历" << endl;
	PostOrderInternally(T);
	cout << "开始层次遍历" << endl;
	LevelOrder(T);
	cout << "开始非递归版先序遍历" << endl;
	PreOrder(T);
	cout << "开始非递归版中序遍历" << endl;
	InOrder(T);
	cout << "开始非递归版后序遍历" << endl;
	PostOrder(T);
	cout << "end" << endl;
	int DFSHeight = GetHeightDFS(T);
	cout << "深度优先搜索计算二叉树的高度:" << DFSHeight << endl;
	int postOrderHeight = GetHeightPostOrder(T);
	cout << "后序遍历二叉树计算树的高度:" << postOrderHeight << endl;
	int levelOrderHeight = GetHeightLevelOrder(T);
	cout << "层次遍历二叉树计算树的高度:" << levelOrderHeight << endl;
	
	
}