#include <iostream>

using namespace std;

struct ListNode {
	int value;	
	ListNode *next;
};

int main(int argc, char *argv[]) {
	ListNode *head = new ListNode;
	head->next = NULL;
	
	int size = 0;
	
	cout << "输入节点个数：" << endl;
	cin >> size;
	cout << "输入的节点个数为：" << size << endl;
	
	int i = size;
	while (i--) {
		int j;
		cout << "输入第"<< size - i << "个节点：" << endl;
		cin >> j;
		ListNode *node = new ListNode; 
	}
}