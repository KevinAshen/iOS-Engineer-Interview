#include <iostream>
#include <vector>
#include <cstdio>

using namespace std;

class Solution {
public:
	int missingNumber(vector<int>& nums) {
		int res = 0;
		for (int i = 0; i < nums.size(); ++i) {
			int tmp = (i + 1) ^ nums[i];
			cout << "tmp" << tmp << endl;
			res ^= (i + 1) ^ nums[i];
			cout << res << endl;
		}
		return res;
	}
};

int main(int argc, char *argv[]) {
	Solution f;
	vector<int> nums = {0, 1, 2, 4, 5, 6};
	int res;
	res = f.missingNumber(nums);
	cout << res << endl;
}