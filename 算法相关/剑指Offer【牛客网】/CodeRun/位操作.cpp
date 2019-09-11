#include <iostream>
#include <vector>
#include <cstdio>

using namespace std;

class Solution {
public:
	int maximumSum(vector<int>& arr) {
		int n = arr.size();
		if (n == 1) {
			return arr[0];
		}
		vector<int> leftMaxSum(n, 0);
		for (int i = 1; i < n; i++) {
			leftMaxSum[i] = max(arr[i - 1], leftMaxSum[i - 1] + arr[i - 1]);
			cout << "leftMaxSum:" << i << " value:" << leftMaxSum[i] << endl;
		}
		vector<int> rightMaxSum(n, 0);
		for (int i = n - 2; i > -1; i--) {
			rightMaxSum[i] = max(arr[i + 1], rightMaxSum[i + 1] + arr[i + 1]);
			cout << "rightMaxSum:" << i << " value:" << rightMaxSum[i] << endl;
		}
		int maxNum = max(leftMaxSum[n - 1], rightMaxSum[0]);
		for (int i = 1; i < n - 1; i++) {
			maxNum = max(maxNum, leftMaxSum[i]);
			maxNum = max(maxNum, rightMaxSum[i]);
			maxNum = max(maxNum, leftMaxSum[i] + rightMaxSum[i]);
		}
		return maxNum;
	}
};

int main(int argc, char *argv[]) {
	Solution f;
	vector<int> nums = {1, 2, 3, 4};
	int res = f.maximumSum(nums);
	cout << res << endl;
}