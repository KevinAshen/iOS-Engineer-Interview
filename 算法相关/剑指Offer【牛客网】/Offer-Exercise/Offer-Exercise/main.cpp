//
//  main.cpp
//  Offer-Exercise
//
//  Created by Kevin.J on 2019/8/3.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cstdio>
using namespace std;

class Solution {
public:
    bool Find(int target, vector<vector<int> > array) {
        if (array.empty()) {
            return false;
        }
        if (array[0].empty()) {
            return false;
        }
        int m = 0;
        int n = array[0].size() - 1;
        while (target != array[m][n]) {
            printf("OLD:m:%d n:%d\n", m, n);
            if (target > array[m][n]) {
                m++;
            } else {
                n--;
            }
            printf("NEW:m:%d n:%d\n", m, n);
            if (m > array.size() - 1 || n < 0) {
                
                return false;
            }
        }
        return true;
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
//    Solution f;
//    vector<vector<int> > arr = {{1, 2, 8, 9}, {2, 4, 9, 12}, {4, 7, 10, 13}, {6, 8, 11, 15}};
//    printf("结果：%d\n", f.Find(9, arr));
    vector<vector<int>> a = {};
    printf("%d", a[0].empty());
    return 0;
}
