#include <iostream>

using namespace std;

struct ListNode {
	int value;	
	ListNode *next;
};

int main(int argc, char *argv[]) {
	
	ListNode *head = new ListNode;
	head->next = NULL;
	
	ListNode *last = head;
	
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
		node->value = j;
		last -> next = node;
		last = node;
	}
	last -> next = NULL;
	
	ListNode *pNode = head->next;
	while (pNode != NULL) {
		cout << pNode->value << endl;
		pNode = pNode->next;
	}
	
	pNode = head->next;
	
	ListNode *pre = NULL;
	ListNode *slow = pNode;
	ListNode *fast = pNode;
	
	while (fast != NULL && fast->next != NULL) {
		fast = fast->next->next;
		ListNode *next = slow->next;
		slow->next = pre;
		pre = slow;
		slow = next;
	}
	
	if (fast != NULL) {
		slow = slow->next;
	}
	
	while (slow != NULL) {
		if (slow->value != pre->value) {
			cout << "FALSE" << endl;
			break;
		}
		slow = slow->next;
		pre = pre->next;
	}
	cout << "TRUE" << endl;
}