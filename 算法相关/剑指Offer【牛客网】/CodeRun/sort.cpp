#include <iostream>
#include <vector>

using namespace std;


// 插入排序
void insertionSort(vector<int> &arr) {
	
	int arrLen = arr.size();
	if (arrLen == 0) {
		return ;
	}
	for (int i = 1; i < arrLen; i++) {
		int value = arr[i];
		int j = i - 1;
		for (; j >= 0; j--) {
			if (arr[j] > value) {
				arr[j + 1] = arr[j];
			} else {
				break;
			}
		}
		arr[j + 1] = value;
	}	
}

// 冒泡排序
void bubbleSort(vector<int> &arr) {
	
	int arrLen = arr.size();
	if (arrLen == 0) {
		return ;
	}	
	for (int i = 0; i < arrLen; i++) {
		bool flag = false;
		for (int j = 0; j < arrLen - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(arr[j], arr[j + 1]);
				flag = true;
			}
		}
		if (!flag) {
			break;
		}
	}
	return ;
}

// 选择排序
void selectionSort(vector<int> &arr) {
	int arrLen = arr.size();
	if (arrLen == 0) {
		return ;
	}
	for (int i = 0; i < arrLen; i++) {
		int minNum = arr[i];
		int minIndex = i;
		for (int j = i; j < arrLen; j++) {
			if (minNum > arr[j]) {
				minNum = arr[j];
				minIndex = j;
			}
		}
		swap(arr[i], arr[minIndex]);
	}
}

// 希尔排序
void shellSort(vector<int> &arr) {
	
	int arrLen = arr.size();
	if (arrLen == 0) {
		return ;
	}
	
	int d = arrLen / 2;
	int x, j, k = 1;
	while (d >= 1) {
		for (int i = d; i < arrLen; i++) {
			x = arr[i];
			j = i - d;
			// 直接插入排序，会向前找所适合的位置
			while (j >= 0 && arr[j] > x) {
				// 交换位置
				arr[j + d] = arr[j];
				j = j - d;
			}
			arr[j + d] = x;
		}
		d = d / 2;
	}
}

// 合并排序
void merge(vector<int> &arr, int l, int mid, int r) {
	int help[r - l + 1];
	int lIndex = l;
	int rIndex = mid + 1;
	int i = 0;
	// 两边都还没有到头的情况
	while (lIndex <= mid && rIndex <= r) {
		help[i++] = arr[lIndex] < arr[rIndex] ? arr[lIndex++] : arr[rIndex++];
	}
	// 右边已经到头的情况
	while (lIndex <= mid) {
		help[i++] = arr[lIndex++];
	}
	// 左边已经到头的情况
	while (rIndex <= r) {
		help[i++] = arr[rIndex++];
	}
	// 将辅助数组里的数据写入原数组
	for (i = 0; i < r - l + 1; i++) {
		arr[l + i] = help[i];
	}
}

static void mergeSort(vector<int> &arr, int l, int r) {
	if (l == r) {
		return;
	}
	int mid = (l + r) / 2;
	mergeSort(arr, l, mid);
	mergeSort(arr, mid + 1, r);
	merge(arr, l, mid, r);
}

void mergeSort(vector<int> &arr) {
	int arrLen = arr.size();
	if (arrLen == 0) {
		return ;
	}
	mergeSort(arr, 0, arrLen - 1);
}

// 快速排序
int partition(vector<int> &arr, int p, int r) {
	int pivot = arr[r];
	int i = p;
	for (int j = p; j < r; j++) {
		if (arr[j] < pivot) {
			swap(arr[i], arr[j]);
			i++;
		}
	}
	swap(arr[i], arr[r]);
	return i;
}

static void quickSort(vector<int> &arr, int p, int r) {
	if (p >= r) {
		return;
	}
	
	int q = partition(arr, p, r);
	quickSort(arr, p, q - 1);
	quickSort(arr, q + 1, r);
}

void quickSort(vector<int> &arr) {
	
	int arrLen = arr.size();
		if (arrLen == 0) {
			return ;
		}
	quickSort(arr, 0, arrLen - 1);
}

// 堆排序
void HeapifyUpToDown(vector<int> &arr, int k, int i) {
	
	while (true) {
		int maxPos = i;
		if (i * 2 <= k && arr[i] < arr[i * 2]) {
			maxPos = i * 2;
		}
		if (i * 2 + 1 <= k && arr[maxPos] < arr[i * 2 + 1]) {
			maxPos = i * 2 + 1;
		}
		if (maxPos == i) {
			break;
		}
		swap(arr[i], arr[maxPos]);
		i = maxPos;
	}
}

// 从下往上建堆，即不停执行插入操作
void CreateHeapDownToUp(vector<int> &arr) {
	
	int arrLen = arr.size();
}

// 从上往下建堆，即从后往前遍历数组，进行堆化
void CreateHeapUpToDown(vector<int> &arr) {
	
	int arrLen = arr.size();
	for (int i = arrLen / 2; i >= 1; i--) {
		HeapifyUpToDown(arr, arrLen, i);
	}
	for (int &i : arr) {
		cout << i << endl;
	}
}



void HeapSort(vector<int> &arr) {
	
	int arrLen = arr.size();
	if (arrLen == 0) {
		return;
	}
	CreateHeapUpToDown(arr);
	int k = arrLen - 1;
	while (k > 1) {
		swap(arr[1], arr[k]);
		k--;
		HeapifyUpToDown(arr, k, 1);
	}
	
}

int main(int argc, char *argv[]) {
	vector<int> arr = {5, 5, 5, 19, 8, 4, 1, 20, 13, 16};
	insertionSort(arr);
	cout << "\n" << endl;
	for (int &i : arr) {
		cout << i << endl;
	}
}