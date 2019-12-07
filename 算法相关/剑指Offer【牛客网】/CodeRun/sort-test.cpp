#include <iostream>
#include <vector>

using namespace std;

// 归并
void merge(vector<int> &arr, int left, int mid, int right) {
	
	int len = right - left + 1;
	int help[len];
	int leftIndex = left;
	int rightIndex = mid + 1;
	int i = 0;
	while (leftIndex <= mid && rightIndex <= right) {
		help[i++] = arr[leftIndex] < arr[rightIndex] ? arr[leftIndex++] : arr[rightIndex++];
	}
	
	while (leftIndex <= mid) {
		help[i++] = arr[leftIndex++];
	}
	
	while (rightIndex <= right) {
		help[i++] = arr[rightIndex++];
	}
	
	for (int j = 0; j < len; j++) {
		arr[left + j] = help[j];
	}
}

void mergeSort(vector<int> &arr, int left, int right) {
	
	if (left >= right) {
		return;
	}
	int mid = left + ((right - left) >> 1);
	mergeSort(arr, left, mid);
	mergeSort(arr, mid + 1, right);
	merge(arr, left, mid, right);
}

void mergeSort(vector<int> &arr) {
	
	int len = arr.size();
	if (len == 0) {
		return;
	}
	
	mergeSort(arr, 0, len - 1);
}

// 快排
int partition(vector<int> &arr, int left, int right) {
	
	int pivot = arr[right];
	int i = left;
	for (int j = left; j < right; j++) {
		if (arr[i] < pivot) {
			swap(arr[i], arr[j]);
			i++;
		}
	}  
	swap(arr[i], arr[right]);
	return i;
}

void quickSort(vector<int> &arr, int left, int right) {
	
	if (left >= right) {
		return;
	}
	
	int pivot = partition(arr, left, right);
	quickSort(arr, left, pivot - 1);
	quickSort(arr, pivot + 1, right);
}

void quickSort(vector<int> &arr) {
	
	int len = arr.size();
	quickSort(arr, 0, len - 1);
}

int main(int argc, char *argv[]) {
	
	//vector<int> arr(10, 0);
//	for (int &i : arr) {
//		cin >> i;
//	}
	vector<int> arr = {1, 1, 6, 6, 8, 3, 5, 100, 300, 200, 99, 99};
	mergeSort(arr);
	for (int i : arr) {
		cout << i << endl;
	}
}


