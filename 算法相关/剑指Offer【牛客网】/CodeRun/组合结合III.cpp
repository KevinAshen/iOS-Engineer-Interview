#include <iostream>
#include <vector>
#include <cstdio>

using namespace std;

class Solution {
public:
	void DFS(vector<vector<int>> &res, int target, int start, vector<int> out, int num) {
		if (target == 0 && num == 0) {
			res.push_back(out);
			return;
		}
		if (target < 0 || num < 0) {
			return;
		}
		
		for (int i = start; i < 10; i++) {
			out.push_back(i);
			DFS(res, target - i, i + 1, out, num - 1);
			out.pop_back();
		}
	}
	
	vector<vector<int>> combinationSum3(int k, int n) {
		vector<vector<int>> res;
		if (n > 9) {
			return res;
		}
		DFS(res, n, 1, {}, k);
		
		return res;
	}
};


void ABC(int a, int b) {
	cout << "123" << endl;
	cout << a << endl;
	cout << b << endl;
}

int main(int argc, char *argv[]) {
	int a = 3;
	int b = 4;
	ABC(--a, b--);
}