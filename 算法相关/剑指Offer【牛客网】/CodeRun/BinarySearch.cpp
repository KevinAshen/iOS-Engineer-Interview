#include <iostream>
#include <vector>

using namespace std;

int BinarySearch(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		int mid = left + ((right - left) >> 1);
		if (arr[mid] == value) {
			return mid;
		} else if (arr[mid] < value) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
	return right;
}

int bsearchInternally(vector<int> &arr, int left, int right, int value) {
	if (left > right) {
		return -1;
	}
	
	int mid = left + ((right - left) >> 1);
	if (arr[mid] == value) {
		return mid;
	} else if (arr[mid] < value) {
		return bsearchInternally(arr, mid + 1, right, value);
	} else {
		return bsearchInternally(arr, left, mid - 1, value);
	}
}

// 变体一：查找第一个值等于给定值的元素[hard]
int BinarySearchXH(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (arr[mid] >= value) {
			right = mid - 1;
		} else {
			left = mid + 1;
		} 
	}
	
	if (left < len && arr[left] == value) {
		return left;
	} else {
		return -1;
	}
}

// 变体一：查找第一个值等于给定值的元素[easy]
int BinarySearchXE(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (arr[mid] > value) {
			right = mid - 1;
		} else if (arr[mid] < value) {
			left = mid + 1;
		} else {
			if ((mid == 0) || (arr[mid - 1] != value)) {
				return mid;
			} else {
				right = mid - 1;
			}
		}
	}
	
	return -1;
}

// 变体二：查找最后一个值等于给定值的元素
int BinarySearchY(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (arr[mid] > value) {
			right = mid - 1;
		} else if (arr[mid] < value) {
			left = mid + 1;
		} else {
			if ((mid == len - 1) || (arr[mid + 1] != value)) {
				return mid;
			} else {
				left = mid + 1;
			}
		}
	}
	
	return -1;
}

// 变体三：查找第一个大于等于给定值的元素
int BinarySearchZ(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (arr[mid] >= value) {
			if ((mid == 0) || (arr[mid - 1] < value)) {
				return mid;
			} else {
				right = mid - 1;
			}
		} else {
			left = mid + 1;
		}
	}	
	return -1;
}

// 变体四：查找最后一个小于等于给定值的元素
int BinarySearchZA(vector<int> &arr, int value) {
	
	int len = arr.size();
	int left = 0;
	int right = len - 1;
	int mid = left + ((right - left) >> 1);
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (arr[mid] > value) {
			right = mid - 1;
		} else {
			if ((mid == len - 1) || (arr[mid + 1] > value)) {
				return mid;
			} else {
				left = mid + 1;
			}
		}
	}
	
	return -1;
}

int search(vector<int>& nums, int target) {
	int left = 0, right = nums.size() - 1;
	while (left <= right) {
	int mid = left + (right - left) / 2;
	if (nums[mid] == target) return mid;
		else if (nums[mid] < nums[right]) {
			if (nums[mid] < target && nums[right] >= target) left = mid + 1;
			else right = mid - 1;
		} else {
			if (nums[left] <= target && nums[mid] > target) right = mid - 1;
			else left = mid + 1;
		}
	}
		return -1;
}

int main(int argc, char *argv[]) {
	vector<int> arr = {4,5,6,7,0,1,2};
	int res = search(arr, 0);
	cout << res << endl;
}