#include <iostream>
#include <vector>

using namespace std;

int Partition(vector<int> &arr, int left, int right) {
	
	int pivot = arr[right];
	int i = left;
	for (int j = left; j < right; j++) {
		if (arr[j] < pivot) {
			swap(arr[i], arr[j]);
			i++;
		}
	}
	
	swap(arr[i], arr[right]);
	return i;
}

void QuickSort(vector<int> &arr, int left, int right) {
	
	if (left >= right) {
		return;
	}
	
	int pivot = Partition(arr, left, right);
	QuickSort(arr, left, pivot - 1);
	QuickSort(arr, pivot + 1, right);
}

void QuickSort(vector<int> &arr) {
	
	int len = arr.size();
	if (len == 0) {
		return;
	}
	
	QuickSort(arr, 0, len - 1);
}

int main(int argc, char *argv[]) {
	
	vector<int> arr(10, 0);
	for (int &i : arr) {
		cin >> i;
	}
	//vector<int> arr = {4, 2, 1, 7, 5, 9};
	QuickSort(arr);
	for (int i : arr) {
		cout << i << endl;
	}
}


