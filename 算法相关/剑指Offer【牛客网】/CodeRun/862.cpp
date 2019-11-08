#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class Solution {
public:
	int shortestSubarray(vector<int>& A, int K) {
		int n = A.size(), res = INT_MAX;
		deque<int> dq;
		vector<int> sums(n + 1);
		for (int i = 1; i <= n; ++i) sums[i] = sums[i - 1] + A[i - 1];
		//打印nums的内容
		for (int i = 0; i < sums.size(); i++) {
			cout << "sums[" << i << "] :"  << sums[i] << '\t';
		}
		cout << "\n";
		for (int i = 0; i <= n; ++i) {
			cout << "当前的i是:" << i << endl;
			cout << "NO1 打印DP START:" << endl;
			//打印deque的内容
			for (int i = 0; i < dq.size(); i++) {
				cout << "dq[" << i << "] :"  << dq[i] << '\t';
			}
			cout << "\n";
			cout << "NO1 打印DP END:" << endl;

			while (!dq.empty() && sums[i] - sums[dq.front()] >= K) {
				cout << "NO2 打印DP START:" << endl;
				//打印deque的内容
				for (int i = 0; i < dq.size(); i++) {
					cout << "dq[" << i << "] :"  << dq[i] << '\t';
				}
				cout << "\n";
				cout << "NO2 打印DP END:" << endl;
				cout << "OLD res:" << res << endl;
				res = min(res, i - dq.front());
				cout << "NEW res:" << res << endl;
				dq.pop_front();
			}
			while (!dq.empty() && sums[i] <= sums[dq.back()]) {
				cout << "NO3 打印DP START:" << endl;
				//打印deque的内容
				for (int i = 0; i < dq.size(); i++) {
					cout << "dq[" << i << "] :"  << dq[i] << '\t';
				}
				cout << "\n";
				cout << "NO3 打印DP END:" << endl;
				dq.pop_back();
			}
			cout << "NO4 打印DP START:" << endl;
			//打印deque的内容
			for (int i = 0; i < dq.size(); i++) {
				cout << "dq[" << i << "] :"  << dq[i] << '\t';
			}
			cout << "\n";
			cout << "NO4 打印DP END:" << endl;
			dq.push_back(i);
		}
		return res == INT_MAX ? -1 : res;
	}
};
int main(int argc, char *argv[]) {
	Solution f;
	vector<int> arr = {168};
	int res = f.shortestSubarray(arr, 167);
	cout << res << endl;
}