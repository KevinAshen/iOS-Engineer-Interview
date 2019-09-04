#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
	vector<int> findDisappearedNumbers(vector<int>& nums) {
		vector<int> res;
		if(nums.empty()) return nums;
		for(int i=0;i<nums.size();i++)
		{
			int index=(nums[i]-1)%nums.size();
			nums[index]+=nums.size();
		}
		for(int i=0;i<nums.size();i++)
		{
			if(nums[i]<=nums.size())
				res.push_back(i+1);
		}
		return res;
	}
};


int main(int argc, char *argv[]) {
	Solution f;
	vector<int> nums = {4, 3, 2, 7, 8, 2, 3, 1};
	vector<int> res = f.findDisappearedNumbers(nums);
	for (int &i : res) {
		cout << i <<endl;
	}
}