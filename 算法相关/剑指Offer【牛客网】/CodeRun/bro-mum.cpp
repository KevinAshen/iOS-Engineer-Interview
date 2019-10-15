#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
	vector<int> fraction(vector<int>& cont) {
		int coutLen = cont.size();
		if (coutLen == 1) {
			return {cont[0], 1};
		}
		int mum = cont[coutLen - 1];
		int son = 1;
		int bro = 1;
		for (int i = coutLen - 1; i > 1; i--) {
			bro = son;
			son = 1;
			cout << "old:son " << son << endl;
			cout << "old:mum " << mum << endl;
			cout << "\n" << endl;
			son *= mum;
			mum = mum * cont[i - 1] + bro;
			cout << "new:son " << son << endl;
			cout << "new:mum " << mum << endl;
			cout << "\n" << endl;
			
		}
		vector<int> res = {son, mum};
		return res;
	}
};

int main(int argc, char *argv[]) {
	Solution f;
	vector<int> res;
	vector<int> arr = {0, 0, 3};
	res = f.fraction(arr);
	for (int &i : res) {
		cout << i << endl;
	}
	
	
}