#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

class Solution {
public:
	void moveZeroes(vector<int>& nums) {
		int len = nums.size();
		if (len == 0 || len == 1) {
			return;
		}
		int cur = 0;
		for (int i = 0; i < len; i++) {
			while (nums[cur] == 0 && cur < len) {
				cout << cur << endl;
				cur++;    
			}
			if (cur >= len) {
				break;
			}
			swap(nums[i], nums[cur]);
			cur++;
			if (cur >= len) {
				break;
			}
		}
		return;
	}
};

int main(int argc, char *argv[]) {
	vector<int> nums = {1, 0};
	Solution f;
	f.moveZeroes(nums);
	for (int i = 0; i < nums.size(); i++) {
		cout << i << "value" << nums[i] << endl;
	}
}