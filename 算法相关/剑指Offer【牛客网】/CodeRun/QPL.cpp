#include <iostream>
#include <vector>

#define maxNum 100000

using namespace std;

void fun(vector<int> &arr, vector<vector<int>> &res, int i) {
	
	if (i == arr.size()) {
		res.push_back(arr);
	} 
	for (int j = i; j < arr.size(); j++) {
		swap(arr[i], arr[j]);
		fun(arr, res, i + 1);
		swap(arr[i], arr[j]);
	}
}

int main(int argc, char *argv[]) {
	
	string str;
	getline(cin, str);
	
	vector<int> arr;
	int tmpNum = 0;
	for (int i = 0; i < str.length(); i++) {
		char a = str[i];
		if (a != ' ') {
			int tmp = a - '0';
			tmpNum = 10 * tmpNum + tmp;
		} else {
			arr.push_back(tmpNum);
			tmpNum = 0;
		}
		if (i == str.length() - 1) {
			arr.push_back(tmpNum);
		}
	}
	
	
	if (arr.empty()) {
		return 0;
	}

	vector<vector<int>> res;
	fun(arr, res, 0);
	
	
	for (vector<int> &arr : res) {
		for (int i : arr) {
			cout << i;
		}
		cout << ' ';
	}
}